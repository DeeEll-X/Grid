// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <experimental/filesystem>
#include <jsoncpp/json/json.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "args.hpp"
#include "system.hpp"

namespace Grid {
namespace fs = std::experimental::filesystem;
class Container {
 public:
  struct Config {
    std::string mBundle;
  };

  explicit Container(System &sys) : mSystem(sys) {}
  void Create(const std::string &id, const std::string &bundle,
              const std::string &syncPath);
  int Kill();
  void Sync();
  void NewWorkSpace();
  void CreateWriteLayer();
  void CreateMountPoint();

 private:
  void LoadStatusFile();
  void LoadConfig();
  System &mSystem;
  Config mConfig;
  std::string mOciVersion;
  std::string mId;
  std::bitset<sizeof(Status)> mState;
  int64_t mPid{0};
  std::string mBundle;
  std::map<std::string, std::string> mAnnotations;
  fs::path mRootPath;
  // System &mSystem;
};
}  // namespace Grid
