// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <map>
#include <string>
#include <memory>

#include "args.hpp"
#include "config.hpp"
#include "container.hpp"

namespace Grid {
class Core {
 private:
  std::unique_ptr<CreateRet> Exec(const CreateArgs &args);
  std::unique_ptr<StartRet> Exec(const StartArgs &args);
  std::map<std::string, std::unique_ptr<Container>> mContainerMap;
  Config mConfig;
  System& mSystem;

 public:
  explicit Core(const Config &, System &);
  std::unique_ptr<Ret> Exec(const Args &args) {
    switch (args.GetType()) {
    case Args::argsType::Create:
      return Exec(static_cast<const CreateArgs &>(args));
    case Args::argsType::Start:
      return Exec(static_cast<const StartArgs &>(args));
    default:
      break;
    }
  }
  void Initialize();
};
}  // namespace Grid
