#include "src/conan.hpp"

#include <cstdio>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <optional>
#include <ostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "src/config.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view PROFILES = ".conan2/profiles";


std::optional<std::string> get_compiler_version(std::string_view compiler) {
  const std::string cmd { std::format("{} -dumpversion", compiler) };
  auto result { utils::execute_command(cmd, true) };
  if (!result.success) {
    return std::nullopt;
  }

  // Return the major version number without any decimals.
  auto idx { result.output.find_first_of('.') };
  if (idx == std::string::npos) {
    // No decimals included in the string.
    return result.output;
  }

  return result.output.substr(0, idx);
}


std::expected<conan::Profile, std::string>
read_profile(std::string_view profile_name) {
  const std::filesystem::path profile_file {
    std::format("{}/{}/{}", utils::get_home_dir(), PROFILES, profile_name)
  };
  if (!std::filesystem::exists(profile_file)) {
    return std::unexpected {
      std::format("file not found: {}", profile_file.string())
    };
  }

  /*
  std::ifstream profile_stream(profile_file);
  if (!profile_stream.is_open()) {
    return std::unexpected {
      std::format("failed to open file: {}", profile_file.string())
    };
  }
  */
  auto result { utils::read_file_into_memory(profile_file) };
  if (!result.has_value()) {
    return std::unexpected {
      std::format("failed to open file: {}", profile_file.string())
    };
  }

  std::string file_contents { std::move(result.value()) };
  std::stringstream ss_fc { std::move(file_contents) };

  conan::Profile profile {};
  std::string line {};
  std::string label {};
  std::string other_settings {};

  while (std::getline(ss_fc, line)) {
    // Conan profiles typically start with a '[settings]' section.
    if (line.starts_with('[')) {
      continue;
    }

    // Split line by '='.
    std::stringstream ss { std::move(line) };
    std::getline(ss, label, '=');

    if (label == "arch") {
      std::getline(ss, profile.arch);
    }
    else if (label == "build_type") {
      std::getline(ss, profile.build_type);
    }
    else if (label == "compiler") {
      std::getline(ss, profile.compiler);
    }
    else if (label == "compiler.cppstd" || label == "compiler.cstd") {
      std::getline(ss, profile.std);
    }
    else if (label == "compiler.libcxx") {
      std::getline(ss, profile.lib);
    }
    else if (label == "compiler.version") {
      std::getline(ss, profile.compiler_version);
    }
    else if (label == "os") {
      std::getline(ss, profile.os);
    }
    else {
      // Not a field that we need to care about, but we should still
      // copy it over to the profile for this project.
      std::getline(ss, other_settings);
      profile.other_settings.append(label + '=');
      profile.other_settings.append(other_settings + '\n');
    }
  }

  return profile;
}


std::optional<std::string> write_profile(const conan::Profile& profile,
                                         const config::GojoConfig& cfg) {
  const std::filesystem::path profile_file {
    std::format("{}/{}/{}", utils::get_home_dir(), PROFILES, cfg.project_name)
  };

  std::ofstream profile_stream(profile_file);
  if (!profile_stream.is_open()) {
    return std::make_optional(
      std::format("failed to open file: {}", profile_file.string())
    );
  }

  // Need to chop off the '++' for conan, only gcc and clang are valid
  // compilers, though they refer to the C++ versions (g++ and clang++)
  // when compiling C++ libraries.
  std::string_view compiler { profile.compiler };
  std::string compiler_version { profile.compiler_version };

  // Yeah there's some repetition here but I think it's better than two
  // nested if branches.
  if (cfg.compiler.starts_with("clang")) {
    compiler = "clang";
    auto cv_result { get_compiler_version(cfg.compiler) };
    if (!cv_result.has_value()) {
      return std::make_optional(
        std::format("failed to find compiler: {}", cfg.compiler)
      );
    }
    compiler_version = cv_result.value();
  }
  if (cfg.compiler.starts_with("g")) {
    compiler = "gcc";
    auto cv_result { get_compiler_version(cfg.compiler) };
    if (!cv_result.has_value()) {
      return std::make_optional(
        std::format("failed to find compiler: {}", cfg.compiler)
      );
    }
    compiler_version = cv_result.value();
  }

  const bool is_cpp { cfg.project_lang == "C++" };
  std::string lang_std { std::to_string(cfg.lang_standard) };
  if (cfg.gnu_extensions) {
    lang_std = "gnu" + lang_std;
  }

  const std::string updated_profile { std::format(
R"([settings]
arch={0}
build_type={1}
compiler={2}
{3}={4}
compiler.libcxx={5}
copmiler.version={6}
os={7}
{8})",
      profile.arch,            // {0}
      cfg.build_type,          // {1}
      compiler,                // {2}
      is_cpp ?                 // {3}
      "compiler.cppstd" :
      "compiler.cstd",
      lang_std,                // {4}
      profile.lib,             // {5}
      compiler_version,        // {6}
      profile.os,              // {7}
      profile.other_settings)  // {8}
  };

  profile_stream << updated_profile << std::flush;

  return std::nullopt;
}


std::optional<std::string> setup_profiles(const config::GojoConfig& cfg) {
  // Check for conan profile
  const std::filesystem::path default_profile {
    std::format("{}/{}/default", utils::get_home_dir(), PROFILES)
  };
  if (!std::filesystem::exists(default_profile)) {
    const std::string cmd { "conan profile detect --force" };
    const auto result { utils::execute_command(cmd) };
    if (!result.success) {
      return std::make_optional("failed to detect conan profile");
    }
  }

  auto read_result { read_profile("default") };
  if (!read_result.has_value()) {
    return std::make_optional(std::move(read_result.error()));
  }

  // Read default profile and create new profile based on a combination
  // of the default and the GojoConfig, ensuring the same compiler,
  // build mode, and language standard are used for both.
  const conan::Profile profile { read_result.value() };
  auto write_result { write_profile(profile, cfg) };
  if (write_result.has_value()) {
    return write_result;
  }

  return std::nullopt;
}

}  // namespace


