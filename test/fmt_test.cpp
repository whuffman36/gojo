#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"
#include "test/utils.hpp"

namespace {

constexpr std::string_view test_format_file_contents {
    R"(---
Language: Cpp
BasedOnStyle: llvm
)"
};

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path path {
        std::filesystem::absolute(get_assets_path() / "commands" / "format")
    };
    return path;
}

}  // namespace

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
    std::vector<std::string_view> args { "format", "--lang=c", "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    EXPECT_NO_THROW(cmd::fmt(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, FmtTestsLibC) {
    std::vector<std::string_view> args { "format", "--lang=c", "--lib",
                                         "--tests" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    EXPECT_NO_THROW(cmd::fmt(args));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, FmtUse) {
    std::vector<std::string_view> args { "format" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    ASSERT_TRUE(std::filesystem::create_directories(utils::get_home_dir() /
                                                    ".gojo" / "formats"));
    std::ofstream src { utils::get_home_dir() / ".gojo" / "formats" / "test" };
    ASSERT_TRUE(src.is_open());

    src << test_format_file_contents << std::flush;

    args.clear();
    args.emplace_back("--use=test");
    EXPECT_NO_THROW(cmd::fmt(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "format" /
                                        ".clang-format"));

    EXPECT_TRUE(std::filesystem::remove_all(utils::get_home_dir() / ".gojo"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, FmtSaveAs) {
    std::vector<std::string_view> args { "format" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    std::ofstream src { test_root_path() / "format" / ".clang-format" };
    ASSERT_TRUE(src.is_open());

    src << test_format_file_contents << std::flush;

    args.clear();
    args.emplace_back("--save-as=test");
    EXPECT_NO_THROW(cmd::fmt(args));
    EXPECT_TRUE(std::filesystem::exists(utils::get_home_dir() / ".gojo" /
                                        "formats" / "test"));

    EXPECT_TRUE(std::filesystem::remove_all(utils::get_home_dir() / ".gojo"));
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
