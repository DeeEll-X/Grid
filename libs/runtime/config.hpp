// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <jsoncpp/json/json.h>

#include <fstream>
#include <string>

namespace Grid {
struct Config {
  std::string mRootDir;
  static Config LoadFromJson(const std::string &jsonPath) {
    Config c;
    std::ifstream configFile(jsonPath, std::ifstream::binary);
    if (!configFile.is_open()) {
      throw std::runtime_error("loadconfig open file fail: " + jsonPath);
    }
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(configFile, root)) {
      c.mRootDir = root["rootDir"].asString();
    } else {
      throw std::runtime_error("loadconfig parse fail: " + jsonPath);
    }

    return c;
  }
};
}  // namespace Grid
