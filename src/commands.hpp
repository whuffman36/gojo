/// @file      src/commands.hpp
/// @brief     All the commands available to the user of the gojo CLI API.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License


#ifndef GOJO_SRC_COMMANDS_HPP
#define GOJO_SRC_COMMANDS_HPP

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "src/config.hpp"


namespace commands {

/// @brief Create a new gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found or contains
///          syntax errors during file mode, or if there are any errors
///          creating or writing to files.
/// @param args Arguments passed to the init command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> init(std::span<std::string_view> args);


/// @brief Build a gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, if there
///          are any compilation errors, or if the meta build system used
///          by the project has not been configured properly.
/// @param args Arguments passed to the build command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> build(std::span<std::string_view> args);


/// @brief Run a gojo project's executable.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, if the
///          project's executable cannot be found, or if the project's target
///          is a library.
/// @param args Arguments passed to the run command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> run(std::span<std::string_view> args);


/// @brief Run a gojo project's unit tests.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, if the
///          project does not have any unit tests, or if the project does not
///          have testing enabled.
/// @param args Arguments passed to the test command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> test(std::span<std::string_view> args);


/// @brief Clean the build directory of a gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, or if
///          the project's meta build system has not been configured properly.
/// @param args Arguments passed to the clean command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> clean(std::span<std::string_view> args);


/// @brief Auto-format a gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, or if the gojo config file cannot be found.
/// @param args Arguments passed to the fmt command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> fmt(std::span<std::string_view> args);


/// @brief Run static analyzers on a gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, or if
///          the project's meta build system has not been configured properly.
/// @param args Arguments passed to the check command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> check(std::span<std::string_view> args);


/// @brief Create an empty gojo config file in the current directory.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized.
/// @param args Arguments to be passed to the file-template command.
std::optional<std::string> file_template(std::span<std::string_view> args);


/// @brief Re-configure a gojo project
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, or if
///          there are any syntax errors in the gojo config file.
/// @param args Arguments passed to the refresh command.
/// @param cfg_in `GojoConfig` object to reconfigure the project with.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> refresh(std::span<std::string_view> args,
                                   std::optional<config::GojoConfig> cfg_in);


/// @brief Manage dependencies.
std::optional<std::string> dep(std::span<std::string_view> args);


/// @brief Install and build the dependencies of a gojo project.
/// @details This function can fail if any of the arguemnts are malformed or
///          unrecognized, if the gojo config file cannot be found, if
///          there are any syntax errors in the gojo config file, if there
///          is no package manager config file found, if there are syntax
///          errors in the package manager config file, or if the package
///          manager cannot find a package.
/// @param args Arguments passed to the install command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> install(std::span<std::string_view> args);


/// @brief Manage gojo profiles.
/// @details This function takes care of profile creation, deletion, reads,
///          and writes of gojo profiles. This function can fail if any of
///          the arguments are malformed or unrecognized, if a gojo profile
///          cannot be found, or if there are any sytax errors in a gojo
///          profile.
/// @param args Arguments passed to the profile command.
/// @returns std::nullopt on success, an error message on failure.
std::optional<std::string> profile(std::span<std::string_view> args);


/// @brief Print the manual for the gojo CLI.
void info();


/// @brief Print the help screen for the gojo CLI.
void help();

}  // namespace commands

#endif
