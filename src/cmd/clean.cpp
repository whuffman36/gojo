#include "src/cmd/commands.hpp"

#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view HELP {
R"({1}Command:{0}
  gojo {2}clean{0} {4}[options]{0}

{1}Summary:{0}
  Remove build artifacts and/or caches.

{1}Options:{0}
  {4}--deep{0}          Remove all build artifacts and caches from the,
                  project, while keeping dependencies intact.

  {4}--all{0}           Remove everything from the build directory, including
                  dependencies. Use this option with caution as it
                  requires dependencies to be reinstalled and rebuilt,
                  and the project has to be re-configured.
)"
};

}


namespace cmd {

void clean(std::span<std::string_view> args) {
  bool deep { false };
  bool all { false };

  for (const auto arg : args) {
    if (arg == "--deep") {
      deep = true;
      break;
    }
    else if (arg == "--all") {
      all = true;
      break;
    }
    else if (arg == "--help") {
      utils::print_info(HELP);
      return;
    }
    else {
      throw std::runtime_error { std::format("gojo clean: argument not recognized: {}", arg) };
    }
  }

  const cfg::Config cfg { cfg::Config::this_project() };

  cfg::BuildSystem build_system {};
  if (!cfg.build_system.has_value()) {
    build_system = cfg::BuildSystem::CMake;
  }
  else {
    build_system = cfg.build_system.value();
  }
  switch (build_system) {
    case cfg::BuildSystem::CMake:
      if (all) {
        // Delete entire build folder.
        std::filesystem::remove_all(cfg.get_build_dir());
        std::filesystem::create_directory(cfg.get_build_dir());
        return cmake::configure(cfg, false);
      }
      else {
        return cmake::clean(cfg, deep);
      }
      break;
  }
}

}
