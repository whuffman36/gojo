#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "commands" / "testdir"
};

}


TEST(CommandsTest, TestTestCpp) {
  std::vector<std::string_view> args { "testexample", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--tests");
  ASSERT_NO_THROW(cmd::build(args));

  args.clear();
  EXPECT_NO_THROW(cmd::test(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, TestTestLibCpp) {
  std::vector<std::string_view> args { "testexample", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--tests");
  ASSERT_NO_THROW(cmd::build(args));

  args.clear();
  EXPECT_NO_THROW(cmd::test(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, TestTestC) {
  std::vector<std::string_view> args { "testexample", "--lang=c", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--tests");
  ASSERT_NO_THROW(cmd::build(args));

  args.clear();
  EXPECT_NO_THROW(cmd::test(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, TestTestLibC) {
  std::vector<std::string_view> args { "testexample", "--lang=c", "--lib", "--tests" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--tests");
  ASSERT_NO_THROW(cmd::build(args));

  args.clear();
  EXPECT_NO_THROW(cmd::test(args));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}
