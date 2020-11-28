// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <experimental/filesystem>
#include <string>

namespace Grid {
namespace Utils {

void MakeDir(const fs::path &dir) {
  std::experimental::filesystem::create_directory(dir);
}

}  // namespace Utils
}  // namespace Grid
