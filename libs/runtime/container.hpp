// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <jsoncpp/json/json.h>
#include <experimental/filesystem>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <bitset>

#include "args.hpp"

namespace Grid {
namespace fs = std::experimental::filesystem;
class Container {
 public:
  struct Config {
    std::string mBundle;
  };

  Container() = default;
  void Create(const std::string &id, const std::string &bundle,
              const std::string &syncPath);
  int Kill();
  void Sync();

 private:
  void LoadStatusFile();
  void LoadConfig();
  Config mConfig;
  std::string mOciVersion;
  std::string mId;
  std::bitset<sizeof(Status)> mState;
  int64_t mPid{0};
  std::string mBundle;
  std::map<std::string, std::string> mAnnotations;
  fs::path mStatusPath;
};
}  // namespace Grid
