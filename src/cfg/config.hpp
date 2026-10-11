/// @file      src/config.hpp
/// @brief     Functionality for creating, reading, and writing gojo config
///            files and gojo profiles.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_CONFIG_HPP
#define GOJO_SRC_CONFIG_HPP

#include <array>
#include <filesystem>
#include <optional>
#include <string_view>

#include "src/cfg/enums.hpp"


namespace cfg {

class Config {
public:
  Config() = default;
  Config(Config&&) = default;
  Config& operator=(Config&&) = default;
  ~Config() = default;

  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  static Config this_project();

  static Config from_file(const std::filesystem::path& file);

  void to_file(const std::filesystem::path& file) const;

  Compiler get_compiler() const;

  Generator get_generator() const;

  LanguageStandard get_language_standard() const;

  BuildSystem get_build_system() const;

  BuildType get_build_type() const;

  const std::filesystem::path get_build_dir() const;

  const std::string_view get_header_extension() const;

  const std::string_view get_source_extension() const;

  bool is_multi_config() const;

  bool are_gnu_extensions_enabled() const;

  bool is_testing_enabled() const;

  bool is_clang_tidy_enabled() const;

  bool is_cppcheck_enabled() const;


  std::optional<std::string> compiler_flags;
  std::optional<std::string> clang_tidy_args;
  std::optional<std::string> cppcheck_checks;
  std::optional<std::string> cppcheck_args;
  std::optional<std::string> language_server;

  std::string name;

  std::array<std::optional<BuildType>, 4> build_types;
  std::optional<LanguageStandard> language_standard;
  std::optional<Compiler> compiler;
  std::optional<BuildSystem> build_system;
  std::optional<Generator> generator;
  std::optional<BuildType> build_type;
  std::optional<PackageManager> package_manager;
  std::optional<FormatStyle> clang_format_style;
  std::optional<TestFramework> test_framework;

  std::optional<bool> gnu_extensions_enabled;
  std::optional<bool> testing_enabled;
  std::optional<bool> clang_tidy_enabled;
  std::optional<bool> cppcheck_enabled;

  Language language;
  Target target;
};

}  // namespace config

#endif
