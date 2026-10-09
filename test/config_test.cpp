#include <filesystem>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"


namespace {

// This code is run from <PROJECT_ROOT>/build/Debug/test
// The assets path is <PROJECT_ROOT>/test/assets/config
const std::filesystem::path ASSETS_PATH { "../../../test/assets/config" };

const std::filesystem::path CONFIG_MINIMUM { ASSETS_PATH / "config_minimum.toml" };

const std::filesystem::path CONFIG_FULL { ASSETS_PATH / "config_full.toml" };


cfg::Compiler system_default_compiler() {
  #if defined(__APPLE__)
    return cfg::Compiler::AppleClangPp;
  #endif
  #if defined(__linux__)
    return cfg::Compiler::Gpp;
  #endif
  std::unreachable();
}

cfg::Generator system_default_generator() {
  #if defined(__APPLE__)
    return cfg::Generator::UnixMakefiles;
  #endif
  #if defined(__linux__)
    return cfg::Generator::UnixMakefiles;
  #endif
  std::unreachable();
}

}  // namespace


TEST(ConfigTest, FromFile) {
  cfg::Config cfg_minimum {};
  EXPECT_NO_THROW(cfg_minimum = cfg::Config::from_file(CONFIG_MINIMUM));

  EXPECT_EQ(cfg_minimum.name, "example");
  EXPECT_EQ(cfg_minimum.language, cfg::Language::Cpp);
  EXPECT_EQ(cfg_minimum.target, cfg::Target::Binary);
  EXPECT_FALSE(cfg_minimum.language_server.has_value());
  EXPECT_FALSE(cfg_minimum.language_standard.has_value());
  EXPECT_FALSE(cfg_minimum.build_system.has_value());
  EXPECT_FALSE(cfg_minimum.build_type.has_value());
  EXPECT_FALSE(cfg_minimum.clang_format_style.has_value());
  EXPECT_FALSE(cfg_minimum.clang_tidy_enabled.has_value());
  EXPECT_FALSE(cfg_minimum.clang_tidy_args.has_value());
  EXPECT_FALSE(cfg_minimum.cppcheck_enabled.has_value());
  EXPECT_FALSE(cfg_minimum.cppcheck_checks.has_value());
  EXPECT_FALSE(cfg_minimum.cppcheck_args.has_value());
  EXPECT_FALSE(cfg_minimum.compiler.has_value());
  EXPECT_FALSE(cfg_minimum.compiler_flags.has_value());
  EXPECT_FALSE(cfg_minimum.generator.has_value());
  EXPECT_FALSE(cfg_minimum.gnu_extensions_enabled.has_value());
  EXPECT_FALSE(cfg_minimum.package_manager.has_value());
  EXPECT_FALSE(cfg_minimum.test_framework.has_value());
  EXPECT_FALSE(cfg_minimum.testing_enabled.has_value());


  cfg::Config cfg_full {};
  EXPECT_NO_THROW(cfg_full = cfg::Config::from_file(CONFIG_FULL));

  EXPECT_EQ(cfg_full.name, "example");
  EXPECT_EQ(cfg_full.language, cfg::Language::Cpp);
  EXPECT_EQ(cfg_full.target, cfg::Target::Binary);
  EXPECT_TRUE(cfg_full.language_server.has_value());
  EXPECT_EQ(cfg_full.language_server.value(), "clangd");
  EXPECT_TRUE(cfg_full.language_standard.has_value());
  EXPECT_EQ(cfg_full.language_standard.value(), cfg::LanguageStandard::Cpp23);
  EXPECT_TRUE(cfg_full.build_system.has_value());
  EXPECT_EQ(cfg_full.build_system.value(), cfg::BuildSystem::CMake);
  EXPECT_TRUE(cfg_full.build_type.has_value());
  EXPECT_EQ(cfg_full.build_type.value(), cfg::BuildType::Debug);
  EXPECT_TRUE(cfg_full.clang_format_style.has_value());
  EXPECT_EQ(cfg_full.clang_format_style.value(), cfg::FormatStyle::Google);
  EXPECT_TRUE(cfg_full.clang_tidy_enabled.has_value());
  EXPECT_EQ(cfg_full.clang_tidy_enabled.value(), true);
  EXPECT_TRUE(cfg_full.clang_tidy_args.has_value());
  EXPECT_EQ(cfg_full.clang_tidy_args.value(), "example args");
  EXPECT_TRUE(cfg_full.cppcheck_enabled.has_value());
  EXPECT_EQ(cfg_full.cppcheck_enabled.value(), true);
  EXPECT_TRUE(cfg_full.cppcheck_checks.has_value());
  EXPECT_EQ(cfg_full.cppcheck_checks.value(), "example checks");
  EXPECT_TRUE(cfg_full.cppcheck_args.has_value());
  EXPECT_EQ(cfg_full.cppcheck_args.value(), "example cppcheck args");
  EXPECT_TRUE(cfg_full.compiler.has_value());
  EXPECT_EQ(cfg_full.compiler.value(), cfg::Compiler::Default);
  EXPECT_TRUE(cfg_full.compiler_flags.has_value());
  EXPECT_EQ(cfg_full.compiler_flags.value(), "-Wall");
  EXPECT_TRUE(cfg_full.generator.has_value());
  EXPECT_EQ(cfg_full.generator.value(), cfg::Generator::Ninja);
  EXPECT_TRUE(cfg_full.gnu_extensions_enabled.has_value());
  EXPECT_EQ(cfg_full.gnu_extensions_enabled.value(), true);
  EXPECT_TRUE(cfg_full.package_manager.has_value());
  EXPECT_EQ(cfg_full.package_manager.value(), cfg::PackageManager::Conan);
  EXPECT_TRUE(cfg_full.test_framework.has_value());
  EXPECT_EQ(cfg_full.test_framework.value(), cfg::TestFramework::GoogleTest);
  EXPECT_TRUE(cfg_full.testing_enabled.has_value());
  EXPECT_EQ(cfg_full.testing_enabled.value(), true);

  const std::filesystem::path CONFIG_NO_NAME { ASSETS_PATH / "config_no_name.toml" };
  EXPECT_THROW(const cfg::Config cfg_no_name { cfg::Config::from_file(CONFIG_NO_NAME) }, std::runtime_error);

  const std::filesystem::path CONFIG_NO_LANGUAGE { ASSETS_PATH / "config_no_language.toml" };
  EXPECT_THROW(const cfg::Config cfg_no_language { cfg::Config::from_file(CONFIG_NO_LANGUAGE) }, std::runtime_error);

  const std::filesystem::path CONFIG_NO_TARGET { ASSETS_PATH / "config_no_target.toml" };
  EXPECT_THROW(const cfg::Config cfg_no_target { cfg::Config::from_file(CONFIG_NO_TARGET) }, std::runtime_error);

  const std::filesystem::path CONFIG_MALFORMED { ASSETS_PATH / "config_malformed.toml" };
  EXPECT_THROW(const cfg::Config cfg_malformed { cfg::Config::from_file(CONFIG_MALFORMED) }, std::runtime_error);
}


