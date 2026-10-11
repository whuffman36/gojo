#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "commands" / "format"
};

}


TEST(CommandsTest, FmtTestCpp) {
  std::vector<std::string_view> args { "format" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestLibCpp) {
  std::vector<std::string_view> args { "format", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestC) {
  std::vector<std::string_view> args { "format", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestLibC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestTestsCpp) {
  std::vector<std::string_view> args { "format", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestLibTestsCpp) {
  std::vector<std::string_view> args { "format", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestTestsC) {
  std::vector<std::string_view> args { "format", "--lang=c",  "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, FmtTestLibTestsC) {
  std::vector<std::string_view> args { "format", "--lang=c", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  EXPECT_NO_THROW(cmd::fmt(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}
