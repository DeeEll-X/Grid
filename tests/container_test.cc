// Copyright [2020] <DeeEll-X/Veiasai>"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "mock.hpp"
#include "runtime/container.hpp"

namespace Grid {
namespace Test {

class ContainerFixture : public ::testing::Test {
 public:
  ContainerFixture() : mMockSystem(), mContainer{mMockSystem} {}
  MockSystem mMockSystem;
  Container mContainer;
  fs::path bundle{"./bundle"};
  fs::path syncPath{"./rootdir/containers"};
  void SetUp() {
    // code here will execute just before the test ensues
    fs::create_directories(syncPath);
  }

  void TearDown() {
    // code here will be called just after the test completes
    // ok to through exceptions from here if need be
  }
};

TEST_F(ContainerFixture, Create) {
  std::string containerId = "containerId";

  fs::path wPath{syncPath}, mntPath{syncPath};
  wPath /= containerId;
  wPath /= "writeLayer";
  mntPath /= containerId;
  mntPath /= "mntFolder";
  EXPECT_CALL(mMockSystem,
              MountAUFS(wPath.generic_string(), bundle.generic_string(),
                        mntPath.generic_string()));

  mContainer.Create(containerId, bundle, syncPath);

  fs::path statusPath{syncPath};
  statusPath /= containerId;
  statusPath /= "status.json";
  std::ifstream statusFile(statusPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!statusFile.is_open()) {
    throw std::runtime_error("statusFile cannot open!");
  }
  if (reader.parse(statusFile, root)) {
    ASSERT_EQ(root["ID"].asString(), containerId);
    ASSERT_EQ(root["Bundle"].asString(), bundle.generic_string());
    ASSERT_TRUE(root["Created"].asBool());
    ASSERT_FALSE(root["Creating"].asBool());
    ASSERT_FALSE(root["Running"].asBool());
    ASSERT_FALSE(root["Stopped"].asBool());
    ASSERT_EQ(root["Pid"].asInt64(), Json::Value::Int64(0));
  }
}

}  // namespace Test
}  // namespace Grid