TEST(ConfigTest, ToFile) {
  const std::filesystem::path tmp1 { ASSETS_PATH / "tmp1.toml" };

  cfg::Config cfg_full {};
  EXPECT_NO_THROW(cfg_full = cfg::Config::from_file(CONFIG_FULL));
  EXPECT_NO_THROW(cfg_full.to_file(tmp1));

  cfg::Config cfg_tmp {};
  EXPECT_NO_THROW(cfg_tmp = cfg::Config::from_file(tmp1));
  ASSERT_EQ(cfg_full.name, cfg_tmp.name);
  EXPECT_TRUE(std::filesystem::remove(tmp1));

  const std::filesystem::path tmp2 { ASSETS_PATH / "tmp2.toml" };
  cfg::Config cfg_minimum {};
  EXPECT_NO_THROW(cfg_minimum = cfg::Config::from_file(CONFIG_MINIMUM));

  EXPECT_NO_THROW(cfg_minimum.to_file(tmp2));
  EXPECT_NO_THROW(cfg_tmp = cfg::Config::from_file(tmp2));
  ASSERT_EQ(cfg_minimum.name, cfg_tmp.name);
  std::filesystem::remove(tmp2);
}


TEST(ConfigTest, Getters) {
  cfg::Config cfg_full {};
  EXPECT_NO_THROW(cfg_full = cfg::Config::from_file(CONFIG_FULL));

  EXPECT_NE(cfg_full.get_compiler(), cfg::Compiler::Default);
  EXPECT_EQ(cfg_full.get_generator(), cfg::Generator::Ninja);
  EXPECT_EQ(cfg_full.get_language_standard(), cfg::LanguageStandard::Cpp23);
  EXPECT_EQ(cfg_full.get_build_system(), cfg::BuildSystem::CMake);
  EXPECT_EQ(cfg_full.get_build_type(), cfg::BuildType::Debug);
  EXPECT_EQ(cfg_full.get_header_extension(), "hpp");
  EXPECT_EQ(cfg_full.get_source_extension(), "cpp");
  EXPECT_FALSE(cfg_full.is_multi_config());
  EXPECT_TRUE(cfg_full.are_gnu_extensions_enabled());
  EXPECT_TRUE(cfg_full.is_testing_enabled());
  EXPECT_TRUE(cfg_full.is_clang_tidy_enabled());
  EXPECT_TRUE(cfg_full.is_cppcheck_enabled());

  cfg::Config cfg_minimum {};
  EXPECT_NO_THROW(cfg_minimum = cfg::Config::from_file(CONFIG_MINIMUM));

  EXPECT_EQ(cfg_minimum.get_compiler(), system_default_compiler());
  EXPECT_EQ(cfg_minimum.get_generator(), system_default_generator());
  EXPECT_EQ(cfg_minimum.get_language_standard(), cfg::LanguageStandard::Cpp23);
  EXPECT_EQ(cfg_minimum.get_build_system(), cfg::BuildSystem::CMake);
  EXPECT_EQ(cfg_minimum.get_build_type(), cfg::BuildType::Debug);
  EXPECT_EQ(cfg_minimum.get_header_extension(), "hpp");
  EXPECT_EQ(cfg_minimum.get_source_extension(), "cpp");
  EXPECT_FALSE(cfg_minimum.is_multi_config());
  EXPECT_FALSE(cfg_minimum.are_gnu_extensions_enabled());
  EXPECT_FALSE(cfg_minimum.is_testing_enabled());
  EXPECT_FALSE(cfg_minimum.is_clang_tidy_enabled());
  EXPECT_FALSE(cfg_minimum.is_cppcheck_enabled());
}