namespace conan {

std::optional<std::string> search(std::string_view package) {
  constexpr std::string_view cmd_str {
    "conan search {}"
  };

  const std::string cmd {
    std::format(cmd_str, package)
  };

  auto result { utils::execute_command(cmd) };
  if (!result.success) {
    return std::make_optional("gojo dep search: package not found");
  }

  return std::nullopt;
}


std::optional<std::string> install(const config::GojoConfig& cfg,
                                   bool verbose) {
  const std::filesystem::path profile_file {
    std::format("{}/{}/{}", utils::get_home_dir(), PROFILES, cfg.project_name)
  };
  if (!std::filesystem::exists(profile_file)) {
    auto result { setup_profiles(cfg) };
    if (result.has_value()) {
      return result;
    }
  }

  std::println(MAGENTA("Installing conan dependencies...\n"));
  std::fflush(stdout);

  constexpr std::string_view cmd_str { 
    "conan install . "
    "--deployer=full_deploy "
    "--output-folder=./deps "
    "--build=missing "
    "-c tools.cmake.cmaketoolchain:user_presets=\"\" "
    "--profile={} "
    "-s build_type={}"
  };

  std::string build_type {};
  // Can't std::move cfg data, have to copy here.
  std::stringstream ss { cfg.dep_build_types };
  while (std::getline(ss, build_type, ',')) {
    const std::string cmd {
      std::format(cmd_str, cfg.project_name, build_type)
    };

    std::filesystem::current_path(cfg.project_root);
    auto result { utils::execute_command(cmd, !verbose) };
    if (!result.success) {
      return std::make_optional(
        std::format("{}\nfailed to install conan dependencies",
                    result.output)
      );
    }
  }
/*
  // In order to download the packages in debug and release mode, we have
  // to install two times, one for each build type.
  // Kinda sucks...
  const std::string cmd_dbg {
    std::format(cmd_str, cfg.project_name, "Debug")
  };
  const std::string cmd_rel {
    std::format(cmd_str, cfg.project_name, "Release")
  };

  std::println(MAGENTA("Installing conan dependencies...\n"));
  std::fflush(stdout);

  std::filesystem::current_path(cfg.project_root);
  auto dbg_result { utils::execute_command(cmd_dbg, !verbose) };
  if (!dbg_result.success) {
    return std::make_optional(
      std::format("{}\nfailed to install conan dependencies",
                  dbg_result.output)
    );
  }

  auto rel_result { utils::execute_command(cmd_rel, !verbose) };
  if (!rel_result.success) {
    return std::make_optional(
      std::format("{}\nfailed to install conan dependencies",
                  rel_result.output)
    );
  }
*/
  std::fflush(stdout);
  std::println(
    GREEN("\nDependencies installed successfully\n")
    MAGENTA(
      "\nAdd the following line to your CMakeLists.txt"
      " in the root of the project:"
    )
    CYAN("\tfind_package(<package_name> REQUIRED)\n")
    MAGENTA("for each package installed.")
  );
  return std::nullopt;
}


std::optional<std::string> uninstall(std::string_view package) {
  constexpr std::string_view cmd_str {
    "conan remove {}"
  };

  const std::string cmd {
    std::format(cmd_str, package)
  };

  auto result { utils::execute_command(cmd) };
  if (!result.success) {
    return std::make_optional("gojo dep uninstall: package not found");
  }

  return std::nullopt;
}


std::optional<std::string> add(const std::filesystem::path& conanfile,
                               std::string_view package,
                               std::string_view version) {
  std::string_view line_to_match {};
  std::string contents {};
  if (version == "latest") {
    version = "[>]";
  }
  if (conanfile.string().ends_with("py")) {
    line_to_match = "def requirements(self):";
    // Have to match the python indentation.
    contents = std::format("        self.requires({}/{})", package, version);
  }
  if (conanfile.string().ends_with("txt")) {
    line_to_match = "[requires]";
    contents = std::format("{}/{}", package, version);
  }
  auto result { 
    utils::append_file_at_line(conanfile, line_to_match, contents)
  };
  if (result.has_value()) {
    return result;
  }
  
  return std::nullopt;
}


std::optional<std::string> remove(const std::filesystem::path& conanfile,
                                  std::string_view package) {
  auto result { utils::read_file_into_memory(conanfile) };
  if (result.has_value()) {
    return result;
  }

  std:: string file_contents { std::move(result.value()) };
  auto idx { file_contents.find(package) };

  if (idx == std::string::npos) {
    return std::make_optional(
      std::format("package not found in conanfile: {}", package)
    );
  }

  // Search left until a newline is found.
  std::size_t left_idx { idx };
  while (file_contents[left_idx] != '\n' && left_idx > 0) {
    left_idx--;
  }

  // Search right until a newline is found.
  std::size_t right_idx { idx };
  while (file_contents[right_idx] != '\n' && right_idx < file_contents.size()) {
    right_idx++;
  }

  // Remove line from the file.
  file_contents.erase(left_idx, right_idx - left_idx);

  // Write modified file back.
  std::ofstream fstream { conanfile };
  fstream.write(file_contents.c_str(), static_cast<std::streamsize>(file_contents.size()));

  return std::nullopt;
}

}  // namespace conan
