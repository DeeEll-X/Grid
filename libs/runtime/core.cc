// Copyright [2020] <DeeEll-X/Veiasai>"
#include <algorithm>
#include <string>
#include <vector>

#include "container.hpp"
#include "core.hpp"
#include "utils.hpp"

namespace Grid {
static std::string MakeContainersSyncPath(const std::string &rootDir) {
  return rootDir + "/containers";
}
Core::Core(const Config &c, System &s) : mConfig(c), mSystem(s) {}

void Core::Initialize() {
  std::vector<std::string> vec{MakeContainersSyncPath(mConfig.mRootDir)};
  std::for_each(vec.begin(), vec.end(), MakeDir);
}

std::unique_ptr<CreateRet> Core::Exec(const CreateArgs &args) {
  // load config and add to map
  auto it = mContainerMap.emplace(args.mContainerId,
                                  std::make_unique<Container>(mSystem));
  if (!it.second) {
    throw std::runtime_error(
        "emplace container to the map fail: containerid: " + args.mContainerId);
  }
  mContainerMap[args.mContainerId]->Create(
      args.mContainerId, args.mBundlePath,
      MakeContainersSyncPath(mConfig.mRootDir));
  return std::make_unique<CreateRet>(args.mContainerId, 0);
}

std::unique_ptr<StartRet> Core::Exec(const StartArgs &args) {
  return std::make_unique<StartRet>(args.mContainerId, 0);
}
}  // namespace Grid
