/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
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
    gojo {2}fmt{0} [options]

{1}Summary:{0}
    Auto format the project source code according to the style chosen. The
    format style can be set in the gojo.toml config file with the field
    checks.clang_format_style. The style can be any of the styles provided
    by clang-format, the gojo style that can be found in ~/.gojo/formats/gojo,
    or a custom style defined in a .clang-format file at the root of the
    project. If no style is set, this command has no effect.
    More information on clang-format can be found here:
    {3}https://clang.llvm.org/docs/ClangFormat.html{0}

    Custom formats can be created and saved to ~/.gojo/formats, as well as used
    in other projects.

    The following is a list of all possible values for the field
    checks.clang_format_style:

        - custom
        - gojo
        - google
        - llvm
        - chromium
        - mozilla
        - webkit
        - microsoft
        - gnu

    If the field is set to 'custom', there must be a .clang-format file in the
    project root.

{1}Options:{0}
    --use=<format>      Use the format defined in ~/.gojo/formats/<format>.

    --save-as=<name>    Save the format defined in the project root
                        .clang-format file to ~/.gojo/formats/<name>.
)"
};

const std::filesystem::path& get_formats_dir() {
    static const std::filesystem::path dir {
        std::filesystem::absolute(utils::get_home_dir() / ".gojo" / "formats")
    };
    return dir;
}

void copy_format_file(const std::filesystem::path& dest,
                      const std::filesystem::path& src) {
    if (!std::filesystem::exists(src)) {
        throw std::runtime_error { std::format(
            "gojo fmt --from: no format file named {}", src.filename().string()
        ) };
    }

    const std::ifstream src_file(src);
    if (!src_file.is_open()) {
        throw std::runtime_error { std::format("failed to open file: {}",
                                               src.string()) };
    }

    std::filesystem::create_directories(dest.parent_path());
    std::ofstream dest_file(dest);
    if (!dest_file.is_open()) {
        throw std::runtime_error { std::format("failed to open file: {}",
                                               dest.string()) };
    }

    dest_file << utils::get_file_contents(get_formats_dir());
}

}  // namespace

namespace cmd {

void fmt(std::span<std::string_view> args) {
    for (const auto arg : args) {
        if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        }
        else if (arg.starts_with("--use=")) {
            constexpr std::size_t arg_len { 6 };
            copy_format_file(utils::get_root_dir() / ".clang-format",
                             get_formats_dir() / arg.substr(arg_len));
            break;
        }
        else if (arg.starts_with("--save-as=")) {
            constexpr std::size_t arg_len { 10 };
            copy_format_file(get_formats_dir() / arg.substr(arg_len),
                             utils::get_root_dir() / ".clang-format");
            return;
        }
        else {
            throw std::runtime_error {
                std::format("gojo fmt: argument not recognized: {}", arg)
            };
        }
    }

    cfg::Config cfg { cfg::Config::this_project() };
    std::string style {};
    if (cfg.clang_format_style.has_value()) {
        if (cfg.clang_format_style.value() == cfg::FormatStyle::Custom) {
            style = " -style=file";
        }
        else if (cfg.clang_format_style.value() == cfg::FormatStyle::Gojo) {
            style = std::format(
                " -style=file:{}-{}",
                (utils::get_home_dir() / ".gojo" / "formats" / "gojo").string(),
                utils::to_lower(cfg::to_str(cfg.language))
            );
        }
        else {
            style = " -style=";
            style.append(cfg::to_str(cfg.clang_format_style.value()));
        }
    }

    constexpr std::string_view fmt_str {
        "find {} -type f "
        "-name '*.{}' -o "
        "-name '*.{}' "
        "-exec clang-format{} -i {{}} +"
    };
    const std::string cmd { std::format(fmt_str, utils::get_root_dir().string(),
                                        cfg.get_header_extension(),
                                        cfg.get_source_extension(), style) };

    const auto result { utils::execute_command(
        cmd.c_str(), false, true, cfg.get_build_dir() / "log.txt"
    ) };
    if (!result.success) {
        throw std::runtime_error { "failed to format" };
    }
}

}  // namespace cmd
