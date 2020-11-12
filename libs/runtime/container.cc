// Copyright [2020] <DeeEll-X/Veiasai>"
#include <glog/logging.h>
#include <sys/mount.h>
#include <vector>

#include "container.hpp"

namespace Grid {
// rootdir/containers/id/->mntpath
void Container::Create(const std::string &id, const std::string &bundle,
                       const std::string &containersSyncPath) {
  mId = id;
  mBundle = bundle;
  LoadConfig();
  mState.reset();
  mState[CREATED] = true;
  mRootPath.clear();
  mRootPath /= containersSyncPath;
  mRootPath /= id;
  NewWorkSpace();  // add mnt URL and root URL
  LOG(INFO) << "creating container: "
            << " id:" << mId << " bundle:" << mBundle
            << " contentPath:" << mRootPath;
  // write file
  Sync();
}

void Container::NewWorkSpace() {
  CreateWriteLayer();
  CreateMountPoint();
}

void Container::CreateWriteLayer() {
  fs::path wLayerPath{mRootPath};
  wLayerPath /= "writeLayer";
  fs::create_directories(wLayerPath);
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
  fs::path statusPath{mRootPath};
  statusPath /= "status.json";
  std::ifstream statusfile(statusPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!statusfile.is_open()) {
    throw std::runtime_error("statusFile cannot open!");
  }
  if (reader.parse(statusfile, root)) {
    this->mOciVersion = root["OCIVersion"].asString();
    this->mId = root["ID"].asString();
    mState[CREATED] = root["Created"].asBool();
    mState[RUNNING] = root["Running"].asBool();
    mState[STOPPED] = root["Stopped"].asBool();
    mState[CREATING] = root["Creating"].asBool();
    this->mPid = root["Pid"].asInt64();
    this->mBundle = root["Bundle"].asString();
    this->mConfig.mBundle = this->mBundle;
    std::vector<std::string> memberNames = root["Annotations"].getMemberNames();
    for (auto &it : memberNames) {
      mAnnotations[it] = root["Annotations"][it].asString();
    }
  }
  statusfile.close();
}

void Container::LoadConfig() {
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
    /* to add */
  }
  configFile.close();
}

void Container::Sync() {
  Json::Value root;

  root["OCIVersion"] = mOciVersion;
  root["ID"] = mId;
  root["Created"] = static_cast<bool>(mState[CREATED]);
  root["Running"] = static_cast<bool>(mState[RUNNING]);
  root["Stopped"] = static_cast<bool>(mState[STOPPED]);
  root["Creating"] = static_cast<bool>(mState[CREATING]);
  root["Pid"] = Json::Value::Int64(mPid);
  root["Bundle"] = mBundle;
  for (auto &it : mAnnotations) {
    root["Annotations"][it.first] = it.second;
  }

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

}  // namespace Grid
