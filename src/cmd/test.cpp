#include "src/cmd/commands.hpp"

#include <cstddef>
#include <filesystem>
#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view HELP {
R"({1}Command:{0}
  gojo {2}test{0} {4}[options]{0}

{1}Summary:{0}
  Run unit tests in the project. Tests must be enabled in the .gojo config
  file and a call to 'gojo build' must be successfully completed before
  running this command.

{1}Options:{0}
  {4}--target=<target>{0} Test only a specific target intead of all unit tests.
)"
};

}


namespace cmd {

void test(std::span<std::string_view> args) {
  std::string target {};
  for (const auto arg : args) {
    if (arg.starts_with("--target=")) {
      // Length of "--target="
      constexpr std::size_t target_arg_len { 9 };
      target = std::format(" -R \"{}\"", arg.substr(target_arg_len));
    }
    else if (arg == "--help") {
      utils::print_info(HELP);
      return;
    }
    else {
      throw std::runtime_error { std::format("gojo test: argument not recognized: {}", arg) };
    }
  }

  const cfg::Config cfg { cfg::Config::this_project() };
  if (!cfg.is_testing_enabled()) {
    return;
  }

  std::filesystem::path build { utils::get_root_dir() };
  build /= "build";
  std::string_view build_type { cfg::to_string(cfg.get_build_type()) };
  std::string build_type_arg {};

  if (cfg.is_multi_config()) {
    build /= "test";
    build_type_arg = std::format(" -C {}", build_type);
  }
  else {
    build /= build_type;
    build /= "test";
  }

  if (!std::filesystem::exists(build)) {
    throw std::runtime_error { std::format("directory not found: {}", build.string()) };
  }

  std::filesystem::current_path(build);
  const std::string cmd {
    std::format("ctest --output-on-failure{}{}", build_type_arg, target)
  };
  auto cmd_result { utils::execute_command(cmd.c_str()) };
  if (!cmd_result.success) {
    throw std::runtime_error { "failed to run tests" };
  }
}

}