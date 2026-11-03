/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg//enums.hpp"
#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
    gojo {2}check{0} [options]

{1}Summary:{0}
    Check the source code with static analysis tools. The static analysis
    tools used are:

        - clang-tidy
        - cppcheck
    
    These tools can be enabled, disabled, and tuned to custom workflows
    with several settings in the gojo.toml config file. For clang-tidy,
    the field checks.clang_tidy_enabled decides if the tool is used at all.
    Arguments can be passed to clang-tidy with the field
    checks.clang_tidy_args, which is an array of strings. More about clang-tidy
    can be found here: {3}https://clang.llvm.org/extra/clang-tidy/{0}

    cppcheck can be enabled with the field checks.cppcheck_enabled. The field
    checks.cppcheck_checks is an array of strings where each entry is a "check"
    that is enabled in cppcheck. checks.cppcheck_args is an array where each
    entry is passed to cppcheck as an argument. To learn more about cppcheck
    checks and arguments, go here:
    {3}https://cppcheck.sourceforge.io/manual.html{0}

    By default, no static analysis checks are enabled. This comand has no
    effect if no checks are enabled.

{1}Options:{0}
    --verbose, -v       Allow the output from the static analyzers to be
                        forwarded to standard output.
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
            throw std::runtime_error {
                std::format("gojo check: argument not recognized: {}", arg)
            };
        }
    }

    const cfg::Config cfg { cfg::Config::this_project() };

    if (!verbose) {
        verbose = cfg.is_verbose_enabled();
    }

    if (!cfg.is_clang_tidy_enabled() && !cfg.is_cppcheck_enabled()) {
        utils::gojo_print(utils::LogLevel::Warning, "no checks enabled");
        return;
    }

    if (!std::filesystem::exists(cfg.get_build_dir() /
                                 "compile_commands.json")) {
        throw std::runtime_error {
            std::format("file not found: {}/compile_commands.json",
                        cfg.get_build_dir().string())
        };
    }

    if (cfg.is_clang_tidy_enabled()) {
        if (!std::filesystem::exists(utils::get_root_dir() / ".clang-tidy")) {
            throw std::runtime_error { std::format(
                "file not found: {}/.clang-tidy", utils::get_root_dir().string()
            ) };
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
            default: break;
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
                        cfg.get_build_dir().string(),  // {0}
                        ignore,                        // {2}
                        clang_tidy_args)               // {3}
        };

        if (verbose) {
            utils::gojo_print(utils::LogLevel::Status,
                              "Running clang-tidy...\n");
            utils::gojo_print(utils::LogLevel::Info, "Command: {}", cmd);
        }
        const auto cmd_result { utils::execute_command(
            cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt"
        ) };
        if (!cmd_result.success) {
            throw std::runtime_error { "failed clang-tidy checks" };
        }
        if (verbose) {
            utils::gojo_print(utils::LogLevel::Success, "\nNo problems found!");
        }
    }

    if (cfg.is_cppcheck_enabled()) {
        std::string language_standard_arg {
            utils::to_lower(cfg::to_str(cfg.language))
        };

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
                language_standard_arg.append(
                    cfg::to_str(cfg.get_language_standard())
                );
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
            utils::gojo_print(utils::LogLevel::Info, "Command: {}", cmd);
        }
        const auto cmd_result { utils::execute_command(
            cmd.c_str(), false, verbose, cfg.get_build_dir() / "log.txt"
        ) };
        if (!cmd_result.success) {
            throw std::runtime_error { "failed cppcheck checks" };
        }
        if (verbose) {
            utils::gojo_print(utils::LogLevel::Success, "\nNo problems found!");
        }
    }
}

}  // namespace cmd
