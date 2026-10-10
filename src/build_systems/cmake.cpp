#include "src/build_systems/cmake.hpp"

#include <chrono>
#include <cstdio>
#include <format>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/utils.hpp"


namespace cmake {

void configure(const cfg::Config& cfg, bool verbose, bool reconfigure) {
  // Keeps most of the build directory intact, but ignores the cache built
  // up from previous builds. This is different from a brand new configure
  // in an empty build directory.
  std::string_view fresh_build {};
  if (reconfigure) {
    fresh_build = "--fresh ";
  }

  std::string_view build_type { cfg::to_string(cfg.get_build_type()) };

  const std::string build_dir { cfg.get_build_dir().string() };

  std::string_view generator { cfg::to_string(cfg.get_generator()) };

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

  std::string_view language_standard {};
  if (cfg.language_standard.has_value()) {
    language_standard = cfg::to_string(cfg.language_standard.value());
  }
  else {
    language_standard = "23";
  }

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
    "{0}"
    "-S . "
    "-B {1} "
    "-G \"{2}\" "
    "-DCMAKE_{3}_COMPILER={4} "
    "-DCMAKE_{3}_STANDARD={5} "
    "-DCMAKE_{3}_STANDARD_REQUIRED=ON "
    "-DCMAKE_{3}_EXTENSIONS={6} "
    "-DCMAKE_BUILD_TYPE={7}"  // ignored for multi-config generators
    "{8}"
    "{9}"
    "{10}"
  };
  const std::string cmd { std::format(cmdstr,
    fresh_build,       // {0}
    build_dir,         // {1}
    generator,         // {2}
    language,          // {3}
    compiler,          // {4}
    language_standard, // {5}
    extensions,        // {6}
    build_type,        // {7}
    compiler_flags,    // {8}
    build_tests,       // {9}
    toolchain          // {10}
  )};

  if (verbose) {
    utils::print_status("Configuring CMake...\n");
    std::println("command: {}\n", cmd);
    std::fflush(stdout);

    auto result { utils::execute_command(cmd.c_str()) };

    if (!result.success) {
      throw std::runtime_error { "failed to configure CMake project" };
    }

    std::fflush(stdout);
    utils::print_success("\nCMake configured successfully");
    return;
  }

  auto result { utils::execute_command(cmd.c_str(), true) };

  if (!result.success) {
    throw std::runtime_error { std::format("failed to configure CMake project: \n{}", result.output) };
  }
}


void build(const cfg::Config& cfg, std::string_view target, bool clean, bool verbose) {
  std::string build_target {};
  if (!target.empty()) {
    build_target = std::format("--target {} ", target);
  }

  std::string_view clean_flag { clean ? " --clean-first" : "" };

  std::string_view build_type { cfg::to_string(cfg.get_build_type()) };

  std::string build_dir { cfg.get_build_dir().string() };
  std::string config {};
  if (cfg.is_multi_config()) {
    config = std::format("--config {} ", build_type);
  }

  const std::string cmd { 
    std::format("cmake --build {} {}{}--parallel{}",
                build_dir,
                config,
                build_target,
                clean_flag)
  };

  std::println(MAGENTA("Building project {}...\n"), cfg.name);
  std::fflush(stdout);

  auto begin { std::chrono::system_clock::now() };

  auto result { utils::execute_command(cmd.c_str(), !verbose) };

  if (!result.success) {
    std::println("{}", result.output);
    throw std::runtime_error { "failed to build CMake project" };
  }

  auto end { std::chrono::system_clock::now() };
  const std::chrono::duration<float> total_time { end - begin };

  std::fflush(stdout);
  std::println(
    GREEN("\nSuccessfully built in {} seconds"), total_time.count()
  );
}


void clean(const cfg::Config& cfg, bool deep) {
  if (deep) {
    return cmake::configure(cfg, false, true);
  }

  // turn into format string with cfg.get_build_dir()
  const std::string cmd { std::format("cmake --build {} --target clean", cfg.get_build_dir().string()) };
  auto result { utils::execute_command(cmd.c_str()) };
  if (!result.success) {
    throw std::runtime_error { "failed to clean CMake project" };
  }
}


std::string_view get_version() {
  // Version 4.0 cutoff support for version 3.5 and under and
  // was released in March 2025.
  // Most recent version (July 2026): 4.3.4
  return "3.31.6...4.3.4";
}

}  // namespace cmake
