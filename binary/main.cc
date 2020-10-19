// Copyright [2020] <DeeEll-X/Veiasai>"
#include <glog/logging.h>

#include <iostream>

#include <cli/parser.hpp>
#include <runtime/core.hpp>

int main(int argc, char* argv[]) {
  Grid::Core core;
  Grid::Cli::Parser parser;
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);

  try {
    auto args = parser.parse(argc, argv);
    auto ret = core.exec(args);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
