#include <cstdint>
#include <filesystem>
#include <flat_map>
#include <format>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/cfg//enums.hpp"
#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/package_managers/conan.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
  gojo {2}dep{0} {4}[options]{0}

{1}Summary:{0}
  TODO

{1}Options:{0}
  TODO
)"
};

enum class Subcommand : std::uint8_t {
    Search,
    Install,
    Add,
    Remove,
    Uninstall,
};

std::optional<Subcommand> subcommand_from_str(std::string_view str) {
    static const std::flat_map<std::string_view, Subcommand> map {
        { "search", Subcommand::Search },
        { "install", Subcommand::Install },
        { "add", Subcommand::Add },
        { "remove", Subcommand::Remove },
        { "uninstall", Subcommand::Uninstall },
    };

    const auto result { map.find(str) };
    if (result == map.end()) {
        return std::nullopt;
    }

    return result->second;
}

}  // namespace

namespace cmd {

void dep(std::span<std::string_view> args) {
    if (args.empty()) {
        throw std::runtime_error { "gojo dep: missing option" };
    }

    std::string_view dep_option = args.front();
    if (dep_option == "--help") {
        utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
        return;
    }

    auto subcommand { subcommand_from_str(dep_option) };
    if (!subcommand.has_value()) {
        throw std::runtime_error {
            std::format("gojo dep: option not recognized: {}", dep_option)
        };
    }

    cfg::Config cfg { cfg::Config::this_project() };
    if (!cfg.package_manager.has_value()) {
        throw std::runtime_error { "gojo dep: no package manager enabled" };
    }

    bool verbose { cfg.is_verbose_enabled() };

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    switch (subcommand.value()) {
        case Subcommand::Search:
            if (args.size() > 2) {
                throw std::runtime_error {
                    "gojo dep search: too many arguments"
                };
            }

            switch (cfg.package_manager.value()) {
                case cfg::PackageManager::Conan:
                    // clang tidy doesn't like the use of unchecked access in
                    // containers. There is literally no other way to access
                    // std::span in C++23.
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
                    conan::search(args[1]);
                    break;
                case cfg::PackageManager::Vcpkg: break;
            }
            break;
        case Subcommand::Install:
            if (args.size() > 2) {
                throw std::runtime_error {
                    "gojo dep install: too many arguments"
                };
            }
            if (args.size() > 1 &&
                (args[1] != "--verbose" && args[1] != "-v")) {
                throw std::runtime_error { std::format(
                    "gojo dep install: argument not recognized: {}", args[1]
                ) };
            } else if (args.size() > 1 &&
                       (args[1] == "--verbose" || args[1] == "-v")) {
                verbose = true;
            }
            switch (cfg.package_manager.value()) {
                case cfg::PackageManager::Conan:
                    conan::install(cfg, verbose);
                    break;
                case cfg::PackageManager::Vcpkg: break;
            }
            break;
        case Subcommand::Add:
            {
                if (args.size() < 2) {
                    throw std::runtime_error {
                        "gojo dep add: missing argument"
                    };
                }
                if (args.size() > 3) {
                    throw std::runtime_error {
                        "gojo dep add: too many arguments"
                    };
                }
                std::string_view version { "latest" };
                if (args.size() == 3) {
                    version = args[2];
                }
                switch (cfg.package_manager.value()) {
                    case cfg::PackageManager::Conan:
                        {
                            std::filesystem::path conanfile {
                                utils::get_root_dir() / "conanfile.py"
                            };
                            if (!std::filesystem::exists(conanfile)) {
                                conanfile =
                                    utils::get_root_dir() / "conanfile.txt";
                            }
                            if (!std::filesystem::exists(conanfile)) {
                                throw std::runtime_error { std::format(
                                    "file not found: {}", conanfile.string()
                                ) };
                            }
                            conan::add(conanfile, args[1], version);
                            break;
                        }
                    case cfg::PackageManager::Vcpkg: break;
                }
                break;
            }
        case Subcommand::Remove:
            {
                if (args.size() < 2) {
                    throw std::runtime_error {
                        "gojo dep remove: missing argument"
                    };
                }
                if (args.size() > 2) {
                    throw std::runtime_error {
                        "gojo dep remove: too many arguments"
                    };
                }

                switch (cfg.package_manager.value()) {
                    case cfg::PackageManager::Conan:
                        {
                            std::filesystem::path conanfile {
                                utils::get_root_dir() / "conanfile.py"
                            };
                            if (!std::filesystem::exists(conanfile)) {
                                conanfile =
                                    utils::get_root_dir() / "conanfile.txt";
                            }
                            if (!std::filesystem::exists(conanfile)) {
                                throw std::runtime_error { std::format(
                                    "file not found: {}", conanfile.string()
                                ) };
                            }
                            conan::remove(conanfile, args[1]);
                            break;
                        }
                    case cfg::PackageManager::Vcpkg: break;
                }
                break;
            }
        case Subcommand::Uninstall:
            if (args.size() < 2) {
                throw std::runtime_error {
                    "gojo dep uninstall: missing argument"
                };
            }
            if (args.size() > 2) {
                throw std::runtime_error {
                    "gojo dep uninstall: too many arguments"
                };
            }

            switch (cfg.package_manager.value()) {
                case cfg::PackageManager::Conan:
                    {
                        conan::uninstall(args[1]);
                        break;
                    }
                case cfg::PackageManager::Vcpkg: break;
            }
            break;
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
}

}  // namespace cmd
