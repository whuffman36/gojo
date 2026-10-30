/// @file      src/utils.hpp
/// @brief     Common functionality used in several files and/or namespaces.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#ifndef GOJO_SRC_UTILS_HPP
#define GOJO_SRC_UTILS_HPP

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <utility>

// {0} CLEAR
// {1} MAGENTA_BOLD
// {2} MAGENTA
// {3} CYAN
#define INFO_COLORS                                                \
    utils::CLEAR, utils::MAGENTA_BOLD, utils::MAGENTA, utils::CYAN \

namespace utils {

constexpr std::string_view YELLOW { "\033[0;33m" };

constexpr std::string_view MAGENTA { "\033[0;35m" };

constexpr std::string_view MAGENTA_BOLD { "\033[1;35m" };

constexpr std::string_view GREEN_BOLD { "\033[1;32m" };

constexpr std::string_view RED_BOLD { "\033[1;31m" };

constexpr std::string_view CYAN { "\033[0;36m" };

constexpr std::string_view CLEAR { "\033[0m" };

enum class LogLevel : std::uint8_t {
    Info,
    Status,
    Success,
    Warning,
    Error,
};

struct CommandResult {
    std::string output;
    bool success;
};

CommandResult execute_command(
    const char* const cmd, bool capture_output, bool verbose,
    const std::optional<std::filesystem::path>& logfile = std::nullopt
);

std::string get_file_contents(const std::filesystem::path& file);

void append_file_at_line(const std::filesystem::path& file,
                         std::string_view line_to_match,
                         std::string_view contents);

std::string to_upper(std::string_view str);

std::string to_lower(std::string_view str);

template <typename... Args>
void gojo_print(LogLevel level, std::format_string<Args...> fmt,
                Args&&... args) {
    switch (level) {
        case LogLevel::Info:
            std::println(fmt, std::forward<Args>(args)...);
            break;
        case LogLevel::Status:
            std::print(MAGENTA_BOLD);
            std::print(fmt, std::forward<Args>(args)...);
            std::println(CLEAR);
            break;
        case LogLevel::Success:
            std::print(GREEN_BOLD);
            std::print(fmt, std::forward<Args>(args)...);
            std::println(CLEAR);
            break;
        case LogLevel::Warning:
            std::print(stderr, "{}[Warning]:{} ", YELLOW, CLEAR);
            std::println(stderr, fmt, std::forward<Args>(args)...);
            break;
        case LogLevel::Error:
            std::print(stderr, "{}[Error]:{} ", RED_BOLD, CLEAR);
            std::println(stderr, fmt, std::forward<Args>(args)...);
            break;
    }
}

void gojo_log(const std::filesystem::path& destination, const char* content,
              std::size_t content_size);

const std::filesystem::path get_root_dir();

const std::filesystem::path get_home_dir();

const std::filesystem::path get_profiles_dir();

const std::filesystem::path get_config_path();

}  // namespace utils

#endif
