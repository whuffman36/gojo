#include "src/cmd/commands.hpp"

#include <cstdint>
#include <flat_map>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/package_managers/conan.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg//enums.hpp"
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


const std::flat_map<std::string_view, Subcommand> subcommand_map {
  { "search", Subcommand::Search },
  { "install", Subcommand::Install },
  { "add", Subcommand::Add },
  { "remove", Subcommand::Remove },
  { "uninstall", Subcommand::Uninstall },
};

}


namespace cmd {

void dep(std::span<std::string_view> args) {
  if (args.empty()) {
    throw std::runtime_error { "gojo dep: missing option" };
  }

  std::string_view dep_option = args.front();
  if (dep_option == "--help") {
    utils::print_info(HELP);
    return;
  }
  
  auto subcommand_result { subcommand_map.find(dep_option) };
  if (subcommand_result == subcommand_map.end()) {
    throw std::runtime_error { std::format("gojo dep: option not recognized: {}", dep_option) };
  }

  cfg::Config cfg { cfg::Config::this_project() };
  if (cfg.package_manager.has_value()) {
    throw std::runtime_error { "gojo dep: no package manager enabled" };
  }

  switch (subcommand_result->second) {
    case Subcommand::Search:
      if (args.size() > 2) {
        throw std::runtime_error { "gojo dep search: too many arguments" };
      }

      switch (cfg.package_manager.value()) {
        case cfg::PackageManager::Conan:
          // clang tidy doesn't like the use of unchecked access in containers.
          // There is literally no other way to access std::span in C++23.
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
          conan::search(args[1]);
          break;
        case cfg::PackageManager::Vcpkg:
          break;
      }
      break;
    case Subcommand::Install:
      switch (cfg.package_manager.value()) {
        case cfg::PackageManager::Conan:
          conan::install(cfg, true);
          break;
        case cfg::PackageManager::Vcpkg:
          break;
      }
      break;
    case Subcommand::Add: {
      if (args.size() < 2) {
        throw std::runtime_error { "gojo dep add: missing argument" };
      }
      if (args.size() > 3) {
        throw std::runtime_error { "gojo dep add: too many arguments" };
      }
      std::string_view version { "latest" };
      if (args.size() == 3) {
        version = args[2];
      }
      switch (cfg.package_manager.value()) {
        case cfg::PackageManager::Conan: {
          std::filesystem::path conanfile { utils::get_root_dir() / "conanfile.py" };
          if (!std::filesystem::exists(conanfile)) {
            conanfile = utils::get_root_dir() / "conanfile.txt";
          }
          if (!std::filesystem::exists(conanfile)) {
            throw std::runtime_error { std::format("file not found: {}", conanfile.string()) };
          }
          conan::add(conanfile, args[1], version);
          break;
        }
        case cfg::PackageManager::Vcpkg:
          break;
      }
      break;
    }
    case Subcommand::Remove: {
      if (args.size() < 2) {
        throw std::runtime_error { "gojo dep remove: missing argument" };
      }
      if (args.size() > 2) {
        throw std::runtime_error { "gojo dep remove: too many arguments" };
      }

      switch (cfg.package_manager.value()) {
        case cfg::PackageManager::Conan: {
          std::filesystem::path conanfile { utils::get_root_dir() / "conanfile.py" };
          if (!std::filesystem::exists(conanfile)) {
            conanfile = utils::get_root_dir() / "conanfile.txt";
          }
          if (!std::filesystem::exists(conanfile)) {
            throw std::runtime_error { std::format("file not found: {}", conanfile.string()) };
          }
          conan::remove(conanfile, args[1]);
          break;
        }
        case cfg::PackageManager::Vcpkg:
          break;
      }
      break;
    }
    case Subcommand::Uninstall:
      if (args.size() < 2) {
        throw std::runtime_error { "gojo dep uninstall: missing argument" };
      }
      if (args.size() > 2) {
        throw std::runtime_error { "gojo dep uninstall: too many arguments" };
      }

      switch (cfg.package_manager.value()) {
        case cfg::PackageManager::Conan: {
          conan::uninstall(args[1]);
          break;
        }
        case cfg::PackageManager::Vcpkg:
          break;
      }
      break;
  }
}

}
