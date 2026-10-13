/// @file      src/cmake.hpp
/// @brief     Functionality for configuring and building a CMake project.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_BUILD_SYSTEMS_CMAKE_HPP
#define GOJO_SRC_BUILD_SYSTEMS_CMAKE_HPP

#include <string_view>

#include "src/cfg/config.hpp"


namespace cmake {

void configure(const cfg::Config& cfg, bool verbose);


void build(const cfg::Config& cfg, std::string_view target, bool verbose);


consteval std::string_view get_version() {
  // Version 3.31.6 is the default version of cmake on github actions.
  // Most recent version (July 2026): 4.3.4
  return "3.31.6...4.3.4";
}

}  // namespace cmake

#endif
