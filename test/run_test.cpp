#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "commands" / "run"
};

}


TEST(CommandsTest, RunTestCpp) {
  std::vector<std::string_view> args { "runtest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  ASSERT_NO_THROW(cmd::run(args));

  args.clear();
  args.push_back("--release");
  EXPECT_NO_THROW(cmd::run(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, RunTestC) {
  std::vector<std::string_view> args { "runtest", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  ASSERT_NO_THROW(cmd::run(args));

  args.clear();
  args.push_back("--release");
  EXPECT_NO_THROW(cmd::run(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, RunTestLib) {
  std::vector<std::string_view> args { "runtest", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::run(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug"));
  EXPECT_FALSE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "runtest"));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}