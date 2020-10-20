// Copyright [2020] <DeeEll-X/Veiasai>"
#include "parser.hpp"

#include <glog/logging.h>

#include <iostream>
#include <string>
#include <vector>

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
  mGenericOptions.add_options()("action", po::value<std::vector<std::string>>(),
                                "create container");
}

Args Parser::parse(int argc, char *argv[]) {
  po::positional_options_description p;
  p.add("action", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(mGenericOptions)
                .positional(p)
                .run(),
            vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << mGenericOptions << "\n";
    exit(0);
  } else if (vm.count("action")) {
    // ./Grid create id path
    auto vec = vm["action"].as<std::vector<std::string>>();
    if (vec[0] == "create") {
      if (vec.size() == 3) {
        CreateArgs args{vec[1], vec[2]};
        LOG(INFO) << " parser: create "
                  << " container id: " << args.mContainerId
                  << " bundle path: " << args.mBundlePath;
        return args;
      } else {
        throw std::runtime_error("parse : create must have 2 args!");
      }
    }
  }

  return CreateArgs();
}

}  // namespace Cli
}  // namespace Grid
