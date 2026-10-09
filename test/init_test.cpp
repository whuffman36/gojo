#include <filesystem>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "src/cmd/commands.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "test/utils.hpp"


namespace {

// This code is run from <PROJECT_ROOT>/build/Debug/test
// The assets path is <PROJECT_ROOT>/test/assets
const std::filesystem::path PATH {
  get_assets_path() / "commands"
};

}


TEST(CommandsTest, InitTestCpp) {
  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
  std::vector<std::string_view> args { "example" };
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.cpp"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitTestCppLib) {
  std::vector<std::string_view> args { "example" };
  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
  args.push_back("--lib");
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


TEST(CommandsTest, InitTestC) {
  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
  std::vector<std::string_view> args { "example" };
  args.push_back("--lang=c");
  EXPECT_NO_THROW(cmd::init(args));

  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "gojo.toml"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "CMakeLists.txt"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "src" / "example.c"));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));

  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path()));
}


TEST(CommandsTest, InitTestCLib) {
  std::vector<std::string_view> args { "example" };
  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
  args.push_back("--lib");
  args.push_back("--lang=c");
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


TEST(CommandsTest, InitTestProfileCpp) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  ASSERT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
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

  EXPECT_TRUE(cleanup_test_gojo_project(PATH / "init"));
}


TEST(CommandsTest, InitTestLibProfileCpp) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
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

  EXPECT_TRUE(cleanup_test_gojo_project(PATH / "init"));
}


TEST(CommandsTest, InitTestProfileC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  cfg.language = cfg::Language::C;
  cfg.test_framework = cfg::TestFramework::Unity;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
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

  EXPECT_TRUE(cleanup_test_gojo_project(PATH / "init"));
}


TEST(CommandsTest, InitTestLibProfileC) {
  cfg::Config cfg;
  ASSERT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_full.toml")));
  cfg.target = cfg::Target::Library;
  cfg.test_framework = cfg::TestFramework::Unity;
  ASSERT_TRUE(create_test_gojo_profile(cfg));

  EXPECT_TRUE(std::filesystem::create_directory(PATH / "init"));
  std::filesystem::current_path(PATH / "init");
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

  EXPECT_TRUE(cleanup_test_gojo_project(PATH / "init"));
}
