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

void configure(const cfg::Config& cfg, bool verbose, bool reconfigure=false);


void build(const cfg::Config& cfg, std::string_view target, bool clean, bool verbose);


void clean(const cfg::Config& cfg, bool deep);


std::string_view get_version();

}  // namespace cmake

#endif
