#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string_view>
#include <vector>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/cmd/commands.hpp"
#include "test/utils.hpp"

namespace {

const std::filesystem::path& test_root_path() {
    static const std::filesystem::path path { get_assets_path() / "commands" /
                                              "new" };
    return path;
}

}  // namespace

TEST(CommandsTest, NewCpp) {
    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.cpp"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));

    EXPECT_TRUE(std::filesystem::remove_all(test_root_path()));
}

TEST(CommandsTest, NewLibCpp) {
    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--lib" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.cpp"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example" /
                                        "example.hpp"));

    EXPECT_TRUE(std::filesystem::remove_all(test_root_path()));
}

TEST(CommandsTest, NewC) {
    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--lang=c" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.c"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));

    EXPECT_TRUE(std::filesystem::remove_all(test_root_path()));
}

TEST(CommandsTest, NewLibC) {
    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "--lib",
                                         "-v", "--lang=c" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.c"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example" /
                                        "example.h"));

    EXPECT_TRUE(std::filesystem::remove_all(test_root_path()));
}

TEST(CommandsTest, NewProfileCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    ASSERT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--profile=test" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.cpp"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clang-tidy"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clangd"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".gitignore"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "conanfile.py"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "test"));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, NewProfileLibCpp) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--profile=test", "--lib" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.cpp"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clang-tidy"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clangd"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".gitignore"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "conanfile.py"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "test"));

    EXPECT_TRUE(std::filesystem::exists(root_path / "include"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example" /
                                        "example.hpp"));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, NewProfileC) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    cfg.language = cfg::Language::C;
    cfg.test_framework = cfg::TestFramework::Unity;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    ASSERT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--profile=test" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.c"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clang-tidy"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clangd"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".gitignore"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "conanfile.py"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "test"));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, NewProfileLibC) {
    cfg::Config cfg;
    ASSERT_NO_THROW(cfg =
                        cfg::Config::from_file(get_config("config_full.toml")));
    cfg.target = cfg::Target::Library;
    cfg.test_framework = cfg::TestFramework::Unity;
    ASSERT_TRUE(create_test_gojo_profile(cfg));

    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());
    std::vector<std::string_view> args { "path/to/project/dir/example", "-v",
                                         "--profile=test", "--lang=c" };
    EXPECT_NO_THROW(cmd::new_init(args));

    const std::filesystem::path root_path { test_root_path() / "path" / "to" /
                                            "project" / "dir" / "example" };

    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.c"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clang-tidy"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".clangd"));
    EXPECT_TRUE(std::filesystem::exists(root_path / ".gitignore"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "conanfile.py"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "test"));

    EXPECT_TRUE(std::filesystem::exists(root_path / "include"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "include" / "example" /
                                        "example.h"));

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}

TEST(CommandsTest, NewInteractive) {
    std::vector<std::string_view> args { "example" };
    EXPECT_TRUE(std::filesystem::create_directories(test_root_path()));
    std::filesystem::current_path(test_root_path());

    std::stringstream input { // project name:
                              "example\n"
                              // project language:
                              "1\n"  // Option 1 (C++)
                                     // project target:
                              "1\n"  // Option 1 (Binary)
                                     // verbose mode:
                              "1\n"  // Option 2 (false, disabled)
                                     // language standard:
                              "0\n"  // Skip (default - C++23)
                                     // compiler:
                              "1\n"  // Option 1 (default)
                                     // compiler flags:
                              "\n"   // Skip (no flags)
                                     // gnu extensions:
                              "1\n"  // Option 1 (true, enabled)
                                     // build system:
                              "0\n"  // Skip (CMake)
                                     // generator
                              "1\n"  // Option 1 (default)
                                     // build type
                              "2\n"  // Option 2 (Release)
                                     // format-style
                              "5\n"  // Option 5 (Mozilla)
                                     // language server
                              "\n"   // Skip
                                     // clang-tidy enabled
                              "1\n"  // Option 1 (true, enabled)
                                     // clang-tidy args
                              "\n"   // Skip
                                     // cppcheck enabled
                              "2\n"  // Option 2 (false, disabled)
                                     // cppcheck checks
                              "\n"   // Skip
                                     // cppcheck args
                              "\n"   // Skip
                                     // testing enabled
                              "1\n"  // Option 1 (true, enabled)
                                     // test framework
                              "2\n"  // Option 2 (Googletest)
                                     // package manager
                              "1\n"  // Option 1 (conan)
                                     // build types
                              "Debug,Release\n"
    };

    args.emplace_back("-v");
    args.emplace_back("--interactive");
    EXPECT_NO_THROW(cmd::new_init(args, input));

    const std::filesystem::path root_path { test_root_path() / "example" };
    EXPECT_TRUE(std::filesystem::exists(root_path / "gojo.toml"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "CMakeLists.txt"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "src" / "example.cpp"));
    EXPECT_TRUE(std::filesystem::exists(root_path / "build"));

    cfg::Config cfg {};
    EXPECT_NO_THROW(cfg = cfg::Config::from_file(root_path / "gojo.toml"));
    EXPECT_EQ(cfg.name, "example");
    EXPECT_EQ(cfg.language, cfg::Language::Cpp);
    EXPECT_EQ(cfg.target, cfg::Target::Binary);
    ASSERT_TRUE(cfg.verbose.has_value());
    EXPECT_TRUE(cfg.verbose.value());
    EXPECT_FALSE(cfg.language_standard.has_value());
    EXPECT_EQ(cfg.compiler, cfg::Compiler::Default);
    EXPECT_FALSE(cfg.compiler_flags.has_value());
    ASSERT_TRUE(cfg.gnu_extensions_enabled.has_value());
    EXPECT_TRUE(cfg.gnu_extensions_enabled.value());
    EXPECT_FALSE(cfg.build_system.has_value());
    EXPECT_EQ(cfg.generator, cfg::Generator::Default);
    EXPECT_EQ(cfg.build_type, cfg::BuildType::Release);
    EXPECT_EQ(cfg.clang_format_style, cfg::FormatStyle::Mozilla);
    EXPECT_FALSE(cfg.language_server.has_value());
    ASSERT_TRUE(cfg.clang_tidy_enabled.has_value());
    EXPECT_TRUE(cfg.clang_tidy_enabled.value());
    EXPECT_FALSE(cfg.clang_tidy_args.has_value());
    ASSERT_TRUE(cfg.cppcheck_enabled.has_value());
    EXPECT_FALSE(cfg.cppcheck_enabled.value());
    EXPECT_FALSE(cfg.cppcheck_checks.has_value());
    EXPECT_FALSE(cfg.cppcheck_args.has_value());
    ASSERT_TRUE(cfg.testing_enabled.has_value());
    EXPECT_TRUE(cfg.testing_enabled.value());
    ASSERT_TRUE(cfg.test_framework.has_value());
    EXPECT_EQ(cfg.test_framework.value(), cfg::TestFramework::GoogleTest);
    ASSERT_TRUE(cfg.package_manager.has_value());
    EXPECT_EQ(cfg.package_manager.value(), cfg::PackageManager::Conan);
    EXPECT_EQ(cfg.build_types.at(0), cfg::BuildType::Debug);
    EXPECT_EQ(cfg.build_types.at(1), cfg::BuildType::Release);

    EXPECT_TRUE(cleanup_test_gojo_project(test_root_path()));
}
