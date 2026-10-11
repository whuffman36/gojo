/// @file      src/utils.hpp
/// @brief     Common functionality used in several files and/or namespaces.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_UTILS_HPP
#define GOJO_SRC_UTILS_HPP

#include <filesystem>
#include <string>
#include <string_view>

#define RED(str) "\033[1;31m" str "\033[0m"
#define GREEN(str) "\033[1;32m" str "\033[0m"
#define YELLOW(str) "\033[0;33m" str "\033[0m"
#define MAGENTA(str) "\033[1;35m" str "\033[0m"
#define CYAN(str) "\033[0;36m" str "\033[0m"


namespace utils {

constexpr std::string_view YELLOW { "\033[0;33m" };

constexpr std::string_view MAGENTA { "\033[0;35m" };

constexpr std::string_view MAGENTA_BOLD { "\033[1;35m" };

constexpr std::string_view CYAN { "\033[0;36m" };

constexpr std::string_view CLEAR { "\033[0m" };

struct CommandResult {
  std::string output;
  bool success;
};

CommandResult execute_command(const char* const cmd, bool capture_output=false) noexcept;

std::string get_file_contents(const std::filesystem::path& file);

void append_file_at_line(const std::filesystem::path& file, std::string_view line_to_match, std::string_view contents);

std::string to_upper(std::string_view str) noexcept;

std::string to_lower(std::string_view str) noexcept;

void print_status(std::string_view str) noexcept;

void print_success(std::string_view str) noexcept;

void print_error(std::string_view str) noexcept;

void print_warning(std::string_view str) noexcept;

void print_info(std::string_view str) noexcept;

const std::filesystem::path get_root_dir() noexcept;

const std::filesystem::path get_home_dir() noexcept;

const std::filesystem::path get_profiles_dir() noexcept;

const std::filesystem::path get_config_path() noexcept;

}  // namespace utils

#endif
