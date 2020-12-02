// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <signal.h>

#include <string>

namespace Grid {
enum Status { CREATING, CREATED, RUNNING, STOPPED };
std::string StatusToString(Status stat);
Status StringToStatus(const std::string &stat);
class Args {
 public:
  enum argsType { Create, Start, Kill, State, Delete };
  virtual argsType GetType() const = 0;
};

class Ret {
 public:
  enum retType { Create, Start, Kill, State, Delete };
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
  CreateRet(const std::string &containerId) : mContainerId(containerId) {}
  retType GetType() const override { return retType::Create; }
  std::string mContainerId;
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
  StartRet(const std::string &containerId) : mContainerId(containerId) {}
  retType GetType() const override { return retType::Start; }
  std::string mContainerId;
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
  KillRet(const std::string &containerId) : mContainerId(containerId) {}
  retType GetType() const override { return retType::Kill; }
  std::string mContainerId;
};

class StateArgs : public Args {
 public:
  explicit StateArgs(const std::string &containerId)
      : mContainerId(containerId) {}
  argsType GetType() const override { return argsType::State; }
  std::string mContainerId;
};

class StateRet : public Ret {
 public:
  explicit StateRet(const std::string &containerId)
      : mContainerId(containerId) {}
  retType GetType() const override { return retType::State; }
  std::string mContainerId;
};

class DeleteArgs : public Args {
 public:
  explicit DeleteArgs(const std::string &containerId)
      : mContainerId(containerId) {}
  argsType GetType() const override { return argsType::Delete; }
  std::string mContainerId;
};

class DeleteRet : public Ret {
 public:
  explicit DeleteRet(const std::string &containerId)
      : mContainerId(containerId) {}
  retType GetType() const override { return retType::Delete; }
  std::string mContainerId;
};
}  // namespace Grid
