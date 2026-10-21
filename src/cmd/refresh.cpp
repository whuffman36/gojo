#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/cfg//enums.hpp"
#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
  gojo {2}refresh{0} {4}[options]{0}

{1}Summary:{0}
  Read the .gojo config file located at the root of the project and reconfigure
  the project. This command is necessary when changing the following fields in
  the .gojo config file:

    - 'language standard'
    - 'gnu extensions'
    - 'compiler'
    - 'compiler flags'
    - 'generator'
    - 'build type'
    - 'testing enabled'

{1}Options:{0}
  {4}--profile=<name>{0}   Configure the project with an existing gojo profile.
                     The profile must exist.
)"
};

}

namespace cmd {

void refresh(std::span<std::string_view> args) {
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
                std::format("gojo refresh: argument not recognized: {}", arg)
            };
        }
    }

    const cfg::Config cfg { cfg::Config::this_project() };

    if (!verbose) {
        verbose = cfg.is_verbose_enabled();
    }

    clean({});

    switch (cfg.get_build_system()) {
        case cfg::BuildSystem::CMake:
            return cmake::configure(cfg, verbose);
            break;
    }
}

}  // namespace cmd
