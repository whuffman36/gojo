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
  gojo {2}new{0} {3}<name>{0} {3}<path>{0} {3}[options]{0}

{1}Summary:{0}
  Create a new gojo project in the current directory. A name arguemnt
  is required. If no options are supplied, a default C++ project is
  created. The settings for a default C++ project can be found in the
  'gojo info' command.

{1}Arguments:{0}
  {3}name{0}            Name of the gojo project to be created.

{1}Options:{0}
  {3}--file | -f{0}       Create a new gojo project using a .gojo config
                    file located in the current directory. This option
                    syngergizes well with 'gojo template'. All other
                    arguments and options are ignored when using --file,
                    including <name>. This command will fail if no .gojo
                    config file is found in the current directory.

  {3}-i{0}                Create a new gojo project by answering several
                    interactive prompts. All other arguments and options
                    are ignored when using -i, including <name>.

  {3}--lib{0}             Set the project to be a library. By default, a new
                    gojo project is a binary executable.

  {3}--lang=<c|c++>{0}    Set the language of the project. The language options
                    are 'c' or 'c++'. By default, a new gojo project is C++.

  {3}--profile=<name>{0}  Create a new gojo project from an existing profile.
                    The profile must exist. When combining this option with
                    options, this one must come first, ie
                    gojo init --profile=default --lib
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
