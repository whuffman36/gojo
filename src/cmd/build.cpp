/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <cstddef>
#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/cmd/commands.hpp"
#include "src/package_managers/conan.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
    gojo {2}build{0} [options]

{1}Summary:{0}
    Compile the project using the settings defined in the gojo.toml config
    file. Some settings can be set or overridden from the command line, such
    as the build type and whether or not tests are compiled. This command
    detects if the project needs to be reconfigured before compiling.

{1}Options:{0}
    --release           Compile the project and any dependencies with all
                        compiler optimizations enabled.

    --debug             Compile the project and anydependencies with debug
                        symbols and zero compiler optimizations enabled.

    --tests             Enable testing in the project and compile unit tests
                        along with the source code.

    --no-tests          Disable testing in the project and omit unit tests from
                        the compilation process.

    --verbose, -v       Allow the output from the build system to be forwarded
                        to standard output.

    --target=<target>   Compile only a specific target instead of the whole
                        project.
)"
};

}

namespace cmd {

void build(std::span<std::string_view> args) {
    cfg::BuildType build_type { cfg::BuildType::Debug };
    std::string_view target {};
    bool testing_enabled { false };
    bool reconfigure { false };
    bool verbose { false };

    for (const auto arg : args) {
        if (arg == "--release") {
            build_type = cfg::BuildType::Release;
        }
        else if (arg == "--debug") {
            build_type = cfg::BuildType::Debug;
        }
        else if (arg == "--tests") {
            testing_enabled = true;
        }
        else if (arg == "--no-tests") {
            testing_enabled = false;
        }
        else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
        else if (arg.starts_with("--target=")) {
            // Length of "--target="
            constexpr std::size_t target_arg_len { 9 };
            target = arg.substr(target_arg_len);
        }
        else if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        }
        else {
            throw std::runtime_error {
                std::format("gojo build: argument not recognized: {}", arg)
            };
        }
    }

    cfg::Config cfg { cfg::Config::this_project() };

    if (!verbose) {
        verbose = cfg.is_verbose_enabled();
    }

    reconfigure =
        ((cfg.get_build_type() != build_type) && !cfg.is_multi_config()) ||
        (cfg.is_testing_enabled() != testing_enabled);

    if (!std::filesystem::exists(cfg.get_build_dir())) {
        reconfigure = true;
    }

    if (cfg.build_type.has_value() || (build_type != cfg::BuildType::Debug)) {
        cfg.build_type = build_type;

        bool exists_in_dep_build_types { false };
        std::size_t empty_idx { cfg.build_types.size() };
        for (std::size_t idx { 0 }; idx < cfg.build_types.size(); idx++) {
            if (cfg.build_types.at(idx).has_value()) {
                exists_in_dep_build_types =
                    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
                    cfg.build_types.at(idx).value() == build_type;
            }
            else {
                empty_idx = idx;
            }
        }

        if (!exists_in_dep_build_types) {
            cfg.build_types.at(empty_idx) = build_type;
        }
    }

    if (cfg.testing_enabled.has_value() || (testing_enabled != false)) {
        cfg.testing_enabled = testing_enabled;
    }

    if (reconfigure) {
        if (cfg.package_manager.has_value()) {
            switch (cfg.package_manager.value()) {
                case cfg::PackageManager::Conan:
                    conan::install(cfg, false);
                    break;
                case cfg::PackageManager::Vcpkg: break;
            }
        }
        switch (cfg.get_build_system()) {
            case cfg::BuildSystem::CMake: cmake::configure(cfg, verbose); break;
        }

        cfg.to_file(utils::get_config_path());
    }

    switch (cfg.get_build_system()) {
        case cfg::BuildSystem::CMake: cmake::build(cfg, target, verbose); break;
    }
}

}  // namespace cmd
