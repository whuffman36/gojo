/// @file      src/config.hpp
/// @brief     Functionality for creating, reading, and writing gojo config
///            files and gojo profiles.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_CONFIG_HPP
#define GOJO_SRC_CONFIG_HPP

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>


namespace config {

/// @brief Represents a gojo config file.
struct GojoConfig {
  std::string project_name;
  std::string project_root;
  std::string project_lang;
  std::string executable_name;
  std::string compiler;
  std::string compiler_flags;
  std::string build_system;
  std::string generator;
  std::string build_type;
  std::string pkg_manager;
  std::string dep_build_types;
  std::string src_ext;
  std::string hdr_ext;
  std::string fmt_style;
  std::string clang_tidy_args;
  std::string cppcheck_checks;
  std::string cppcheck_args;
  std::string testing_framework;
  int lang_standard;
  bool is_library;
  bool testing_enabled;
  bool git_repo;
  bool clang_tidy_enabled;
  bool cppcheck_enabled;
  bool gnu_extensions;
  bool multi_config;
  bool clangd_enabled;
};


const GojoConfig DEFAULT = {
  .project_name = "",
  .project_root = ".",
  .project_lang = "C++",
  .executable_name = "",
  .compiler = "system default",
  .compiler_flags = "-Wall -Wextra -Wpedantic -Wconversion -Wshadow",
  .build_system = "CMake",
  .generator = "system default",
  .build_type = "Debug",
  .pkg_manager = "none",
  .dep_build_types = "",
  .src_ext = "cpp",
  .hdr_ext = "hpp",
  .fmt_style = "google",
  .clang_tidy_args = "",
  .cppcheck_checks = "warning,performance,portability,missingInclude",
  .cppcheck_args = "--inline-suppr --suppress=missingIncludeSystem",
  .testing_framework = "none",
  .lang_standard = 23,
  .is_library = false,
  .testing_enabled = true,
  .git_repo = true,
  .clang_tidy_enabled = true,
  .cppcheck_enabled = true,
  .gnu_extensions = true,
  .multi_config = false,
  .clangd_enabled = true,
};


/// @brief Returns true if the project's meta build system is using a multi-
///        config generator.
/// @param generator Generator used by the project's meta build system.
/// @returns True if `generator` is multi-config.
bool is_multi_config(std::string_view generator);


/// @brief Read a gojo config file into a usable `GojoConfig` object.
/// @details This function can fail if the file is not found or if there is a
///          sytax error in the gojo config file.
/// @param cfg_file Path to the gojo config file to be read.
/// @returns `GojoConfig` object on success, an error message on failure.
std::expected<GojoConfig, std::string>
read_from_file(const std::filesystem::path& cfg_file);


/// @brief Write a `GojoConfig` object to a gojo config file.
/// @details This function can fail if `file` is not found or cannot be
///          written to.
/// @param cfg `GojoConfig` object to be written from.
/// @param file Path of the file to be written to.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> write_to_file(const GojoConfig& cfg,
                                         const std::filesystem::path& file={});


/// @brief Create an empty gojo config file template.
/// @details This function can fail if `project_root` is not found or cannot
///          be written to.
/// @param project_root Path of the file to be written to.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string>
create_template(const std::filesystem::path& project_root);


/// @brief Create a gojo profile using the current project's gojo config file.
/// @details This function can fail if ~/.gojocli/profiles/ does not exist or
///          cannot be written to. If a gojo profile with name `profile_name`
///          already exists, that profile is overwritten.
/// @param profile_name Name of the profile to be created or overwritten.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> create_profile(std::string_view profile_name);


/// @brief Read a gojo profile into a usable `GojoConfig` object.
/// @details This function can fail if the profile is not found or if there is
///          a sytax error in the gojo profile.
/// @param profile_name Name of the profile to be read.
/// @returns `GojoConfig` object on success, an error message on failure.
std::expected<GojoConfig, std::string>
read_profile(std::string_view profile_name);

}  // namespace config


#endif
