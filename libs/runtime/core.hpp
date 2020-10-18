// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include "args.hpp"

namespace Grid {
class Core {
 private:
  CreateRet exec(const CreateArgs& args);
  StartRet exec(const StartArgs& args);

 public:
  Core();
  Ret exec(const Args& args) { return StartRet(); }
};
}  // namespace Grid
