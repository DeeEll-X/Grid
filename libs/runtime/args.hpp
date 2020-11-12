// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <signal.h>

#include <string>

namespace Grid {
enum Status { CREATING, CREATED, RUNNING, STOPPED };
class Args {
 public:
  enum argsType { Create, Start, Kill, Delete, State };
  virtual argsType GetType() const = 0;
};

class Ret {
 public:
  enum retType { Create, Start, Kill, Delete, State };
  virtual retType GetType() const = 0;
};

class CreateArgs : public Args {
 public:
  CreateArgs() = default;
  CreateArgs(const std::string &containerId, const std::string &bundlePath)
      : mContainerId(containerId), mBundlePath(bundlePath) {}
  argsType GetType() const override { return argsType::Create; }
  std::string mContainerId;
  std::string mBundlePath;
};

class CreateRet : public Ret {
 public:
  CreateRet(const std::string &containerId, const int64_t stat)
      : mContainerId(containerId), status(stat) {}
  retType GetType() const override { return retType::Create; }
  std::string mContainerId;
  int64_t status;
};

class StartArgs : public Args {
 public:
  explicit StartArgs(const std::string &containerId)
      : mContainerId(containerId) {}
  argsType GetType() const override { return argsType::Start; }
  std::string mContainerId;
};

class StartRet : public Ret {
 public:
  StartRet(const std::string &containerId, const int64_t stat)
      : mContainerId(containerId), status(stat) {}
  retType GetType() const override { return retType::Start; }
  std::string mContainerId;
  int64_t status;
};

class KillArgs : public Args {
 public:
  KillArgs(const std::string &containerId, const int signal)
      : mContainerId(containerId), mSignal(signal) {}
  argsType GetType() const override { return argsType::Kill; }
  std::string mContainerId;
  int mSignal;
};

class KillRet : public Ret {
 public:
  KillRet(const std::string &containerId, const int64_t stat)
      : mContainerId(containerId), status(stat) {}
  retType GetType() const override { return retType::Kill; }
  std::string mContainerId;
  int64_t status;
};
}  // namespace Grid
