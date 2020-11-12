// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <map>
#include <memory>
#include <string>

#include "args.hpp"
#include "config.hpp"
#include "container.hpp"

namespace Grid {
class Core {
 public:
  explicit Core(const Config &, System &);

  struct RootDir {
    explicit RootDir(const fs::path &p)
        : mRoot(p),
          mContainers(mRoot / "containers"),
          mImages(mRoot / "images") {}

    void Initialize();
    fs::path mRoot;
    fs::path mContainers;
    fs::path mImages;
  };

  std::unique_ptr<Ret> Exec(const Args &args) {
    switch (args.GetType()) {
      case Args::argsType::Create:
        return Exec(static_cast<const CreateArgs &>(args));
      case Args::argsType::Start:
        return Exec(static_cast<const StartArgs &>(args));
      case Args::argsType::Kill:
        return Exec(static_cast<const KillArgs &>(args));

      default:
        break;
    }
  }
  void Initialize();

 private:
  std::unique_ptr<CreateRet> Exec(const CreateArgs &args);
  std::unique_ptr<StartRet> Exec(const StartArgs &args);
  std::unique_ptr<KillRet> Exec(const KillArgs &args);
  std::map<std::string, std::unique_ptr<Container>> mContainerMap;
  Config mConfig;
  System &mSystem;
  RootDir mRootDir;
};
}  // namespace Grid
