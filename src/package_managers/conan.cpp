#include "src/package_managers/conan.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/utils.hpp"

namespace {

const std::filesystem::path get_profiles_dir() {
    static const std::filesystem::path profile_dir { utils::get_home_dir() /
                                                     ".conan2" / "profiles" };

    return profile_dir;
}

std::string get_compiler_version(const cfg::Config& cfg) {
    const std::string cmd { std::format(
        "{} -dumpversion", cfg::to_absolute_path(cfg.get_compiler())
    ) };
    const auto result { utils::execute_command(
        cmd.c_str(), true, false, cfg.get_build_dir() / "log.txt"
    ) };
    if (!result.success) {
        throw std::runtime_error { std::format(
            "compiler not found: {}", cfg::to_absolute_path(cfg.get_compiler())
        ) };
    }

    // Return the major version number without any decimals.
    const auto idx { result.output.find_first_of('.') };
    if (idx == std::string::npos) {
        // No decimals included in the string.
        return result.output;
    }

    return result.output.substr(0, idx);
}

void create_default_profile(const cfg::Config& cfg) {
    // Check for conan profile
    if (!std::filesystem::exists(get_profiles_dir() / "default")) {
        const auto result {
            utils::execute_command("conan profile detect --force", false, false,
                                   cfg.get_build_dir() / "log.txt")
        };
        if (!result.success) {
            throw std::runtime_error {
                "failed to detect default conan profile"
            };
        }
    }
}

}  // namespace

namespace conan {

Profile Profile::from_file(std::string_view profile_name) {
    const std::filesystem::path profile_path { get_profiles_dir() /
                                               profile_name };
    if (!std::filesystem::exists(profile_path)) {
        throw std::runtime_error { std::format("file not found: {}",
                                               profile_path.string()) };
    }

    const std::ifstream profile_file(profile_path);
    if (!profile_file.is_open()) {
        throw std::runtime_error { std::format("failed to open file: {}",
                                               profile_path.string()) };
    }

    std::stringstream profile_contents {};
    profile_contents << profile_file.rdbuf();

    conan::Profile profile {};
    std::string line {};
    std::string_view label {};
    std::string_view value {};

    while (std::getline(profile_contents, line)) {
        // Conan profiles typically start with a '[settings]' section.
        if (line.starts_with('[')) {
            continue;
        }

        // Split line by '='.
        const std::size_t index { line.find('=') };
        if (index == std::string::npos) {
            throw std::runtime_error {
                std::format("conan profile '{}' malformed: '{}'",
                            profile_path.string(), line)
            };
        }

        label = std::string_view(&line.at(0), index);
        value = std::string_view(&line.at(index + 1), line.size() - index - 1);

        if (label == "arch") {
            profile.arch = value;
        } else if (label == "build_type") {
            profile.build_type = value;
        } else if (label == "compiler") {
            profile.compiler = value;
        } else if (label == "compiler.cppstd" || label == "compiler.cstd") {
            profile.std = value;
        } else if (label == "compiler.libcxx") {
            profile.lib = value;
        } else if (label == "compiler.version") {
            profile.compiler_version = value;
        } else if (label == "os") {
            profile.os = value;
        } else {
            // Not a field that we need to care about, but we should still
            // copy it over to the profile for this project.
            profile.other_settings.append(std::format("{}={}\n", label, value));
        }
    }

    return profile;
}

void Profile::to_file(const cfg::Config& cfg) {
    const std::filesystem::path profile_path { get_profiles_dir() / cfg.name };

    std::ofstream profile_file(profile_path, std::ios::trunc);
    if (!profile_file.is_open()) {
        throw std::runtime_error { std::format("failed to open file: {}",
                                               profile_path.string()) };
    }

    std::string_view cfg_build_type { cfg::to_str(cfg.get_build_type()) };

    std::string_view compiler_str {};

    switch (cfg.get_compiler()) {
        case cfg::Compiler::AppleClang:
        case cfg::Compiler::AppleClangPp: compiler_str = "apple-clang"; break;
        case cfg::Compiler::Clang:
        case cfg::Compiler::ClangPp:
        case cfg::Compiler::HomebrewClang:
        case cfg::Compiler::HomebrewClangPp: compiler_str = "clang"; break;
        case cfg::Compiler::Gcc:
        case cfg::Compiler::Gpp:
        case cfg::Compiler::HomebrewGcc:
        case cfg::Compiler::HomebrewGpp: compiler_str = "gcc"; break;
        case cfg::Compiler::Default: std::unreachable(); break;
    }

    std::string language_standard {};
    switch (cfg.language) {
        case cfg::Language::C:
            language_standard.append("compiler.cstd=");
            break;
        case cfg::Language::Cpp:
            language_standard.append("compiler.cppstd=");
            break;
    }

    if (cfg.are_gnu_extensions_enabled()) {
        language_standard.append("gnu");
    }
    language_standard.append(cfg::to_str(cfg.get_language_standard()));

    const std::string updated_profile {
        std::format(
            R"([settings]
arch={0}
build_type={1}
compiler={2}
{3}
compiler.libcxx={4}
compiler.version={5}
os={6}
{7})",
            arch,                       // {0}
            cfg_build_type,             // {1}
            compiler_str,               // {2}
            language_standard,          // {3}
            lib,                        // {4}
            get_compiler_version(cfg),  // {5}
            os,                         // {6}
            other_settings
        )  // {7}
    };

