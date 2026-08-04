/// @file      src/conan.hpp
/// @brief     Functionality for installing dependencies and managing profiles
///            through the conan package manager.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_CONAN_HPP
#define GOJO_SRC_CONAN_HPP

#include <optional>
#include <string>
#include <string_view>

#include "src/config.hpp"


namespace conan {

/// @brief Represents a conan profile.
struct Profile {
  std::string arch;
  std::string build_type;
  std::string compiler;
  std::string std;
  std::string lib;
  std::string compiler_version;
  std::string os;
  std::string other_settings;
};


/// @brief Search the conan package repo for a specific package.
/// @details This function can fail if conan is not installed on the system.
/// @param package Package to be searched in the conan repo.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> search(std::string_view package);


/// @brief Install project dependencies described in the conan config file.
/// @details This function can fail if there is no conan config file found
///          in the project root, if there are syntax errors in the conan
///          config file, or if any dependencies cannot be found by conan.
///          This function actually runs `conan install` twice, one for debug
///          mode and one for release mode.
/// @param cfg Project config to base conan profiles off of.
/// @param verbose If set to true, forwards conan output to the user.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> install(const config::GojoConfig& cfg,
                                   bool verbose);


/// @brief Uninstall a conan package from the local cache.
std::optional<std::string> uninstall(std::string_view package);


/// @brief Add dependency to the current project.
/// @details This function modifies the project root CMakeLists.txt and the
///          conan config file (either conanfile.py or conanfile.txt). This
///          function can fail if any of those files do not exist, or if the
///          files are malformed.
/// @param package Package to be added to the project.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> add(const std::filesystem::path& conanfile,
                               std::string_view package,
                               std::string_view version="latest");


/// @brief Remove dependency from the current project.
/// @details This function modifies the project root CMakeLists.txt and the
///          conan config file (either conanfile.py or conanfile.txt). This
///          function can fail if any of these files do not exist, or if the
///          files are malformed.
/// @param package Package to be removed from the project.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> remove(const std::filesystem::path& conanfile,
                                  std::string_view package);

}  // namespace conan

#endif
