#include "src/cmd/commands.hpp"

#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

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
  bool deps { false };

  for (const auto arg : args) {
    if (arg == "--deps") {
      deps = true;
      break;
    }
    else if (arg == "--help") {
      utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
      return;
    }
    else {
      throw std::runtime_error { std::format("gojo clean: argument not recognized: {}", arg) };
    }
  }

  const cfg::Config cfg { cfg::Config::this_project() };

  if (deps) {
    std::filesystem::remove_all(utils::get_root_dir() / "deps");
  }
  std::filesystem::remove_all(cfg.get_build_dir());
}

}
