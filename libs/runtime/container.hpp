// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <jsoncpp/json/json.h>

#include <bitset>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "args.hpp"
#include "system.hpp"

namespace Grid {
namespace fs = std::experimental::filesystem;

int InitProcess(void *);

class Container {
 public:
  struct Config {
    std::string mBundle{""};
    struct Process {
      void parse(const Json::Value &root) {
        terminal = root["terminal"].asBool();
        cwd = root["cwd"].asString();

        for (int i = 0; i < root["env"].size(); ++i) {
          env.emplace_back(root["env"][i].asString());
        }

        for (int i = 0; i < root["args"].size(); ++i) {
          args.emplace_back(root["args"][i].asString());
        }
      }

      bool terminal{false};
      std::string cwd{""};
      std::vector<std::string> env{};
      std::vector<std::string> args{};
    } mProcess;

    struct HookEle {
      fs::path path{""};
      std::vector<std::string> args{};
      std::vector<std::string> env{};
      void parse(const Json::Value &root) {
        path = root["path"].asString();
        for (int i = 0; i < root["env"].size(); ++i) {
          env.emplace_back(root["env"][i].asString());
        }
        for (int i = 0; i < root["args"].size(); ++i) {
          args.emplace_back(root["args"][i].asString());
        }
      }
    };

    enum HookType {
      PRESTART,
      CREATE_RUNTIME,
      CREATE_CONTAINER,
      START_CONTAINER,
      POSTSTART,
      POSTSTOP
    };
    std::map<HookType, std::vector<HookEle>> mHooks;
    HookType StringToHookType(const std::string &hookstr) {
      if (hookstr == "prestart") {
        return HookType::PRESTART;
      } else if (hookstr == "createRuntime") {
        return HookType::CREATE_RUNTIME;
      } else if (hookstr == "createContainer") {
        return HookType::CREATE_CONTAINER;
      } else if (hookstr == "startContainer") {
        return HookType::START_CONTAINER;
      } else if (hookstr == "poststart") {
        return HookType::POSTSTART;
      } else if (hookstr == "poststop") {
        return HookType::POSTSTOP;
      }
      throw std::runtime_error("unknown hook type: " + hookstr);
    }
    void ParseHooks(const Json::Value &root) {
      Json::Value::Members members = root.getMemberNames();
      for (Json::Value::Members::iterator it = members.begin();
           it != members.end(); it++) {
        std::string hookName = *it;
        HookType hookType = StringToHookType(hookName);
        for (int i = 0; i < root[hookName].size(); ++i) {
          mHooks[hookType].emplace_back();
          mHooks[hookType].back().parse(root[hookName][i]);
        }
      }
    }
  };

  explicit Container(System &sys) : mSystem(sys) {}
  void Create(const std::string &id, const std::string &bundle,
              const fs::path &rootPath);
  void Start();
  void SetNS();
  void Kill(const int signal);
  void State(Json::Value &);
  void Delete();
  void Restore(const fs::path &rootPath);
  fs::path GetRootPath() { return mContainerDir.mRootPath; }

 private:
  friend int CreateNamespace(void *);
  friend int InitProcess(void *);
  friend class ContainerVisitor;
  void SetUpMount();
  void PivotRoot(const std::string &);
  void LoadStatusFile();
  void LoadConfig();
  void NewWorkSpace();
  void CreateWriteLayer();
  void CreateMountPoint();
  void StateToJson(Json::Value &);
  void Sync();
  void AmendStatus();
  void Destroy();
  void RunHook(Config::HookType);
  System &mSystem;
  Config mConfig;
  std::string mOciVersion{""};
  std::string mId{""};
  Status mStatus{CREATING};
  int64_t mPid{0};
  std::string mBundle{""};
  std::map<std::string, std::string> mAnnotations;
  struct ContainerDir {
    fs::path mRootPath{""};
    fs::path mMntFolder{""};
    fs::path mWriteLayer{""};
    fs::path mNSMountFolder{""};
    fs::path mStatusFilePath{""};
    void Initialize(fs::path rootPath) {
      mRootPath = rootPath;
      mMntFolder = rootPath / "mntFolder";
      mWriteLayer = rootPath / "writeLayer";
      mNSMountFolder = rootPath / "ns";
      mStatusFilePath = rootPath / "status.json";
    }
  } mContainerDir;
  // System &mSystem;
};
}  // namespace Grid
