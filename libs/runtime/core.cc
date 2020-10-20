// Copyright [2020] <DeeEll-X/Veiasai>"
#include <algorithm>
#include <vector>
#include <string>

#include "core.hpp"
#include "container.hpp"
#include "utils.hpp"
namespace Grid {
static std::string MakeContainersSyncPath(const std::string &rootDir) {
  return rootDir + "/containers";
}
Core::Core(const Config &c) : mConfig(c) {}

void Core::Initialize() {
  std::vector<std::string> vec{MakeContainersSyncPath(mConfig.mRootDir)};
  std::for_each(vec.begin(), vec.end(), MakeDir);
}

CreateRet Core::Exec(const CreateArgs &args) {
  // load config and add to map
  auto it = mContainerMap.emplace(args.mContainerId, Container{});
  mContainerMap[args.mContainerId].Create(
      args.mContainerId, args.mBundlePath,
      MakeContainersSyncPath(mConfig.mRootDir));
  // changed
}

StartRet Core::Exec(const StartArgs &args) {}
}  // namespace Grid
