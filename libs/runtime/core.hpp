// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <map>
#include <string>

#include "args.hpp"
#include "config.hpp"
#include "container.hpp"

namespace Grid {
class Core {
 private:
  CreateRet Exec(const CreateArgs &args);
  StartRet Exec(const StartArgs &args);
  std::map<std::string, Container> mContainerMap;
  Config mConfig;

 public:
  explicit Core(const Config &);
  Ret Exec(const Args &args) { return StartRet(); }
  void Initialize();
};
}  // namespace Grid
