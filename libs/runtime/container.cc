// Copyright [2020] <DeeEll-X/Veiasai>"
#include "container.hpp"

#include <fcntl.h>
#include <glog/logging.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <vector>
static const int STACK_SIZE = (1024 * 1024); /* Stack size for cloned child */
static const std::vector<std::string> namespaces{"uts", "ipc", "net",
                                                 "mnt"};  // pid
static const std::vector<long> nsflags{CLONE_NEWUTS, CLONE_NEWIPC, CLONE_NEWNET,
                                       CLONE_NEWNS};  // CLONE_NEWPID
namespace Grid {
static void Exec(const std::string &path, const std::vector<std::string> &vargs,
                 const std::vector<std::string> &venvs) {
  char *const *args = new char *[vargs.size() + 1];
  auto args_it = (const char **)args;
  for (const auto &it : vargs) {
    *args_it = it.c_str();
    args_it++;
  }
  *args_it = nullptr;

  char *const *envs = new char *[venvs.size() + 1];
  auto envs_it = (const char **)envs;
  for (const auto &it : venvs) {
    *envs_it = it.c_str();
    envs_it++;
  }
  *envs_it = nullptr;
  execvpe(path.c_str(), args, envs);
}

int CreateNamespace(void *c);
// rootdir/containers/id/->mntpath
void Container::Create(const std::string &id, const std::string &bundle,
                       const fs::path &rootPath) {
  mId = id;
  mBundle = bundle;
  LoadConfig();
  LOG(INFO) << "Container::Create: Config loaded" << std::endl;
  mStatus = CREATING;
  mContainerDir.Initialize(rootPath);
  LOG(INFO) << "Container::Create: ContainerDirs initialized" << std::endl;

  Sync();

  NewWorkSpace();  // add mnt URL and root URL
  LOG(INFO) << "Container::Create: writeLayer and mntFolder created"
            << std::endl;
  LOG(INFO) << "Container::Create: creating container"
            << " id:" << mId << " bundle:" << mBundle
            << " contentPath:" << mContainerDir.mRootPath;

  /* Allocate stack for child */
  char *stack = new char[STACK_SIZE];  /* Start of stack buffer */
  char *stackTop = stack + STACK_SIZE; /* End of stack buffer */
  pid_t child_process = clone(CreateNamespace, stackTop,
                              CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID |
                                  CLONE_NEWNET | CLONE_NEWIPC | SIGCHLD,
                              // | CLONE_NEWUSER
                              this);

  auto nspath = (fs::path{"/proc"} / std::to_string(child_process)) / "ns";
  fs::path dst = mContainerDir.mNSMountFolder;
  fs::create_directory(dst);
  LOG(INFO) << "Container::Create: ns mount point: " << dst << " created"
            << std::endl;

  if (mount(dst.c_str(), dst.c_str(), "bind", MS_BIND | MS_REC, "")) {
    Destroy();
    throw std::runtime_error("create namespace fail: bind namespace fail");
  }
  if (mount(nullptr, dst.c_str(), nullptr, MS_PRIVATE | MS_REC, nullptr)) {
    Destroy();
    throw std::runtime_error(
        "create namespace fail: change mount point to private fail");
  }
  LOG(INFO) << "Container::Create: ns mount point mounted itself private"
            << std::endl;

  for (const auto &ns : namespaces) {
    std::ofstream nsfile(dst / ns);
    nsfile.close();
    if (mount((nspath / ns).c_str(), (dst / ns).c_str(), "", MS_BIND,
              nullptr)) {
      Destroy();
      throw std::runtime_error("bind namespace fail: " + ns + strerror(errno));
    }
  }
  LOG(INFO) << "Container::Create: new namespaces mounted" << std::endl;

  RunHook(Config::HookType::CREATE_RUNTIME);
  // std::cout<<"createruntime hook finished"<<std::endl;
  kill(child_process, SIGALRM);
  waitpid(child_process, nullptr, 0);

  mStatus = CREATED;
  // write file
  Sync();
}

static void SigAlrm(int signo) {
  LOG(INFO) << "CreateNamespace: SIFGALRM received" << std::endl;
}

int CreateNamespace(void *c) {
  Container *container = static_cast<Container *>(c);
  if (signal(SIGALRM, SigAlrm) == SIG_ERR) {
    container->Destroy();
    throw std::runtime_error(
        "create namespace: register sigalrm handler fail " +
        std::string(strerror(errno)));
  }
  pause();
  container->RunHook(Container::Config::HookType::CREATE_CONTAINER);
  return 0;
}

void Container::Start() {
  if (mStatus == RUNNING) {
    throw std::runtime_error("this container is already running!");
  }
  /* Allocate stack for child */
  char *stack = new char[STACK_SIZE];  /* Start of stack buffer */
  char *stackTop = stack + STACK_SIZE; /* End of stack buffer */

  /* Create child that has its own UTS namespace;
    child commences execution in childFunc() */
  pid_t child_process = clone(InitProcess, stackTop, CLONE_NEWPID | SIGCHLD,
                              // | CLONE_NEWUSER
                              this);

  if (child_process < 0) {
    throw std::runtime_error("Container::start fail: clone initprocess fail " +
                             std::string(strerror(errno)));
  }
  LOG(INFO) << "Container::Start: child_process" << child_process << std::endl;
  mPid = child_process;
  mStatus = RUNNING;
  Sync();
  RunHook(Config::HookType::POSTSTART);  // TODO: watch exec
  if (mConfig.mProcess.terminal) {
    siginfo_t siginfo;
    waitid(P_PID, child_process, &siginfo, WEXITED);
    mStatus = STOPPED;
    Sync();
  }
}

void Container::Kill(const int signal) {
  if (mStatus == RUNNING)
    kill(mPid, signal);
  else
    throw std::runtime_error(
        "Container::Kill failed: container is not running");
}

void Container::State(Json::Value &jsonval) { StateToJson(jsonval); }

void Container::Delete() {
  if (mStatus == RUNNING) {
    throw std::runtime_error("Container::Delete failed: container is running");
  }
  Destroy();
  RunHook(Config::HookType::POSTSTOP);
}

void Container::Restore(const fs::path &rootPath) {
  mContainerDir.Initialize(rootPath);
  LoadStatusFile();
  AmendStatus();
  LoadConfig();
}

void Container::SetNS() {
  for (int i = 0; i < namespaces.size(); ++i) {
    int fd =
        open((mContainerDir.mNSMountFolder / namespaces[i]).c_str(), O_RDONLY);
    setns(fd, nsflags[i]);
    close(fd);
  }
}

int InitProcess(void *c) {
  auto container = static_cast<Container *>(c);
  container->SetNS();
  auto &process = container->mConfig.mProcess;
  if (!process.terminal) {
    // work as daemon process
    // setsid && close fin, ferr
    // output to logfile
    setsid();
    close(0);
    close(1);
    int fin = open("/dev/null", O_RDONLY);
    if (fin < 0) {
      throw std::runtime_error(
          "Container::Start fail: InitProcess fail: cannot open /dev/null" +
          std::string(strerror(errno)));
    }
    auto logPath = container->GetRootPath() / "logFile";
    int logFd = open(logPath.c_str(), O_CREAT | O_WRONLY);
    if (logFd < 0) {
      throw std::runtime_error(
          "Container::Start fail: InitProcess fail: cannot open logfile" +
          std::string(strerror(errno)));
    }
    dup2(logFd, 2);
    fchmod(logFd, 0644);
    chdir(process.cwd.c_str());
    if (flock(1, LOCK_EX | LOCK_NB)) {
      throw std::runtime_error(
          "Container::Start fail: InitProcess fail: logfile is locked!");
    }
  }

  // readUserCommand();
  container->SetUpMount();
  container->RunHook(Container::Config::START_CONTAINER);
  Exec(process.args[0], process.args, process.env);

  throw std::runtime_error(
      "Container::Start fail: InitProcess fail: reach end of function");
}

void Container::SetUpMount() {
  PivotRoot(mContainerDir.mMntFolder);
  mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, "");
  mount("tmpfs", "/dev", "tmpfs", MS_NOSUID | MS_STRICTATIME, "mode=755");
}

