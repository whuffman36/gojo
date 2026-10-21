#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path PATH { get_assets_path() / "commands" /
                                              "dep" };
    return PATH;
}

}  // namespace


TEST(CommandsTest, DepSearchConan) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    cfg.package_manager = cfg::PackageManager::Conan;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    ASSERT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "example", "--profile=test" };
    ASSERT_NO_THROW(cmd::init(args));

    args.clear();
    args.emplace_back("search");
    args.emplace_back("sqlite3");
    EXPECT_NO_THROW(cmd::dep(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, DepAddInstallRemoveUninstallConan) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    cfg.package_manager = cfg::PackageManager::Conan;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    ASSERT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "example", "--profile=test" };
    ASSERT_NO_THROW(cmd::init(args));

    args.clear();
    args.emplace_back("add");
    args.emplace_back("sqlite3");
    EXPECT_NO_THROW(cmd::dep(args));

    args.clear();
    args.emplace_back("install");
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::dep(args));

    args.clear();
    args.emplace_back("-v");
    EXPECT_NO_THROW(cmd::build(args));

    args.clear();
    args.emplace_back("uninstall");
    args.emplace_back("sqlite3");
    EXPECT_NO_THROW(cmd::dep(args));

    args.clear();
    args.emplace_back("remove");
    args.emplace_back("sqlite3");
    EXPECT_NO_THROW(cmd::dep(args));

    EXPECT_TRUE(remove_test_gojo_profile());
    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
