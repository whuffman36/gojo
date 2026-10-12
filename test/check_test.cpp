#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "src/cfg/config.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path& test_root_path() {
  static const std::filesystem::path PATH {
    get_assets_path() / "commands" / "check"
  };
  return PATH;
}

}


TEST(CommandsTest, CheckTestNotEnabled) {
  std::vector<std::string_view> args { "check" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

#if defined(__APPLE__)
TEST(CommandsTest, CheckTestClangTidy) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.clang_tidy_args = "-checks=-*,performance-*";
  cfg.clang_tidy_enabled = true;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
#endif


TEST(CommandsTest, CheckTestCppCheck) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.cppcheck_enabled = true;
  cfg.cppcheck_checks = "performance";
  cfg.cppcheck_args = std::nullopt;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


#if defined(__APPLE__)
TEST(CommandsTest, CheckTestAllEnabled) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.clang_tidy_enabled = true;
  cfg.clang_tidy_args = "-checks=-*,performance-*";
  cfg.cppcheck_enabled = true;
  cfg.cppcheck_checks = "performance";
  cfg.cppcheck_args = std::nullopt;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
#endif


TEST(CommandsTest, CheckTestAllEnabledC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.clang_tidy_enabled = true;
  cfg.clang_tidy_args = "-checks=-*,performance-*";
  cfg.cppcheck_enabled = true;
  cfg.cppcheck_checks = "performance";
  cfg.cppcheck_args = std::nullopt;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test", "--lang=c" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, CheckTestAllEnabledLib) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.clang_tidy_enabled = true;
  cfg.clang_tidy_args = "-checks=-*,cppcoreguidelines-*";
  cfg.cppcheck_enabled = true;
  cfg.cppcheck_checks = "performance";
  cfg.cppcheck_args = std::nullopt;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}


TEST(CommandsTest, CheckTestAllEnabledLibC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.name = "check";
  cfg.clang_tidy_enabled = true;
  cfg.clang_tidy_args = "-checks=-*,performance-*";
  cfg.cppcheck_enabled = true;
  cfg.cppcheck_checks = "performance";
  cfg.cppcheck_args = std::nullopt;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  std::vector<std::string_view> args { "check", "--profile=test", "--lang=c", "--lib" };
  ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

  args.clear();
  EXPECT_NO_THROW(cmd::check(args));

  EXPECT_TRUE(remove_test_gojo_profile());
  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
