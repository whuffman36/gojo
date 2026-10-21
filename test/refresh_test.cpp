#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path PATH { get_assets_path() / "commands" /
                                              "refresh" };
    return PATH;
}

}  // namespace


TEST(CommandsTest, RefreshCpp) {
    std::vector<std::string_view> args { "example", "-v" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::refresh(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, RefreshC) {
    std::vector<std::string_view> args { "example", "-v", "--lang=c" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::refresh(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, RefreshLibCpp) {
    std::vector<std::string_view> args { "example", "-v", "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::refresh(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, RefreshLibC) {
    std::vector<std::string_view> args { "example", "-v", "--lang=c", "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::refresh(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, RefreshAfterBuild) {
    std::vector<std::string_view> args { "example", "-v" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("-v");
    ASSERT_NO_THROW(cmd::build(args));

    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "example" / "build" / "Debug" / "src" / "example"));

    EXPECT_NO_THROW(cmd::refresh(args));
    EXPECT_FALSE(std::filesystem::exists(test_root_path() / "example" / "build" / "Debug" / "src" / "example"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
