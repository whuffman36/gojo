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
                                   bool verbbose);

}  // namespace conan

#endif
