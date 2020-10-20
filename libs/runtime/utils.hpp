// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <experimental/filesystem>
#include <string>

void MakeDir(const std::string &dir) {
  std::experimental::filesystem::create_directory(dir);
}
