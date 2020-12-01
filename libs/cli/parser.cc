// Copyright [2020] <DeeEll-X/Veiasai>"
#include "parser.hpp"

#include <glog/logging.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using std::cout;

namespace Grid {
namespace Cli {

Parser::Parser()
    : mGenericOptions("Generic Options"),
      mConfigFileOptions("Config File Options") {
  mGenericOptions.add(mConfigFileOptions);
  mGenericOptions.add_options()("help", "produce help message");
  mGenericOptions.add_options()("action", po::value<std::vector<std::string>>(),
                                "container operations");
}

std::unique_ptr<Args> Parser::parse(int argc, char *argv[]) {
  po::positional_options_description p;
  p.add("action", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(mGenericOptions)
                .positional(p)
                .run(),
            vm);
  po::notify(vm);

  std::unique_ptr<Args> ret;
  if (vm.count("help")) {
    cout << mGenericOptions << "\n";
    exit(0);
  } else if (vm.count("action")) {
    // ./Grid create id path
    auto vec = vm["action"].as<std::vector<std::string>>();
    if (vec[0] == "create") {
      if (vec.size() == 3) {
        ret.reset(new CreateArgs(vec[1], vec[2]));
        LOG(INFO) << " parser: create "
                  << " container id: " << vec[1] << " bundle path: " << vec[2];
      } else {
        throw std::runtime_error("parse : create must have 2 args!");
      }
    } else if (vec[0] == "start") {
      if (vec.size() == 2) {
        ret.reset(new StartArgs(vec[1]));
        LOG(INFO) << " parse: start "
                  << " container id: " << vec[1];
      } else {
        throw std::runtime_error("parse : start must have 1 arg!");
      }
    } else if (vec[0] == "kill") {
      if (vec.size() == 3) {
        int signal = stoi(vec[2]);
        if (!signal) {
          throw std::runtime_error("kill failed: signal is invalid!");
        }
        ret.reset(new KillArgs(vec[1], signal));
        LOG(INFO) << " parser: kill "
                  << " container id: " << vec[1] << " signal: " << vec[2];
      } else {
        throw std::runtime_error("parse : kill must have 2 args!");
      }
    } else if (vec[0] == "state") {
      if(vec.size() == 2) {
        ret.reset(new StateArgs(vec[1]));
        LOG(INFO) << " parse: state "
                  << " container id: " << vec[1];
      } else {
        throw std::runtime_error("state failed: state must have 1 arg!");
      }
    } else if (vec[0] == "delete") {
      if(vec.size() == 2) {
        ret.reset(new DeleteArgs(vec[1]));
        LOG(INFO) << " parse: delete "
                  << " container id: " << vec[1];
      } else {
        throw std::runtime_error("state failed: state must have 1 arg!");
      }
    } else {
      throw std::runtime_error("parse : unknown action!");
    }
  } else {
    cout << mGenericOptions << "\n";
    exit(0);
  }

  return ret;
}

}  // namespace Cli
}  // namespace Grid
