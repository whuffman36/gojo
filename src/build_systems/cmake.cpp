#include "src/build_systems/cmake.hpp"

#include <chrono>
#include <cstdio>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/utils.hpp"


namespace cmake {

void configure(const cfg::Config& cfg, bool verbose) {
  std::string_view build_type { cfg::to_str(cfg.get_build_type()) };

  const std::string build_dir { cfg.get_build_dir().string() };

  std::string_view generator { cfg::to_str(cfg.get_generator()) };

  std::string_view language {};
  switch (cfg.language) {
    case cfg::Language::C:
      language = "C";
      break;
    case cfg::Language::Cpp:
      language = "CXX";
      break;
  }

  std::string_view compiler { cfg::to_absolute_path(cfg.get_compiler()) };

  std::string_view language_standard { cfg::to_str(cfg.get_language_standard()) };

  std::string_view extensions { cfg.are_gnu_extensions_enabled() ? "ON" : "OFF" };

  std::string compiler_flags {};
  if (cfg.compiler_flags.has_value()) {
    compiler_flags = std::format(" -DCMAKE_{}_FLAGS=\"{}\"", language, cfg.compiler_flags.value());
  }

  std::string_view build_tests {};
  if (cfg.testing_enabled.has_value()) {
    if (cfg.testing_enabled.value()) {
      build_tests = " -DBUILD_TESTING=ON";
    }
    else {
      build_tests = " -DBUILD_TESTING=OFF";
    }
  }

  std::string toolchain {};
  if (cfg.package_manager.has_value() && cfg.package_manager.value() == cfg::PackageManager::Conan) {
    constexpr std::string_view cmd_str {
      " -DCMAKE_TOOLCHAIN_FILE=deps/build/{}/generators/conan_toolchain.cmake"
    };

    toolchain = std::format(cmd_str, build_type);
  }

  constexpr std::string_view cmdstr {
    "cmake "
    "-S . "
    "-B {0} "
    "-G \"{1}\" "
    "-DCMAKE_{2}_COMPILER={3} "
    "-DCMAKE_{2}_STANDARD={4} "
    "-DCMAKE_{2}_STANDARD_REQUIRED=ON "
    "-DCMAKE_{2}_EXTENSIONS={5} "
    "-DCMAKE_BUILD_TYPE={6}"  // ignored for multi-config generators
    "{7}"
    "{8}"
    "{9}"
  };
  const std::string cmd { std::format(cmdstr,
    build_dir,         // {0}
    generator,         // {1}
    language,          // {2}
    compiler,          // {3}
    language_standard, // {4}
    extensions,        // {5}
    build_type,        // {6}
    compiler_flags,    // {7}
    build_tests,       // {8}
    toolchain          // {9}
  )};

  if (verbose) {
    utils::gojo_print(utils::LogLevel::Status, "Configuring CMake...\n");
    utils::gojo_print(utils::LogLevel::Info, "command: {}\n", cmd);
    std::fflush(stdout);
  }

  auto result { utils::execute_command(cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt") };

  if (!result.success) {
    throw std::runtime_error { "failed to configure CMake project" };
  }

  if (verbose) {
    std::fflush(stdout);
    utils::gojo_print(utils::LogLevel::Success, "\nCMake configured successfully");
  }
}


void build(const cfg::Config& cfg, std::string_view target, bool verbose) {
  std::string build_target {};
  if (!target.empty()) {
    build_target = std::format("--target {} ", target);
  }

  std::string_view build_type { cfg::to_str(cfg.get_build_type()) };

  std::string config {};
  if (cfg.is_multi_config()) {
    config = std::format("--config {} ", build_type);
  }

  const std::string cmd { 
    std::format("cmake --build {} {}{}--parallel",
                cfg.get_build_dir().string(),
                config,
                build_target)
  };

  utils::gojo_print(utils::LogLevel::Status, "Building {} project {}...\n", cfg::to_str(cfg.language), cfg.name);
  std::fflush(stdout);

  auto begin { std::chrono::system_clock::now() };

  auto result { utils::execute_command(cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt") };

  auto end { std::chrono::system_clock::now() };

  if (!result.success) {
    throw std::runtime_error { "failed to build CMake project" };
  }

  const std::chrono::duration<float> total_time { end - begin };

  std::fflush(stdout);
  utils::gojo_print(utils::LogLevel::Success, "\nSuccessfully built in {} seconds", total_time.count());
}

}