void Container::PivotRoot(const std::string &root) {
  // 为了使当前root的老 root 和新 root
  // 不在同一个文件系统下，我们把root重新mount了一次 bind
  // mount是把相同的内容换了一个挂载点的挂载方法
  if (mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr)) {
    throw std::runtime_error("pivotroot fail: mount root fail");
  }

  if (mount(root.c_str(), root.c_str(), "bind", MS_BIND | MS_REC, "")) {
    throw std::runtime_error("pivotroot fail: bind mount root fail");
  }
  // 创建 rootfs/.pivot_root 存储 old_root
  fs::path pivotDir{root};
  pivotDir /= ".pivot_root";
  fs::create_directory(pivotDir);
  // pivot_root 到新的rootfs, 现在老的 old_root 是挂载在rootfs/.pivot_root
  // 挂载点现在依然可以在mount命令中看到
  if (syscall(SYS_pivot_root, root.c_str(), pivotDir.c_str())) {
    throw std::runtime_error(
        std::string("pivotroot fail: pivot_root to new root fail: ") +
        strerror(errno));
  }
  // 修改当前的工作目录到根目录
  if (chdir("/")) {
    throw std::runtime_error("pivotroot fail: chdir to / fail");
  }
  pivotDir = "/";
  pivotDir /= ".pivot_root";
  // umount rootfs/.pivot_root
  if (umount2(pivotDir.generic_string().c_str(), MNT_DETACH)) {
    throw std::runtime_error("pivotroot fail: umount pivot_dir dir fail");
  }
  // 删除临时文件夹
  remove(pivotDir);
}

