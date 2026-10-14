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


TEST(CommandsTest, FmtCpp) {
  std::vector<std::string_view> args { "format" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtLibCpp) {
  std::vector<std::string_view> args { "format", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtC) {
  std::vector<std::string_view> args { "format", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtLibC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestsCpp) {
  std::vector<std::string_view> args { "format", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestsLibCpp) {
  std::vector<std::string_view> args { "format", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestsC) {
  std::vector<std::string_view> args { "format", "--lang=c",  "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, FmtTestsLibC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
