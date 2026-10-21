#include <gtest/gtest.h>

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path PATH { get_assets_path() / "commands" /
                                              "check" };
    return PATH;
}

}  // namespace

TEST(CommandsTest, CheckNotEnabledCpp) {
    std::vector<std::string_view> args { "check", "-v" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

#if defined(__APPLE__)
TEST(CommandsTest, CheckClangTidyCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.clang_tidy_args = "-checks=-*,performance-*";
    cfg.clang_tidy_enabled = true;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
#endif

TEST(CommandsTest, CheckCppCheckCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.cppcheck_enabled = true;
    cfg.cppcheck_checks = "performance";
    cfg.cppcheck_args = std::nullopt;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

#if defined(__APPLE__)
TEST(CommandsTest, CheckAllEnabledCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.clang_tidy_enabled = true;
    cfg.clang_tidy_args = "-checks=-*,performance-*";
    cfg.cppcheck_enabled = true;
    cfg.cppcheck_checks = "performance";
    cfg.cppcheck_args = std::nullopt;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
#endif

TEST(CommandsTest, CheckAllEnabledC) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.clang_tidy_enabled = true;
    cfg.clang_tidy_args = "-checks=-*,performance-*";
    cfg.cppcheck_enabled = true;
    cfg.cppcheck_checks = "performance";
    cfg.cppcheck_args = std::nullopt;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test",
                                         "--lang=c" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, CheckAllEnabledLibCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.clang_tidy_enabled = true;
    cfg.clang_tidy_args = "-checks=-*,cppcoreguidelines-*";
    cfg.cppcheck_enabled = true;
    cfg.cppcheck_checks = "performance";
    cfg.cppcheck_args = std::nullopt;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test",
                                         "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, CheckAllEnabledLibC) {
    cfg::Config cfg;
    ASSERT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );
    cfg.name = "check";
    cfg.clang_tidy_enabled = true;
    cfg.clang_tidy_args = "-checks=-*,performance-*";
    cfg.cppcheck_enabled = true;
    cfg.cppcheck_checks = "performance";
    cfg.cppcheck_args = std::nullopt;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    std::vector<std::string_view> args { "check", "-v", "--profile=test",
                                         "--lang=c", "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::check(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
