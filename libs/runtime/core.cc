// Copyright [2020] <DeeEll-X/Veiasai>"
#include "core.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "container.hpp"
#include "utils.hpp"

namespace Grid {

void Core::RootDir::Initialize() {
  Utils::MakeDir(mRoot);
  Utils::MakeDir(mContainers);
  Utils::MakeDir(mImages);
}

Core::Core(const Config &c, System &s)
    : mConfig(c), mSystem(s), mRootDir(mConfig.mRootDir) {}

void Core::Initialize() {
  mRootDir.Initialize();

  for (const auto &containerDir :
       fs::directory_iterator(mRootDir.mContainers)) {
    // TODO(DeeEll-X): check containerDir is dir
    auto it = mContainerMap.emplace(containerDir.path().filename(),
                                    std::make_unique<Container>(mSystem));
    it.first->second->Restore(containerDir);
  }
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
      mRootDir.mContainers / args.mContainerId);
  return std::make_unique<CreateRet>(args.mContainerId);
}

std::unique_ptr<StartRet> Core::Exec(const StartArgs &args) {
  auto it = mContainerMap.find(args.mContainerId);
  if (it == mContainerMap.end()) {
    throw std::runtime_error(
        "start container fail: container not found! containerid: " +
        args.mContainerId);
  }
  it->second->Start();
  return std::make_unique<StartRet>(args.mContainerId);
}

std::unique_ptr<KillRet> Core::Exec(const KillArgs &args) {
  auto it = mContainerMap.find(args.mContainerId);
  if (it == mContainerMap.end()) {
    throw std::runtime_error(
        "start container fail: container not found! containerid: " +
        args.mContainerId);
  }
  it->second->Kill(args.mSignal);
  return std::make_unique<KillRet>(args.mContainerId);
}

std::unique_ptr<StateRet> Core::Exec(const StateArgs &args) {
  auto it = mContainerMap.find(args.mContainerId);
  if (it == mContainerMap.end()) {
    throw std::runtime_error(
        "start container fail: container not found! containerid: " +
        args.mContainerId);
  }
  Json::Value jsonval;
  it->second->State(jsonval);
  return std::make_unique<StateRet>(args.mContainerId, jsonval);
}

std::unique_ptr<DeleteRet> Core::Exec(const DeleteArgs &args) {
  auto it = mContainerMap.find(args.mContainerId);
  if (it == mContainerMap.end()) {
    throw std::runtime_error(
        "start container fail: container not found! containerid: " +
        args.mContainerId);
  }
  it->second->Delete();
  return std::make_unique<DeleteRet>(args.mContainerId);
}
}  // namespace Grid
