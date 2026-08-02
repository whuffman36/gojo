/// @file      src/utils.hpp
/// @brief     Common functionality used in several files and/or namespaces.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_UTILS_HPP
#define GOJO_SRC_UTILS_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>


/// @brief Wraps `str` in ANSI color codes to produce red output in the
///        terminal.
/// @details `str` must be a string literal. Color and format style is
///          cleared at the end of `str`.
/// @param str String to be wrapped.
/// @returns `str` wrapped in ANSI color codes for red text.
#define RED(str) "\033[1;31m" str "\033[0m"


/// @brief Wraps `str` in ANSI color codes to produce green output in the
///        terminal.
/// @details `str` must be a string literal. Color and format style is
///          cleared at the end of `str`.
/// @param str String to be wrapped.
/// @returns `str` wrapped in ANSI color codes for green text.
#define GREEN(str) "\033[1;32m" str "\033[0m"


/// @brief Wraps `str` in ANSI color codes to produce yellow output in the
///        terminal.
/// @details `str` must be a string literal. Color and format style is
///          cleared at the end of `str`.
/// @param str String to be wrapped.
/// @returns `str` wrapped in ANSI color codes for yellow text.
#define YELLOW(str) "\033[0;33m" str "\033[0m"


/// @brief Wraps `str` in ANSI color codes to produce magenta output in the
///        terminal.
/// @details `str` must be a string literal. Color and format style is
///          cleared at the end of `str`.
/// @param str String to be wrapped.
/// @returns `str` wrapped in ANSI color codes for magenta text.
#define MAGENTA(str) "\033[1;35m" str "\033[0m"


/// @brief Wraps `str` in ANSI color codes to produce cyan output in the
///        terminal.
/// @details `str` must be a string literal. Color and format style is
///          cleared at the end of `str`.
/// @param str String to be wrapped.
/// @returns `str` wrapped in ANSI color codes for cyan text.
#define CYAN(str) "\033[0;36m" str "\033[0m"


namespace utils {

/// @brief ANSI color code for yellow text.
constexpr std::string_view YELLOW       { "\033[0;33m" };

/// @brief ANSI color code for magenta text.
constexpr std::string_view MAGENTA      { "\033[0;35m" };

/// @brief ANSI color code for bold magenta text.
constexpr std::string_view MAGENTA_BOLD { "\033[1;35m" };

/// @brief ANSI color code for cyan text.
constexpr std::string_view CYAN         { "\033[0;36m" };

/// @brief ANSI color code to clear all previous formatting.
constexpr std::string CLEAR             { "\033[0m" };

/// @brief C11 or C++11.
constexpr int STD11 { 11 };

/// @brief C++20.
constexpr int STD20 { 20 };

/// @brief C++23.
constexpr int STD23 { 23 };


/// @brief Custom type to capture the result and output of a child process.
/// @property std::string output Contains output captured from the child
///           process.
/// @property bool success True if the child process exited successfully.
struct CommandResult {
  std::string output;
  bool success;
};


/// @brief Execute a shell command.
/// @details A new child process is created to execute `cmd`.
/// @param cmd Command to be executed by the system shell.
/// @param capture_output Optional. If set to true, output is captured from
///                       the child process and returned to the caller without
///                       sending it to stdout or stderr. If set to false, any
///                       output from the child process is piped directly to
///                       stdout and/or stderr and cannot be accessed by the
///                       caller.
/// @returns `CommandResult` type containing whether or not the child process
///          exited successfully and the output of the child process, which is
///          dependent on the `capture_output` parameter.
CommandResult execute_command(const std::string& cmd,
                              bool capture_output=false);


std::optional<std::string>
read_file_into_memory(const std::filesystem::path& file);


std::optional<std::string>
append_file_at_line(const std::filesystem::path& file,
                    std::string_view line_to_match,
                    std::string_view contents);


/// @brief Convert a string to uppercase.
/// @param str String to be converted to uppercase.
/// @returns `str` converted to uppercase.
std::string to_upper(std::string_view str);


/// @brief Convert a string to lowercase.
/// @param str String to be converted to lowercase.
/// @returns `str` converted to lowercase.
std::string to_lower(std::string_view str);


/// @brief Get the home directory of the current user.
/// @details The home directory is equivalent to ~/ in unix environments.
/// @returns The home directory of the current user.
const std::string_view get_home_dir();


/// @brief Get the directory where gojo profiles are stored.
/// @details gojo profiles can be found at ~/.gojocli/profiles.
/// @returns The gojo profiles directory of the current user.
const std::string get_profiles_dir();

}  // namespace utils

#endif
