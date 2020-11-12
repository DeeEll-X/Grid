// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <stdlib.h>

#include <stdexcept>
#include <string>

namespace Grid {
class System {
 public:
  virtual void Exec(const std::string &command) {
    if (system(command.c_str())) {
      throw std::runtime_error("system exec fail!");
    }
  }

  virtual void MountAUFS(const std::string &writelayer,
                         const std::string &rolayer,
                         const std::string &mntfolder) {
    std::string branches = "br:" + writelayer + "=rw:" + rolayer + "=ro";
    std::string command = "mount -t aufs -o " + branches + " none " + mntfolder;
    // mount -t aufs -o br:mrootpath/writeLayer=rw:mbundle=ro none
    // mrootpath/mntFolder
    if (system(command.c_str()) == -1) {
      throw std::runtime_error("aufs mount fail!");
    }
  }

  virtual ~System() {}
};
}  // namespace Grid
