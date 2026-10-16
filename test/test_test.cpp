#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path path { get_assets_path() / "commands" /
                                              "testdir" };
    return path;
}

}  // namespace

TEST(CommandsTest, TestCpp) {
    std::vector<std::string_view> args { "testexample", "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("--tests");
    ASSERT_NO_THROW(cmd::build(args));

    args.clear();
    EXPECT_NO_THROW(cmd::test(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, TestLibCpp) {
    std::vector<std::string_view> args { "testexample", "--lib", "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("--tests");
    ASSERT_NO_THROW(cmd::build(args));

    args.clear();
    EXPECT_NO_THROW(cmd::test(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, TestC) {
    std::vector<std::string_view> args { "testexample", "--lang=c", "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("--tests");
    ASSERT_NO_THROW(cmd::build(args));

    args.clear();
    EXPECT_NO_THROW(cmd::test(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, TestLibC) {
    std::vector<std::string_view> args { "testexample", "--lang=c", "--lib",
                                         "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("--tests");
    ASSERT_NO_THROW(cmd::build(args));

    args.clear();
    EXPECT_NO_THROW(cmd::test(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, TestBuildFlag) {
    std::vector<std::string_view> args { "testexample", "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("--build");
    EXPECT_NO_THROW(cmd::test(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
