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
namespace Grid {
// rootdir/containers/id/->mntpath
void Container::Create(const std::string &id, const std::string &bundle,
                       const fs::path &rootPath) {
  mId = id;
  mBundle = bundle;
  LoadConfig();
  mStatus = CREATED;
  mRootPath = rootPath;
  NewWorkSpace();  // add mnt URL and root URL
  LOG(INFO) << "creating container: "
            << " id:" << mId << " bundle:" << mBundle
            << " contentPath:" << mRootPath;
  // write file
  Sync();
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
  pid_t child_process = clone(InitProcess, stackTop,
                              CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
                                  CLONE_NEWNET | CLONE_NEWIPC | SIGCHLD,
                              // | CLONE_NEWUSER
                              this);

  if (child_process < 0) {
    throw std::runtime_error(strerror(errno));
  }
  mPid = child_process;
  mStatus = RUNNING;
  Sync();
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
    throw std::runtime_error("kill failed: container is not running");
}

void Container::State() {
  Json::Value root;
  StateToJson(root);
  Json::StyledWriter styledWriter;
  std::cout << styledWriter.write(root);
}

void Container::Delete() {
  // kill pid
  if (mStatus == RUNNING) {
    throw std::runtime_error("delete failed: container is running");
  }
  // DeleteMountPoint(containerName)
  umount((mRootPath / "mntFolder").c_str());
  // DeleteWriteLayer(containerName)
  fs::remove_all(mRootPath);
}

void Container::Restore(const fs::path &rootPath) {
  mRootPath = rootPath;
  LoadStatusFile();
  AmendStatus();
  LoadConfig();
}

int InitProcess(void *c) {
  auto container = static_cast<Container *>(c);
  auto &process = container->mConfig.mProcess;
  if (!process.terminal) {
    // work as daemon process
    // setsid && close fin, ferr
    // output to logfile
    setsid();
    close(0);
    close(1);
    int fin = open("/dev/null", O_RDONLY);
    if (fin < 0) throw std::runtime_error(strerror(errno));
    auto logPath = fs::path(container->mRootPath) / "logFile";
    int logFd = open(logPath.c_str(), O_CREAT | O_WRONLY);
    if (logFd < 0) throw std::runtime_error(strerror(errno));
    dup2(logFd, 2);
    fchmod(logFd, 0644);
    chdir(process.cwd.c_str());
    if (flock(1, LOCK_EX | LOCK_NB)) {
      throw std::runtime_error("start failed: logfile is locked!");
    }
  }

  // readUserCommand();
  container->SetUpMount();
  char *const *args = new char *[process.args.size() + 1];
  auto args_it = (const char **)args;
  for (auto &it : process.args) {
    *args_it = it.c_str();
    args_it++;
  }
  *args_it = nullptr;

  char *const *envs = new char *[process.env.size() + 1];
  auto envs_it = (const char **)envs;
  for (auto &it : process.env) {
    *envs_it = it.c_str();
    envs_it++;
  }
  *envs_it = nullptr;

  // execve
  execvpe(args[0], args, envs);

  throw std::runtime_error(strerror(errno));
}

void Container::SetUpMount() {
  PivotRoot(mRootPath / "mntFolder");
  mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, "");
  mount("tmpfs", "/dev", "tmpfs", MS_NOSUID | MS_STRICTATIME, "mode=755");
}

void Container::PivotRoot(const std::string &root) {
  // 为了使当前root的老 root 和新 root
  // 不在同一个文件系统下，我们把root重新mount了一次 bind
  // mount是把相同的内容换了一个挂载点的挂载方法
  if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == 1) {
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
  fs::create_directories(mRootPath / "writeLayer");
}

void Container::CreateMountPoint() {
  fs::path mntPath{mRootPath};
  mntPath /= "mntFolder";
  fs::create_directories(mntPath);
  fs::path wLayerPath{mRootPath};
  wLayerPath /= "writeLayer";
  mSystem.MountAUFS(wLayerPath.generic_string(), mBundle,
                    mntPath.generic_string());
}

void Container::LoadStatusFile() {
  fs::path statusPath{mRootPath / "status.json"};
  std::ifstream statusfile(statusPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!statusfile.is_open()) {
    throw std::runtime_error("statusFile cannot open!");
  }
  if (reader.parse(statusfile, root)) {
    mOciVersion = root["OCIVersion"].asString();
    mId = root["ID"].asString();
    mStatus = StringToStatus(root["Status"].asString());
    mPid = root["Pid"].asInt64();
    mBundle = root["Bundle"].asString();
    mConfig.mBundle = mBundle;
    std::vector<std::string> memberNames = root["Annotations"].getMemberNames();
    for (auto &it : memberNames) {
      mAnnotations[it] = root["Annotations"][it].asString();
    }
  }
  statusfile.close();
}

void Container::LoadConfig() {
  mConfig.mBundle = mBundle;
  fs::path configPath;
  configPath /= mBundle;
  configPath /= "config.json";
  std::ifstream configFile(configPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!configFile.is_open()) {
    throw std::runtime_error("configFile cannot open!");
  }
  if (reader.parse(configFile, root)) {
    mConfig.mProcess.parse(root["process"]);
  }
  configFile.close();
}

void Container::StateToJson(Json::Value &root) {
  root["OCIVersion"] = mOciVersion;
  root["ID"] = mId;
  root["Status"] = StatusToString(mStatus);
  // root["Status"] = StatusToString()
  root["Pid"] = Json::Value::Int64(mPid);
  root["Bundle"] = mBundle;
  for (auto &it : mAnnotations) {
    root["Annotations"][it.first] = it.second;
  }
}

void Container::Sync() {
  Json::Value root;
  StateToJson(root);

  std::ofstream statusFile;
  fs::path statusPath{mRootPath};
  statusPath /= "status.json";
  statusFile.open(statusPath);
  if (!statusFile.is_open()) {
    throw std::runtime_error("statusFile cannot open!");
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
    auto logPath = fs::path(mRootPath) / "logFile";
    int logFd = open(logPath.c_str(), O_CREAT | O_WRONLY);
    if (flock(logFd, LOCK_EX | LOCK_NB)) {
      mPid = 0;
      mStatus == STOPPED;
      Sync();
    }
    close(logFd);
  }
}

}  // namespace Grid
