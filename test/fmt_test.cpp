#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path& test_root_path() {
  static const std::filesystem::path path {
    get_assets_path() / "commands" / "format"
  };
  return path;
}

}


TEST(CommandsTest, FmtTestCpp) {
  std::vector<std::string_view> args { "format" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestLibCpp) {
  std::vector<std::string_view> args { "format", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestC) {
  std::vector<std::string_view> args { "format", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestLibC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestTestsCpp) {
  std::vector<std::string_view> args { "format", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestLibTestsCpp) {
  std::vector<std::string_view> args { "format", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestTestsC) {
  std::vector<std::string_view> args { "format", "--lang=c",  "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestLibTestsC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
