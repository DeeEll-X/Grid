// Copyright [2020] <DeeEll-X/Veiasai>"
#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <runtime/system.hpp>
#include <string>

namespace Grid {
namespace Test {

class MockSystem : public System {
 public:
  MOCK_METHOD(void, Exec, (const std::string &command), (override));
  MOCK_METHOD(void, MountAUFS,
              (const std::string &writelayer, const std::string &rolayer,
               const std::string &mntfolder),
              (override));
};
}  // namespace Test
}  // namespace Grid
