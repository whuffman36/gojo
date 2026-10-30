#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
  gojo {2}update{0} {3}[options]{0}

{1}Summary:{0}
  TODO

{1}Options:{0}
  {3}--help{0}   Configure the project with an existing gojo profile.
                     The profile must exist.

  {3}--verbose | -v
)"
};

}

namespace cmd {

void update(std::span<std::string_view> args) {
    bool verbose { false };
    for (const auto arg : args) {
        if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        }
        else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
        else {
            throw std::runtime_error {
                std::format("gojo update: argument not recognized: {}", arg)
            };
        }
    }

    constexpr std::string_view install_link {
        "https://raw.githubusercontent.com/whuffman36/gojo/refs/heads/main/install"
    };
    constexpr std::string_view curl_args {
        "--proto '=https' --tlsv1.2 -sSf -L"
    };
    constexpr std::string_view script_name { "gojo-upgrade" };
    const std::filesystem::path gojo_bin_dir { utils::get_home_dir() / ".gojo" /
                                               "bin" };

    utils::gojo_print(utils::LogLevel::Status, "Fetching latest version...");
    const std::string cmd { std::format(
        "curl {0} {1}/{2} > {3}/{2}",
        curl_args,             // {0}
        install_link,          // {1}
        script_name,           // {2}
        gojo_bin_dir.string()  // {3}
    ) };
    const auto result { utils::execute_command(cmd.c_str(), false, verbose) };
    if (!result.success) {
        throw std::runtime_error { "failed to fetch latest version" };
    }
}

}  // namespace cmd
