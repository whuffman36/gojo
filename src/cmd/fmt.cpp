#include "src/cmd/commands.hpp"

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
  gojo {2}fmt{0} {4}[options]{0}

{1}Summary:{0}
  Format the project using clang-format. By default, this command uses the
  .clang-format file found in the root of the project created by 'gojo init'.
  If no file is found and no options are supplied, this command has no effect.
  Any options supplied to this command are forwarded directly to clang-format
  and override any settings in the .clang-format file. By default, code is
  auto formatted and changed in-place according the rules laid out in the
  .clang-format file. More information on clang-format can be found here:
  {4}https://clang.llvm.org/docs/ClangFormat.html{0}

{1}Options:{0}
  Any options supplied to this command are forwarded to clang-format and
  override any rules set in the .clang-format file, if it exists.
)"
};

}


namespace cmd {

void fmt(std::span<std::string_view> args) {
  std::string fmt_args { " -i" };

  for (const auto arg : args) {
    if (arg == "--help") {
      utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
      return;
    }
    else {
      fmt_args = args_to_string(args);
      fmt_args.insert(fmt_args.begin(), ' ');
      break;
    }
  }

  cfg::Config cfg { cfg::Config::this_project() };
  std::string style {};
  if (cfg.clang_format_style.has_value()) {
    if (cfg.clang_format_style.value() == cfg::FormatStyle::Custom) {
      style = " style=file";
    }
    else {
      style = " style=";
      style.append(cfg::to_str(cfg.clang_format_style.value()));
    }
  }

  constexpr std::string_view fmt_str {
    "find {} -type f "
    "-name '*.{}' -o "
    "-name '*.{}' "
    "-exec clang-format{}{} {{}} +"
  };
  const std::string cmd {
    std::format(fmt_str,
                utils::get_root_dir().string(),
                cfg.get_header_extension(),
                cfg.get_source_extension(),
                style,
                fmt_args)
  };

  const auto result { utils::execute_command(cmd.c_str(), false, true, cfg.get_build_dir() / "log.txt") };
  if (!result.success) {
    throw std::runtime_error { "failed to format" };
  }
}

}
