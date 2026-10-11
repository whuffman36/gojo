#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "commands" / "buildtest"
};

}


TEST(CommandsTest, BuildTestCpp) {
  std::vector<std::string_view> args { "buildtest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--no-tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "buildtest"));
  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
  
  args.clear();
  args.push_back("buildtest");
  args.push_back("--tests");
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  args.push_back("--tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "buildtest"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "test"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "test" / "buildtest_test"));
  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, BuildTestC) {
  std::vector<std::string_view> args { "buildtest", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  args.push_back("--no-tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "buildtest"));
  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
  
  args.clear();
  args.push_back("buildtest");
  args.push_back("--lang=c");
  args.push_back("--tests");
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  args.push_back("--tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "buildtest"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "test"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "test" / "buildtest_test"));
  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, BuildTestRelease) {
  std::vector<std::string_view> args { "buildtest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  args.push_back("--debug");
  args.push_back("--no-tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "buildtest"));

  args.clear();
  args.push_back("--release");
  args.push_back("--no-tests");
  EXPECT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Release" / "src"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Release" / "src" / "buildtest"));
  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}
