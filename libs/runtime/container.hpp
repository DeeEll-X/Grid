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
  };

  explicit Container(System &sys) : mSystem(sys) {}
  void Create(const std::string &id, const std::string &bundle,
              const fs::path &rootPath);
  void Start();
  void Kill(const int signal);
  void State();
  void Delete();
  void Restore(const fs::path &rootPath);

 private:
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
  System &mSystem;
  Config mConfig;
  std::string mOciVersion{""};
  std::string mId{""};
  Status mStatus{CREATING};
  int64_t mPid{0};
  std::string mBundle{""};
  std::map<std::string, std::string> mAnnotations;
  fs::path mRootPath;
  // System &mSystem;
};
}  // namespace Grid
