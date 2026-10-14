#include <filesystem>

#include <gtest/gtest.h>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path& test_root_path() {
  static const std::filesystem::path path {
    get_assets_path() / "cmake"
  };
  return path;
}

}


TEST(CMakeTest, Configure) {
  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.testing_enabled = true;

  std::filesystem::current_path(test_root_path() / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));

  // Current path is now set to cmake/bin
  EXPECT_TRUE(std::filesystem::exists(cfg.get_build_dir()));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));

  std::filesystem::current_path(test_root_path() / "lib");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_TRUE(std::filesystem::exists(cfg.get_build_dir()));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));
}


TEST(CMakeTest, Build) {
  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.testing_enabled = true;

  std::filesystem::current_path(test_root_path() / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false));
  EXPECT_TRUE(std::filesystem::exists(cfg.get_build_dir()));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));

  std::filesystem::current_path(test_root_path() / "lib");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false));
  EXPECT_TRUE(std::filesystem::exists(cfg.get_build_dir()));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));
}


TEST(CMakeTest, Test) {
  cfg::Config cfg {};
  EXPECT_NO_THROW(cfg = cfg::Config::from_file(get_config("config_minimum.toml")));
  cfg.testing_enabled = true;

  std::filesystem::current_path(test_root_path() / "bin");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false));
  EXPECT_NO_THROW(cmake::test(cfg, ""));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));

  std::filesystem::current_path(test_root_path() / "lib");
  EXPECT_NO_THROW(cmake::configure(cfg, false));
  EXPECT_NO_THROW(cmake::build(cfg, "", false));
  EXPECT_NO_THROW(cmake::test(cfg, ""));
  EXPECT_TRUE(std::filesystem::remove_all(cfg.get_build_dir()));
}
