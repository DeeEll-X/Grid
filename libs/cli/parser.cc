// Copyright [2020] <DeeEll-X/Veiasai>"
#include "parser.hpp"

#include <iostream>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using std::cout;

namespace Grid {
namespace Cli {

Parser::Parser()
    : mGenericOptions("Generic Options"),
      mConfigFileOptions("Config File Options"),
      mCreateOptions("Create Options") {
  mCreateOptions.add_options()("bundle", "image bundle path");
  mGenericOptions.add(mConfigFileOptions).add(mCreateOptions);
  mGenericOptions.add_options()("help", "produce help message");
}

Args Parser::parse(int argc, char* argv[]) {
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, mGenericOptions), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << mGenericOptions << "\n";
    exit(0);
  }

  return CreateArgs();
}

}  // namespace Cli
}  // namespace Grid
