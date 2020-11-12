// Copyright [2020] <DeeEll-X/Veiasai>"
#include <stdio.h>
#include <stdlib.h>

#include <glog/logging.h>

#include <iostream>

#include <cli/parser.hpp>
#include <runtime/core.hpp>
#include <runtime/system.hpp>

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);
  const char *json = getenv("GRID_CONFIG");
  if (json == nullptr) {
    std::cout << "environment variable grid_config not exist" << std::endl;
    return 1;
  }
  Grid::Config config = Grid::Config::LoadFromJson(json);
  Grid::System sys;
  Grid::Core core{config, sys};
  core.Initialize();
  Grid::Cli::Parser parser;
  // Initialize Google’s logging library.

  try {
    auto args = parser.parse(argc, argv);
    auto ret = core.Exec(*args);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
