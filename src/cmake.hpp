/// @file      src/cmake.hpp
/// @brief     Functionality for configuring and building a CMake project.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_CMAKE_HPP
#define GOJO_SRC_CMAKE_HPP

#include <optional>
#include <string>
#include <string_view>

#include "src/config.hpp"


namespace cmake {

/// @brief Configure a CMake project.
/// @details This function can fail for any reason a CMake configuration can
///          fail.
/// @param cfg Project config to base CMake configuration on.
/// @param verbose If set to true, cmake output is forwarded to the user.
/// @param reconfigure Optional. If set to true, CMake assumes the project has
///                    been previously configured and that several CMake files
///                    remain in the build directory. If set to false, CMake
///                    assumes a fresh configuration with an empty build
///                    directory.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> configure(const config::GojoConfig& cfg,
                                     bool verbose,
                                     bool reconfigure=false);


/// @brief Build a CMake project.
/// @details This function can fail if there are any compilation errors or if
///          CMake has not been configured properly.
/// @param cfg Project config to get build settings from.
/// @param target Specific target to build. If this parameter is empty, the
///               entire project is built.
/// @param clean Determines if the CMake cache is cleaned before building.
/// @param verbose If set to true, cmake output is forwarded to the user.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> build(const config::GojoConfig& cfg,
                                 std::string_view target,
                                 bool clean,
                                 bool verbose);


/// @brief Clean the cache of a CMake project.
/// @details This function can fail if CMake has not been configured properly.
/// @param cfg Project config to get build settings from.
/// @param deep If set to true, the entire build directorry is removed. If set
///             to false, only the CMake cache is removed.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> clean(const config::GojoConfig& cfg, bool deep);

}  // namespace cmake

#endif
