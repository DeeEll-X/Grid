// Copyright [2020] <DeeEll-X/Veiasai>"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock.hpp"

namespace Grid {
namespace Test {

class ContainerFixture : public ::testing::Test {
 public:
  ContainerFixture() {}

  void SetUp() {
    // code here will execute just before the test ensues
  }

  void TearDown() {
    // code here will be called just after the test completes
    // ok to through exceptions from here if need be
  }
};

TEST_F(ContainerFixture, Empty) {}

}  // namespace Test
}  // namespace Grid
