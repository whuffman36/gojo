#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "commands" / "clean"
};

}


TEST(CommandsTest, CleanTest) {
  std::vector<std::string_view> args { "cleantest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  ASSERT_NO_THROW(cmd::build(args));

  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "CMakeCache.txt"));

  args.clear();
  EXPECT_NO_THROW(cmd::clean(args));
  EXPECT_FALSE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, CleanTestLib) {
  std::vector<std::string_view> args { "cleantest", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  ASSERT_NO_THROW(cmd::build(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "CMakeCache.txt"));

  args.clear();
  EXPECT_NO_THROW(cmd::clean(args));
  // Idk how to test this was successfull...

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, CleanTestDeep) {
  std::vector<std::string_view> args { "cleantest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  ASSERT_NO_THROW(cmd::build(args));

  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "CMakeCache.txt"));

  args.clear();
  args.push_back("--deep");
  EXPECT_NO_THROW(cmd::clean(args));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}


TEST(CommandsTest, CleanTestAll) {
  std::vector<std::string_view> args { "cleantest" };
  ASSERT_TRUE(init_test_gojo_project(PATH, args));

  args.clear();
  ASSERT_NO_THROW(cmd::build(args));

  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));
  EXPECT_TRUE(std::filesystem::exists(PATH / "build" / "Debug" / "CMakeCache.txt"));

  args.clear();
  args.push_back("--all");
  EXPECT_NO_THROW(cmd::clean(args));
  EXPECT_FALSE(std::filesystem::exists(PATH / "build" / "Debug" / "src" / "cleantest"));

  EXPECT_TRUE(cleanup_test_gojo_project(PATH));
}