    profile_file << updated_profile << std::flush;
}

void search(std::string_view package) {
    const std::string cmd { std::format("conan search {}", package) };

    const auto result { utils::execute_command(cmd.c_str(), false, true) };
    if (!result.success) {
        throw std::runtime_error { "gojo dep search: package not found" };
    }
}

void install(const cfg::Config& cfg, bool verbose) {
    const std::filesystem::path profile_path { get_profiles_dir() / cfg.name };
    if (!std::filesystem::exists(profile_path)) {
        create_default_profile(cfg);
        Profile::from_file("default").to_file(cfg);
    }

    utils::gojo_print(utils::LogLevel::Status,
                      "Installing conan dependencies...\n");
    std::fflush(stdout);

    constexpr std::string_view cmd_str {
        "conan install {} "
        "--deployer=full_deploy "
        "--output-folder={} "
        "--build=missing "
        "-c tools.cmake.cmaketoolchain:user_presets=\"\" "
        "--profile={} "
        "-s build_type={}"
    };

    std::string cmd {};
    int num_build_types_specified { 0 };
    for (std::size_t idx { 0 }; idx < cfg.build_types.size(); ++idx) {
        if (cfg.build_types.at(idx).has_value()) {
            num_build_types_specified++;
            cmd = std::format(
                cmd_str, utils::get_root_dir().string(),
                (utils::get_root_dir() / "deps").string(), cfg.name,
                // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
                cfg::to_str(cfg.build_types.at(idx).value())
            );
            const auto result { utils::execute_command(
                cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt"
            ) };
            if (!result.success) {
                throw std::runtime_error {
                    "\nfailed to install conan dependencies"
                };
            }
        }
    }

    if (num_build_types_specified == 0) {
        cmd = std::format(cmd_str, utils::get_root_dir().string(),
                          (utils::get_root_dir() / "deps").string(), cfg.name,
                          cfg::to_str(cfg::BuildType::Debug));
        const auto result { utils::execute_command(
            cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt"
        ) };
        if (!result.success) {
            throw std::runtime_error {
                "\nfailed to install conan dependencies"
            };
        }
    }

    std::fflush(stdout);
    utils::gojo_print(utils::LogLevel::Success,
                      "\nDependencies installed successfully");
    utils::gojo_print(
        utils::LogLevel::Status,
        "\nAdd the following line to your CMakeLists.txt in the root of the project:"
    );
    utils::gojo_print(utils::LogLevel::Info,
                      "\tfind_package(<package_name> REQUIRED)\n");
    utils::gojo_print(utils::LogLevel::Status, "for each package installed.");
}

void uninstall(std::string_view package) {
    const std::string cmd { std::format("conan remove {} --confirm", package) };

    const auto result { utils::execute_command(cmd.c_str(), false, true) };
    if (!result.success) {
        throw std::runtime_error { "gojo dep uninstall: package not found" };
    }
}

void add(const std::filesystem::path& conanfile, std::string_view package,
         std::string_view version) {
    std::string_view line_to_match {};
    std::string contents {};
    if (version == "latest") {
        version = "[*]";
    }
    if (conanfile.string().ends_with("py")) {
        line_to_match = "def requirements(self):\n";
        // Have to match the python indentation.
        contents =
            std::format("        self.requires(\"{}/{}\")", package, version);
    }
    if (conanfile.string().ends_with("txt")) {
        line_to_match = "[requires]\n";
        contents = std::format("{}/{}", package, version);
    }
    utils::append_file_at_line(conanfile, line_to_match, contents);
}

void remove(const std::filesystem::path& conanfile, std::string_view package) {
    std::string file_contents { utils::get_file_contents(conanfile) };
    const auto idx { file_contents.find(package) };

    if (idx == std::string::npos) {
        throw std::runtime_error {
            std::format("package not found in conanfile: {}", package)
        };
    }

    // Search left until a newline is found.
    std::size_t left_idx { idx };
    while (file_contents.at(left_idx) != '\n' && left_idx > 0) {
        left_idx--;
    }

    // Search right until a newline is found.
    std::size_t right_idx { idx };
    while (file_contents.at(right_idx) != '\n' &&
           right_idx < file_contents.size()) {
        right_idx++;
    }

    // Remove line from the file.
    file_contents.erase(left_idx, right_idx - left_idx);

    // Write modified file back.
    std::ofstream fstream { conanfile };
    fstream << file_contents;
}

}  // namespace conan
