#include "src/cmd/commands.hpp"

#include <cstddef>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/package_managers/conan.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
R"({1}Command:{0}
  gojo {2}build{0} {4}[options]{0}

{1}Summary:{0}
  Compile the project. By default, all targets are built in debug mode using
  the compiler flags set in the .gojo config file located at the root of the
  project.

{1}Options:{0}
  {4}--release{0}         Compile the project with all compiler optimizations
                    enabled, namely -03. If the project is currently set
                    to debug mode, it will be reconfigured before
                    compilation. If the project is already in release mode,
                    this option has no effect. No debug symbol information
                    is kept when compiled in release mode, unless
                    overidden with specific compiler flags set in the
                    .gojo config file of the project.

  {4}--debug{0}           Compile the project with debug symbols and zero
                    compiler optimizations enabled. If the project is
                    currently set to release mode, it will be reconfigured
                    before compilation. If the project is already in debug
                    mode, this option has no effect.

  {4}--tests{0}           Enable testing in the project and compile unit tests
                    along with the source code. If the project currently
                    does not have testing enabled, it will be reconfigured
                    before compilation. If the project already has testing
                    enabled, this option has no effect.

  {4}--no-tests{0}        Disable testing in the project and omit unit tests from
                    the compilation process. If the project currently has
                    testing enabled, it will be reconfigured before
                    compilation. If the project currently does not have
                    testing enabled, this option has no effect.

  {4}--clean{0}           Clean the build artifacts and caches before compilation.

  {4}--target=<target>{0} Compile only a specific target instead of the whole
                    project.
)"
};

}


namespace cmd {

void build(std::span<std::string_view> args) {
  cfg::BuildType build_type { cfg::BuildType::Debug };
  std::string_view target {};
  bool testing_enabled { false };
  bool clean { false };
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
    else if (arg == "--clean") {
      clean = true;
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
      utils::print_info(HELP);
      return;
    }
    else {
      throw std::runtime_error { std::format("gojo build: argument not recognized: {}", arg) };
    }
  }

  cfg::Config cfg { cfg::Config::this_project() };

  reconfigure = ((cfg.get_build_type() != build_type) && !cfg.is_multi_config()) ||
                (cfg.is_testing_enabled() != testing_enabled);

  if (cfg.build_type.has_value() || (build_type != cfg::BuildType::Debug)) {
    cfg.build_type = build_type;

    bool exists_in_dep_build_types { false };
    std::size_t empty_idx { cfg.build_types.size() };
    for (std::size_t idx {0}; idx < cfg.build_types.size(); idx++) {
      if (cfg.build_types.at(idx).has_value()) {
        exists_in_dep_build_types = cfg.build_types.at(idx).value() == build_type;
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
        case cfg::PackageManager::Vcpkg:
          break;
      }
    }
    switch (cfg.get_build_system()) {
      case cfg::BuildSystem::CMake:
        cmake::configure(cfg, verbose, true);
        break;
    }

    cfg.to_file(utils::get_config_path());
  }

  switch (cfg.get_build_system()) {
    case cfg::BuildSystem::CMake:
      cmake::build(cfg, target, clean, verbose);
      break;
  }
}

}
