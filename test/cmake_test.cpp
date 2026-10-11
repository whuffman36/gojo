#include <filesystem>

#include <gtest/gtest.h>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path CMAKE_PATH {
  get_assets_path() / "cmake"
};

}


TEST(CMakeTest, Configure) {
  const auto test_path { std::filesystem::current_path() };

  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));

  std::filesystem::current_path(CMAKE_PATH / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));

  // Current path is now set to cmake/bin
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path() / "build"));

  std::filesystem::current_path(test_path);
  std::filesystem::current_path(CMAKE_PATH / "lib");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path() / "build"));
}


TEST(CMakeTest, Build) {
  const auto test_path { std::filesystem::current_path() };
  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));

  std::filesystem::current_path(CMAKE_PATH / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false, false));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path() / "build"));

  std::filesystem::current_path(test_path);
  std::filesystem::current_path(CMAKE_PATH / "lib");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_TRUE(std::filesystem::exists(std::filesystem::current_path() / "build"));
  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path() / "build"));
}


TEST(CMakeTest, Clean) {
  const auto test_path { std::filesystem::current_path() };
  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));

  std::filesystem::current_path(CMAKE_PATH / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false, false));
  EXPECT_TRUE(std::filesystem::exists(cfg.get_build_dir() / "main"));
  EXPECT_NO_THROW(cmake::clean(cfg, false));
  EXPECT_FALSE(std::filesystem::exists(cfg.get_build_dir() / "main"));
  EXPECT_TRUE(std::filesystem::remove_all(std::filesystem::current_path() / "build"));
}