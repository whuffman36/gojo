/// @file      src/commands.hpp
/// @brief     All the commands available to the user of the gojo CLI API.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#ifndef GOJO_SRC_CMD_COMMANDS_HPP
#define GOJO_SRC_CMD_COMMANDS_HPP

#include <iostream>
#include <span>
#include <string>
#include <string_view>

namespace cmd {

inline std::string args_to_string(std::span<std::string_view> args) {
    std::string str {};
    for (const auto arg : args) {
        str.append(arg);
        str.push_back(' ');
    }
    return str;
}

void init(std::span<std::string_view> args, std::istream& input = std::cin);

void new_init(std::span<std::string_view> args, std::istream& input = std::cin);

void build(std::span<std::string_view> args);

void run(std::span<std::string_view> args);

void test(std::span<std::string_view> args);

void clean(std::span<std::string_view> args);

void fmt(std::span<std::string_view> args);

void check(std::span<std::string_view> args);

void refresh(std::span<std::string_view> args);

void dep(std::span<std::string_view> args);

void profile(std::span<std::string_view> args);

void info();

void help();

}  // namespace cmd

#endif
