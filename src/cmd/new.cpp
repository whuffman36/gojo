/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <filesystem>
#include <format>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
    gojo {2}init{0} {3}<name> <path>{0} [options]

{1}Summary:{0}
    Create a new gojo project in the current working directory. By default,
    gojo creates a C++ binary without a package manager and testing disabled.
    The default compiler is chosen by your system.

{1}Arguments:{0}
    {3}name{0}                    Name of the project to be created. This name is also
                            used as the name of the executable in the gojo run
                            command and the name of the root CMake project.

    {3}path{0}                    Relative path where to create the new gojo project.

{1}Options:{0}
    --verbose, -v           Allow the output from the build system and package
                            manager to be forwarded to standard output.

    --interactive, -i       Create a new gojo project by answering config
                            questions one at a time.

    --lib                   Set the new gojo project to be a library.

    --lang=<lang>           Set the gojo project to be written in <lang>

    --profile=<profile>     Create a gojo project from the existing profile
                            with name <profile>

    --tests                 Set tests to be enabled for the new project.
)"
};

}  // namespace

namespace cmd {

void new_init(std::span<std::string_view> args, std::istream& input) {
    if (args.size() < 1) {
        throw std::runtime_error { "gojo new: missing path argument" };
    }

    for (const auto arg : args) {
        if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        }
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    const std::filesystem::path location { args[0] };

    if (location.filename().string() != location.string()) {
        if (!std::filesystem::create_directories(
                std::filesystem::absolute(location.parent_path())
            )) {
            throw std::runtime_error {
                std::format("gojo new: failed to create directory: {}",
                            location.parent_path().string())
            };
        }
        // init already creates directory from name
        std::filesystem::current_path(location.parent_path());
    }

    const std::string_view name { location.filename().string() };
    if (name.contains(' ')) {
        throw std::runtime_error { "gojo new: name must not contain spaces" };
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    args[0] = args[0].substr(args[0].size() - name.size(), name.size());
    init(args, input);
}

}  // namespace cmd
