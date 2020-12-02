// Copyright [2020] <DeeEll-X/Veiasai>"
#include "runtime/container.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "mock.hpp"

namespace Grid {

class ContainerVisitor {
 public:
  static void SetStatus(Container& container, const Status status) {
    container.mStatus = status;
  }
  static void SetRootPath(Container& container, const fs::path& path) {
    container.mRootPath = path;
  }
  static void SetPid(Container& container, int pid) { container.mPid = pid; }
  static Container::Config& MutableConfig(Container& container) {
    return container.mConfig;
  }
};

namespace Test {
class ContainerFixture : public ::testing::Test {
 public:
  ContainerFixture() : mMockSystem(), mContainer{mMockSystem} {}
  MockSystem mMockSystem;
  Container mContainer;
  std::string mContainerId = "testContainer";
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
  fs::path wPath{syncPath}, mntPath{syncPath};
  wPath /= mContainerId;
  wPath /= "writeLayer";
  mntPath /= mContainerId;
  mntPath /= "mntFolder";
  EXPECT_CALL(mMockSystem,
              MountAUFS(wPath.generic_string(), bundle.generic_string(),
                        mntPath.generic_string()));

  mContainer.Create(mContainerId, bundle, syncPath / mContainerId);

  fs::path statusPath{syncPath};
  statusPath /= mContainerId;
  statusPath /= "status.json";
  std::ifstream statusFile(statusPath, std::ifstream::binary);
  Json::Value root;
  Json::Reader reader;
  if (!statusFile.is_open()) {
    throw std::runtime_error("statusFile cannot open!");
  }
  if (reader.parse(statusFile, root)) {
    ASSERT_EQ(root["ID"].asString(), mContainerId);
    ASSERT_EQ(root["Bundle"].asString(), bundle.generic_string());
    ASSERT_EQ(root["Status"].asString(), "created");
    ASSERT_EQ(root["Pid"].asInt64(), Json::Value::Int64(0));
  }
  fs::remove_all(syncPath / mContainerId);
}
TEST_F(ContainerFixture, Start) {
  if (getenv("ENABLE_GRID_ROOT") == nullptr) {
    return;
  }
  ASSERT_EQ(system("/bin/bash create_container.sh"), 0);
  ContainerVisitor::SetStatus(mContainer, CREATED);
  ContainerVisitor::SetRootPath(mContainer, syncPath / mContainerId);
  auto& config = ContainerVisitor::MutableConfig(mContainer);
  auto& process = config.mProcess;
  process.terminal = false;
  process.cwd = "/";
  process.args = {"/bin/echo", "1"};
  mContainer.Start();
  sleep(1);
}
TEST_F(ContainerFixture, Kill_NotRunning) {
  ContainerVisitor::SetStatus(mContainer, STOPPED);
  EXPECT_THROW(mContainer.Kill(2), std::runtime_error);
}

TEST_F(ContainerFixture, Kill) {
  ContainerVisitor::SetStatus(mContainer, RUNNING);
  ContainerVisitor::SetPid(mContainer, getpid());

  static bool received;
  received = false;
  struct sigaction act;
  act.sa_handler = [](int) { received = true; };
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if (sigaction(SIGINT, &act, NULL) < 0) ASSERT_TRUE(false);

  mContainer.Kill(2);
  sleep(1);
  ASSERT_TRUE(received);
}
}  // namespace Test
}  // namespace Grid
