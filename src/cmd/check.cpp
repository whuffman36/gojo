#include "src/cmd/commands.hpp"

#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/cfg//enums.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view HELP {
R"({1}Command:{0}
  gojo {2}check{0} {4}[options]{0}

{1}Summary:{0}
  Run static analysis checks on the code without building the project. There
  are two programs used to run static analysis checks, clang-tidy and
  cppcheck. Both can be enabled or disabled in the .gojo config file located
  at the root of the project. If neither program is enabled, this command has
  no effect. clang-tidy can be configured with the .clang-tidy file located
  at the root of the project, created by 'gojo init' if enabled. Additionally,
  command line arguments can be supplied in the .gojo config file at the root
  of the project using the 'clang-tidy arguments' field, which will take
  precedence over the .clang-tidy file. More information on clang-tidy can be
  found here: {4}https://clang.llvm.org/extra/clang-tidy/{0}
  cppcheck can be configured in the .gojo config file located in the root of
  the project using the 'cppcheck checks enabled' and 'cppcheck arguments'
  fields. The former decides which checks will be run and the latter is used
  to pass any command line options to cppcheck. More information on cppcheck
  can be found here: {4}https://cppcheck.sourceforge.io/manual.html{0}

{1}Options:{0}
  {4}--verbose | -v{0}  Print all output from the static analyzers enabled
                  in the project. By default, only errors are printed.
)"
};

}


namespace cmd {

void check(std::span<std::string_view> args) {
  bool verbose { false };

  for (const auto arg : args) {
    if (arg == "--verbose" || arg == "-v") {
      verbose = true;
    }
    else if (arg == "--help") {
      utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
      return;
    }
    else {
      throw std::runtime_error { std::format("gojo check: argument not recognized: {}", arg) };
    }
  }

  const cfg::Config cfg { cfg::Config::this_project() };

  if (!cfg.is_clang_tidy_enabled() && !cfg.is_cppcheck_enabled()) {
    return;
  }

  if (!std::filesystem::exists(cfg.get_build_dir() / "compile_commands.json")) {
    throw std::runtime_error { std::format("file not found: {}/compile_commands.json", cfg.get_build_dir().string()) };
  }

  if (cfg.is_clang_tidy_enabled()) {
    if (!std::filesystem::exists(utils::get_root_dir() / ".clang-tidy")) {
      throw std::runtime_error { std::format("file not found: {}/.clang-tidy", utils::get_root_dir().string()) };
    }

    std::string_view ignore {};
    switch (cfg.get_compiler()) {
      case cfg::Compiler::Clang:
      case cfg::Compiler::ClangPp:
      case cfg::Compiler::AppleClang:
      case cfg::Compiler::AppleClangPp:
      case cfg::Compiler::HomebrewClang:
      case cfg::Compiler::HomebrewClangPp:
        // Some flags may be used in other compilers that clang does not
        // recognize. This flag ignores any unknown flags.
        ignore = " -extra-arg=-Wno-unknown-warning-option";
        break;
      default:
        break;
    }

    std::string clang_tidy_args {};
    if (cfg.clang_tidy_args.has_value()) {
      clang_tidy_args = " " + cfg.clang_tidy_args.value();
    }

    // Run clang-tidy
    constexpr std::string_view cmd_str {
      "run-clang-tidy "
      "-p {0}"
      "{1}"
      "{2}"
    };
    const std::string cmd {
      std::format(cmd_str,
                  cfg.get_build_dir().string(), // {0}
                  ignore,                       // {2}
                  clang_tidy_args)              // {3}
    };

    if (verbose) {
      utils::gojo_print(utils::LogLevel::Status, "Running clang-tidy...\n");
    }
    auto cmd_result { utils::execute_command(cmd.c_str(), !verbose) };
    if (!cmd_result.success) {
      utils::gojo_print(utils::LogLevel::Info, "{}", cmd_result.output);
      throw std::runtime_error { "failed clang-tidy checks" };
    }
    if (verbose) {
      utils::gojo_print(utils::LogLevel::Success, "No problems found!");
    }
  }

  if (cfg.is_cppcheck_enabled()) {
    std::string language_standard_arg { utils::to_lower(cfg::to_str(cfg.language)) };
    
    switch (cfg.get_language_standard()) {
      case cfg::LanguageStandard::C99:
        language_standard_arg.append("99");
        break;
      case cfg::LanguageStandard::C11:
      case cfg::LanguageStandard::C17:
      case cfg::LanguageStandard::C23:
        language_standard_arg.append("11");
        break;
      case cfg::LanguageStandard::Cpp11:
      case cfg::LanguageStandard::Cpp14:
      case cfg::LanguageStandard::Cpp17:
        language_standard_arg.append(cfg::to_str(cfg.get_language_standard()));
        break;
      case cfg::LanguageStandard::Cpp20:
      case cfg::LanguageStandard::Cpp23:
      case cfg::LanguageStandard::Cpp26:
        language_standard_arg.append("20");
        break;
    }

    std::string_view cppcheck_checks {};
    std::string cppcheck_args {};
    if (cfg.cppcheck_checks.has_value()) {
      cppcheck_checks = cfg.cppcheck_checks.value();
    }
    if (cfg.cppcheck_args.has_value()) {
      cppcheck_args = " " + cfg.cppcheck_args.value();
    }

    // Run cppcheck
    constexpr std::string_view cmd_str {
      "cppcheck "
      "--project={0}/compile_commands.json "
      "--std={1} "
      "--enable={2}"
      "{3}"
    };
    const std::string cmd {
      std::format(cmd_str,
                  cfg.get_build_dir().string(),  // {0}
                  language_standard_arg,         // {1}
                  cppcheck_checks,               // {2}
                  cppcheck_args)                 // {3}
    };
    if (verbose) {
      utils::gojo_print(utils::LogLevel::Status, "Running cppcheck...\n");
    }
    auto cmd_result { utils::execute_command(cmd.c_str(), !verbose) };
    if (!cmd_result.success) {
      utils::gojo_print(utils::LogLevel::Info, "{}", cmd_result.output);
      throw std::runtime_error { "failed cppcheck checks" };
    }
    if (verbose) {
      utils::gojo_print(utils::LogLevel::Success, "No problems found!");
    }
  }
}

}
