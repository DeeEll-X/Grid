// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once

#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <memory>

#include "../runtime/args.hpp"

namespace Grid {
namespace Cli {

namespace po = boost::program_options;

class Parser {
 public:
  Parser();
  std::unique_ptr<Args> parse(int argc, char *argv[]);

 private:
  po::options_description mGenericOptions;
  po::options_description mConfigFileOptions;
};

}  // namespace Cli
}  // namespace Grid
