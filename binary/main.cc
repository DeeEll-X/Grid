// Copyright [2020] <DeeEll-X/Veiasai>"
#include <stdio.h>
#include <stdlib.h>

#include <glog/logging.h>

#include <iostream>

#include <cli/parser.hpp>
#include <runtime/core.hpp>

int main(int argc, char *argv[]) {
  std::string json = getenv("GRID_CONFIG");
  Grid::Config config = Grid::Config::LoadFromJson(json);
  Grid::Core core{config};
  Grid::Cli::Parser parser;
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);
  try {
    auto args = parser.parse(argc, argv);
    auto ret = core.Exec(args);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
