#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path PATH { get_assets_path() / "commands" /
                                              "profile" };
    return PATH;
}

}  // namespace


TEST(CommandsTest, ProfileCreate) {
    std::vector<std::string_view> args { "example", "-v" };
    ASSERT_TRUE(init_test_gojo_project(test_root_path(), args));

    args.clear();
    args.emplace_back("create");
    args.emplace_back("test");
    EXPECT_NO_THROW(cmd::profile(args));

    EXPECT_TRUE(std::filesystem::exists(utils::get_profiles_dir() / "test"));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, ProfileList) {
    ASSERT_TRUE(create_test_gojo_profile());

    std::vector<std::string_view> args { "list" };
    EXPECT_NO_THROW(cmd::profile(args));

    EXPECT_TRUE(remove_test_gojo_profile());
}

TEST(CommandsTest, ProfileShow) {
    ASSERT_TRUE(create_test_gojo_profile());

    std::vector<std::string_view> args { "show", "test" };
    EXPECT_NO_THROW(cmd::profile(args));

    EXPECT_TRUE(remove_test_gojo_profile());
}

TEST(CommandsTest, ProfileDelete) {
    ASSERT_TRUE(create_test_gojo_profile());

    std::vector<std::string_view> args { "delete", "test" };
    EXPECT_NO_THROW(cmd::profile(args));

    EXPECT_FALSE(std::filesystem::exists(utils::get_profiles_dir() / "test"));
}
