#include <cstdint>
#include <filesystem>
#include <flat_map>
#include <format>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
  gojo {2}profile{0} {4}[options]{0}

{1}Summary:{0}
  Manage global gojo profiles. Some options require arguemnts to work. Profiles
  are stored in ~/.gojocli/profiles as .gojo config files.

{1}Options{0}
  {4}create{0} {3}<name>{0}    Create a new gojo profile with the specified
                   name. The name arguemnt is required.

  {4}list{0}             List all of the current gojo profiles found in
                   the system.

  {4}show{0} {3}<name>{0}      Show the gojo profile with the specified name.

  {4}delete{0} {3}<name>{0}    Delete a gojo profile with the specified name.
                   specify 'all' to delete all gojo profiles.
)"
};

enum class Subcommand : std::uint8_t {
    Create,
    List,
    Show,
    Delete,
};

std::optional<Subcommand> subcommand_from_str(std::string_view str) {
    static const std::flat_map<std::string_view, Subcommand> map {
        { "create", Subcommand::Create },
        { "List", Subcommand::List },
        { "Show", Subcommand::Show },
        { "delete", Subcommand::Delete },
    };

    const auto result { map.find(str) };
    if (result == map.end()) {
        return std::nullopt;
    }

    return result->second;
}

}  // namespace

namespace cmd {

void profile(std::span<std::string_view> args) {
    if (args.empty()) {
        throw std::runtime_error { "gojo profile: missing option" };
    }

    std::string_view profile_subcommand = args.front();
    if (profile_subcommand == "--help") {
        utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
        return;
    }

    auto subcommand { subcommand_from_str(profile_subcommand) };
    if (!subcommand.has_value()) {
        throw std::runtime_error { std::format(
            "gojo profile: option not recognized: {}", profile_subcommand
        ) };
    }

    switch (subcommand.value()) {
        case Subcommand::Create:
            {
                if (args.size() == 1) {
                    throw std::runtime_error {
                        "gojo profile create: missing name argument"
                    };
                }
                if (args.size() > 2) {
                    throw std::runtime_error {
                        "gojo profile create: too many arguments"
                    };
                }
                // clang tidy doesn't like the use of unchecked access in
                // containers. There is literally no other way to access
                // std::span in C++23.
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
                const std::string_view name { args[1] };
                cfg::Config::this_project().to_file(utils::get_profiles_dir() /
                                                    name);
                break;
            }
        case Subcommand::List:
            {
                if (args.size() > 1) {
                    throw std::runtime_error {
                        "gojo profile list: too many arguments"
                    };
                }

                for (const auto& profile : std::filesystem::directory_iterator {
                         utils::get_profiles_dir() }) {
                    utils::gojo_print(utils::LogLevel::Info, "{}",
                                      profile.path().filename().string());
                }
                break;
            }
        case Subcommand::Show:
            {
                if (args.size() == 1) {
                    throw std::runtime_error {
                        "gojo profile show: missing name argument"
                    };
                }
                if (args.size() > 2) {
                    throw std::runtime_error {
                        "gojo profile show: too many arguments"
                    };
                }

                // clang tidy doesn't like the use of unchecked access in
                // containers. There is literally no other way to access
                // std::span in C++23.
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
                const std::string_view name { args[1] };
                std::string profile_contents {};
                if (name == "current") {
                    profile_contents =
                        utils::get_file_contents(utils::get_config_path());
                } else {
                    profile_contents =
                        utils::get_file_contents(utils::get_profiles_dir() /
                                                 name);
                }
                utils::gojo_print(utils::LogLevel::Info, "{}",
                                  profile_contents);
                break;
            }
        case Subcommand::Delete:
            if (args.size() == 1) {
                throw std::runtime_error {
                    "gojo profile delete: missing name argument"
                };
            }
            if (args.size() > 2) {
                throw std::runtime_error {
                    "gojo profile delete: too many arguments"
                };
            }

            // clang tidy doesn't like the use of unchecked access in
            // containers. There is literally no other way to access std::span
            // in C++23.
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            const std::string_view name { args[1] };
            if (name == "all") {
                std::filesystem::remove_all(utils::get_profiles_dir());
                std::filesystem::create_directory(utils::get_profiles_dir());
            } else {
                if (!std::filesystem::remove(utils::get_profiles_dir() /
                                             name)) {
                    throw std::runtime_error { std::format(
                        "gojo profile delete: no profile with name '{}'", name
                    ) };
                }
            }
            break;
    }
}

}  // namespace cmd
