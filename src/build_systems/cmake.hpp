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

void test(const cfg::Config& cfg, std::string_view target);

}  // namespace cmake

#endif
