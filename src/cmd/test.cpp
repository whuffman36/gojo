#include <cstddef>
#include <format>
#include <span>
#include <src/cfg/enums.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
  gojo {2}test{0} {4}[options]{0}

{1}Summary:{0}
  Run unit tests in the project. Tests must be enabled in the .gojo config
  file and a call to 'gojo build' must be successfully completed before
  running this command.

{1}Options:{0}
  {4}--target=<target>{0} Test only a specific target intead of all unit tests.
)"
};

}

namespace cmd {

void test(std::span<std::string_view> args) {
    std::string target {};
    bool build_first { false };

    for (const auto arg : args) {
        if (arg.starts_with("--target=")) {
            // Length of "--target="
            constexpr std::size_t target_arg_len { 9 };
            target = std::format(" -R \"{}\"", arg.substr(target_arg_len));
        } else if (arg == "--build") {
            build_first = true;
        } else if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        } else {
            throw std::runtime_error {
                std::format("gojo test: argument not recognized: {}", arg)
            };
        }
    }

    const cfg::Config cfg { cfg::Config::this_project() };
    if (!cfg.is_testing_enabled()) {
        utils::gojo_print(utils::LogLevel::Warning, "testing not enabled");
        return;
    }

    switch (cfg.get_build_system()) {
        case cfg::BuildSystem::CMake:
            if (build_first) {
                cmake::build(cfg, target, cfg.is_verbose_enabled());
            }
            cmake::test(cfg, target);
            break;
    }
}

}  // namespace cmd
