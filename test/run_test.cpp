#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path& test_root_path() {
  static const std::filesystem::path path {
    get_assets_path() / "commands" / "run"
  };
  return path;
}

}


TEST(CommandsTest, RunTestCpp) {
  std::vector<std::string_view> args { "runtest" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  args.emplace_back("--debug");
  ASSERT_NO_THROW(cmd::run(args));

  args.clear();
  args.emplace_back("--release");
  EXPECT_NO_THROW(cmd::run(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, RunTestC) {
  std::vector<std::string_view> args { "runtest", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  args.emplace_back("--debug");
  ASSERT_NO_THROW(cmd::run(args));

  args.clear();
  args.emplace_back("--release");
  EXPECT_NO_THROW(cmd::run(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, RunTestLib) {
  std::vector<std::string_view> args { "runtest", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::run(args));
  EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug"));
  EXPECT_FALSE(std::filesystem::exists(test_root_path() / "build" / "Debug" / "src" / "runtest"));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}