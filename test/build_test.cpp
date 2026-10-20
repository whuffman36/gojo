#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_wd_path() {
    static const std::filesystem::path path { get_assets_path() / "commands" /
                                              "buildtest" };
    return path;
}

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path path { test_wd_path() / "buildtest" };
    return path;
}

}  // namespace

TEST(CommandsTest, BuildCpp) {
    std::vector<std::string_view> args { "buildtest" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));

    args.clear();
    args.emplace_back("buildtest");
    args.emplace_back("--tests");
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test" / "buildtest_test"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}

TEST(CommandsTest, BuildLibCpp) {
    std::vector<std::string_view> args { "buildtest", "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));

    args.clear();
    args.emplace_back("buildtest");
    args.emplace_back("--lib");
    args.emplace_back("--tests");
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test" / "buildtest_test"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}

TEST(CommandsTest, BuildC) {
    std::vector<std::string_view> args { "buildtest", "--lang=c" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));

    args.clear();
    args.emplace_back("buildtest");
    args.emplace_back("--lang=c");
    args.emplace_back("--tests");
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test" / "buildtest_test"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}

TEST(CommandsTest, BuildLibC) {
    std::vector<std::string_view> args { "buildtest", "--lib", "--lang=c" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));

    args.clear();
    args.emplace_back("buildtest");
    args.emplace_back("--lib");
    args.emplace_back("--lang=c");
    args.emplace_back("--tests");
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "test" / "buildtest_test"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}

TEST(CommandsTest, BuildRelease) {
    std::vector<std::string_view> args { "buildtest" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));

    args.clear();
    args.emplace_back("--release");
    args.emplace_back("--no-tests");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Release" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Release" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}

TEST(CommandsTest, BuildNoConfig) {
    std::vector<std::string_view> args { "buildtest" };
    ASSERT_TRUE(init_test_gojo_project(test_wd_path(), args));
    ASSERT_NO_THROW(cmd::clean({}));

    args.clear();
    args.emplace_back("--debug");
    args.emplace_back("--verbose");
    EXPECT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "buildtest"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_wd_path()));
}
