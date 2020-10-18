// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <string>

namespace Grid {
class Args {};

class Ret {};

class CreateArgs : public Args {
  std::string mBundlePath;
};

class CreateRet : public Ret {
  int64_t mContainerId;
  int64_t status;
};

class StartArgs : public Args {
  int64_t mContainerId;
};

class StartRet : public Ret {
  int64_t mContainerId;
  int64_t status;
};
}  // namespace Grid
