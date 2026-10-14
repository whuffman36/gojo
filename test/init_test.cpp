#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path& test_root_path() {
  static const std::filesystem::path path {
    get_assets_path() / "commands"
  };
  return path;
}

}


TEST(CommandsTest, InitCpp) {
  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.cpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitLibCpp) {
  std::vector<std::string_view> args { "example" };
  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  args.emplace_back("--lib");
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.cpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example" / "example.hpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitC) {
  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example" };
  args.emplace_back("--lang=c");
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.c"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitLibC) {
  std::vector<std::string_view> args { "example" };
  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  args.emplace_back("--lib");
  args.emplace_back("--lang=c");
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.c"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example" / "example.h"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitProfileCpp) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  ASSERT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example", "--profile=test" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.cpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clang-tidy"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clangd"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".gitignore"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "conanfile.py"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "test"));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path() / "init"));
}


TEST(CommandsTest, InitProfileLibCpp) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example", "--profile=test", "--lib" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.cpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example" / "example.hpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clang-tidy"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clangd"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".gitignore"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "conanfile.py"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "test"));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path() / "init"));
}


TEST(CommandsTest, InitProfileC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  cfg.language = cfg::Language::C;
  cfg.test_framework = cfg::TestFramework::Unity;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example", "--profile=test" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.c"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clang-tidy"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clangd"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".gitignore"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "conanfile.py"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "test"));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path() / "init"));
}


TEST(CommandsTest, InitProfileLibC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  cfg.target = cfg::Target::Library;
  cfg.test_framework = cfg::TestFramework::Unity;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directories(test_root_path() / "init"));
  std::filesystem::current_path(test_root_path() / "init");
  std::vector<std::string_view> args { "example", "--profile=test", "--lang=c" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.c"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "include" / "example" / "example.h"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clang-tidy"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".clangd"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / ".gitignore"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "conanfile.py"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "test"));

  EXPECT_TRUE(cleanup_test_gojo_project(test_root_path() / "init"));
}