void Container::NewWorkSpace() {
  CreateWriteLayer();
  CreateMountPoint();
}

void Container::CreateWriteLayer() {
  fs::create_directories(mContainerDir.mWriteLayer);
}

void Container::CreateMountPoint() {
  fs::create_directories(mContainerDir.mMntFolder);
  mSystem.MountAUFS(mContainerDir.mWriteLayer, mBundle,
                    mContainerDir.mMntFolder);
}

void Container::LoadStatusFile() {
  std::ifstream statusfile(mContainerDir.mStatusFilePath,
                           std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!statusfile.is_open()) {
    throw std::runtime_error("loadStatusFile fail: statusFile cannot open!");
  }
  if (reader.parse(statusfile, root)) {
    mOciVersion = root["ociVersion"].asString();
    mId = root["id"].asString();
    mStatus = StringToStatus(root["status"].asString());
    mPid = root["pid"].asInt64();
    mBundle = root["bundle"].asString();
    mConfig.mBundle = mBundle;
    std::vector<std::string> memberNames = root["annotations"].getMemberNames();
    for (const auto &it : memberNames) {
      mAnnotations[it] = root["annotations"][it].asString();
    }
  }
  statusfile.close();
}

void Container::LoadConfig() {
  mConfig.mBundle = mBundle;
  fs::path configPath{mBundle};
  configPath /= "config.json";
  std::ifstream configFile(configPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!configFile.is_open()) {
    throw std::runtime_error("configFile cannot open!");
  }
  if (reader.parse(configFile, root)) {
    mConfig.mProcess.parse(root["process"]);
    mConfig.ParseHooks(root["hooks"]);
  }
  configFile.close();
}

void Container::StateToJson(Json::Value &root) {
  root["ociVersion"] = mOciVersion;
  root["id"] = mId;
  root["status"] = StatusToString(mStatus);
  // root["Status"] = StatusToString()
  root["pid"] = Json::Value::Int64(mPid);
  root["bundle"] = mBundle;
  for (const auto &it : mAnnotations) {
    root["annotations"][it.first] = it.second;
  }
}

void Container::Sync() {
  Json::Value root;
  StateToJson(root);
  if (access(mContainerDir.mRootPath.c_str(), 0)) {
    fs::create_directories(mContainerDir.mRootPath);
  }

  std::ofstream statusFile(mContainerDir.mStatusFilePath);
  if (!statusFile.is_open()) {
    throw std::runtime_error("Sync fail: statusFile cannot open!");
  }

  Json::StyledWriter styledWriter;
  statusFile << styledWriter.write(root);
  statusFile.close();
}

void Container::AmendStatus() {
  if (mStatus == RUNNING) {
    if (!mPid) {
      throw std::runtime_error("state is running but pid = 0");
    }
    auto logPath = fs::path(mContainerDir.mRootPath) / "logFile";
    int logFd = open(logPath.c_str(), O_CREAT | O_WRONLY);
    if (flock(logFd, LOCK_EX | LOCK_NB)) {
      mPid = 0;
      mStatus == STOPPED;
      Sync();
    }
    close(logFd);
  }
}

void Container::Destroy() {
  // DeleteMountPoint(containerName)
  umount(mContainerDir.mMntFolder.c_str());
  for (const auto &ns : namespaces) {
    umount((mContainerDir.mNSMountFolder / ns).c_str());
  }
  umount(mContainerDir.mNSMountFolder.c_str());
  // DeleteWriteLayer(containerName)
  fs::remove_all(mContainerDir.mRootPath);
}

void Container::RunHook(Config::HookType type) {
  const auto &hooks = mConfig.mHooks[type];
  for (const auto &hook : hooks) {
    pid_t childProcess = vfork();
    if (childProcess < 0) {
      Destroy();
      throw std::runtime_error(strerror(errno));
    }
    if (childProcess == 0) {
      auto statusfile = open(mContainerDir.mStatusFilePath.c_str(), O_RDONLY);
      dup2(statusfile, STDIN_FILENO);
      Exec(hook.path, hook.args, hook.env);
    }
    // TODO: check child process status
  }
}
}  // namespace Grid
