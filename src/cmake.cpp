#include "src/cmake.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <string_view>

#include "src/config.hpp"
#include "src/utils.hpp"


namespace cmake {

std::optional<std::string> configure(const config::GojoConfig& cfg,
                                     bool verbose,
                                     bool reconfigure) {
  std::string_view build_tests { "OFF" };
  if (cfg.testing_enabled) {
    build_tests = "ON";
  }

  std::string toolchain {};
  if (cfg.pkg_manager == "conan") {
    constexpr std::string_view cmd_str {
      "-DCMAKE_TOOLCHAIN_FILE=deps/build/{}/generators/conan_toolchain.cmake"
    };
    toolchain = std::format(cmd_str, cfg.build_type);
  }

  std::string build_dir { std::format("build/{}", cfg.build_type) };
  if (cfg.multi_config) {
    build_dir = "build";
  }

  std::string compiler {};
  std::string_view lang { cfg.project_lang == "C++" ? "CXX" : "C" };
  if (cfg.compiler != "system default") {
    compiler = std::format("-DCMAKE_{}_COMPILER={}", lang, cfg.compiler);
  }

  std::string_view extensions { cfg.gnu_extensions ? "ON" : "OFF" };
  std::string generator {};
  if (cfg.generator != "system default") {
    generator = std::format("-G {}", cfg.generator);
  }

  // Keeps most of the build directory intact, but ignores the cache built
  // up from previous builds. This is different from a brand new configure
  // in an empty build directory.
  std::string_view fresh_build {};
  if (reconfigure) {
    fresh_build = "--fresh";
  }

  constexpr std::string_view cmdstr {
    "cmake -S . {} -B {} \"{}\" {} "
    "-DCMAKE_{}_STANDARD={} "
    "-DCMAKE_{}_STANDARD_REQUIRED=ON "
    "-DCMAKE_{}_EXTENSIONS={} "
    "-DCMAKE_BUILD_TYPE={} "  // ignored for multi-config generators
    "-DCMAKE_{}_FLAGS=\"{}\" "
    "-DBUILD_TESTING={} "
    "{}"
  };
  const std::string cmd { std::format(cmdstr,
    fresh_build, build_dir, generator, compiler,
    lang, cfg.lang_standard,
    lang,
    lang, extensions,
    cfg.build_type,
    lang, cfg.compiler_flags,
    build_tests,
    toolchain
  )};

  if (verbose) {
    std::println(MAGENTA("Configuring CMake...\n"));
    std::fflush(stdout);

    std::filesystem::current_path(cfg.project_root);
    auto result { utils::execute_command(cmd) };

    if (!result.success) {
      return std::make_optional("failed to configure CMake");
    }

    std::fflush(stdout);
    std::println(GREEN("\nCMake configured successfully"));
    return std::nullopt;
  }

  std::filesystem::current_path(cfg.project_root);
  auto result { utils::execute_command(cmd, true) };

  if (!result.success) {
    return std::make_optional(
      std::format("failed to configure CMake: \n{}", result.output)
    );
  }

  return std::nullopt;
}


std::optional<std::string> build(const config::GojoConfig& cfg,
                                 std::string_view target,
                                 bool clean,
                                 bool verbose) {
  std::string build_target {};
  if (!target.empty()) {
    build_target = std::format("--target {}", target);
  }

  std::string_view clean_flag { clean ? "--clean-first" : "" };

  std::string config {};
  std::string build_dir { std::format("build/{}", cfg.build_type) };
  if (cfg.multi_config) {
    config = std::format("--config {}", cfg.build_type);
    build_dir = "build";
  }

  const std::string cmd { 
    std::format("cmake --build {} {} {} --parallel {}",
                build_dir,
                config,
                build_target,
                clean_flag)
  };

  std::println(MAGENTA("Building project {}...\n"), cfg.project_name);
  std::fflush(stdout);

  std::filesystem::current_path(cfg.project_root);
  auto begin { std::chrono::system_clock::now() };

  auto result { utils::execute_command(cmd, !verbose) };

  if (!result.success) {
    return std::make_optional(
      std::format("failed to build project {}", cfg.project_name));
  }

  auto end { std::chrono::system_clock::now() };
  const std::chrono::duration<float> total_time { end - begin };

  std::fflush(stdout);
  std::println(
    GREEN("\nSuccessfully built in {} seconds"), total_time.count()
  );
  return std::nullopt;
}


std::optional<std::string> clean(const config::GojoConfig& cfg, bool deep) {
  if (deep) {
    return cmake::configure(cfg, true);
  }
    
  const std::string cmd { "cmake --build build --target clean" };
  std::filesystem::current_path(cfg.project_root);
  auto result { utils::execute_command(cmd) };
  if (!result.success) {
    return std::make_optional("failed to clean CMake project");
  }

  return std::nullopt;
}

}  // namespace cmake
