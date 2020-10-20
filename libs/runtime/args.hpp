// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <string>

namespace Grid {
enum Status { CREATING, CREATED, RUNNING, STOPPED };
class Args {};

class Ret {};

class CreateArgs : public Args {
 public:
  std::string mContainerId;
  std::string mBundlePath;
  CreateArgs() = default;
  CreateArgs(const std::string &containerId, const std::string &bundlePath)
      : mContainerId(containerId), mBundlePath(bundlePath) {}
};

class CreateRet : public Ret {
 public:
  std::string mContainerId;
  int64_t status;
};

class StartArgs : public Args {
 public:
  std::string mContainerId;
};

class StartRet : public Ret {
 public:
  std::string mContainerId;
  int64_t status;
};
}  // namespace Grid
