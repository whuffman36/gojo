#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path path { get_assets_path() / "commands" /
                                              "clean" };
    return path;
}

}  // namespace

TEST(CommandsTest, CleanCpp) {
    std::vector<std::string_view> args { "cleantest" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    ASSERT_NO_THROW(cmd::build(args));

    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "src" / "cleantest"));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "CMakeCache.txt"));

    args.clear();
    EXPECT_NO_THROW(cmd::clean(args));
    EXPECT_FALSE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                         "src" / "cleantest"));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, CleanLibCpp) {
    std::vector<std::string_view> args { "cleantest", "--lib" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    ASSERT_NO_THROW(cmd::build(args));
    EXPECT_TRUE(std::filesystem::exists(test_root_path() / "build" / "Debug" /
                                        "CMakeCache.txt"));

    args.clear();
    EXPECT_NO_THROW(cmd::clean(args));
    // Idk how to test this was successfull...

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
