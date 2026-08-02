#include <cstdio>
#include <cstdlib>
#include <optional>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "src/commands.hpp"
#include "src/literals.hpp"
#include "src/utils.hpp"


// TODO:
// * Switch std::optional<std::string> to an enum mapping system.
//   - Create an enum GojoError with states for every possible error
//   - Each function returns an optional<GojoError> or expeceted<T, GojoError>
//   - Create a function/mapping of GojoError -> error string.
//   - Have one globally accessible string to capture the error message if needed.
//
// * Change GojoConfig fields to be char arrays for those that can
//
// * Change GojoConfig fields to be enums where possible
//
// * Change utils::execute_command to take in a const char* or std::string_view
//
// * Reduce levels of std::string where possible

constexpr std::string_view VERSION_STRING { "gojo version 1.0.0-beta" };

int main(const int argc, const char* argv[]) {
  if (argc == 1) {
    std::println(literals::WIN,
                 utils::CLEAR,
                 utils::MAGENTA_BOLD,
                 utils::MAGENTA);
    commands::help();
    return EXIT_SUCCESS;
  }

  // clang-tidy doesn't like pointer arithmetic, but this is the only
  // way to construct the vector. Also, I don't think pointer arithmetic
  // is that bad...
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::vector<std::string_view> args_vec(argv, argv + argc);
  std::span<std::string_view> args { args_vec };

  // Remove gojo <command> from argument list.
  args = args.subspan(2);

  const std::string_view command { args_vec.at(1) };
  std::optional<std::string> result { std::nullopt };

  if (command == "init") {
    result = commands::init(args);
  }
  else if (command == "build") {
    result = commands::build(args);
  }
  else if (command == "run") {
    result = commands::run(args);
  }
  else if (command == "test") {
    result = commands::test(args);
  }
  else if (command == "clean") {
    commands::clean(args);
  }
  else if (command == "fmt") {
    result = commands::fmt(args);
  }
  else if (command == "check") {
    result = commands::check(args);
  }
  else if (command == "template") {
    result = commands::file_template(args);
  }
  else if (command == "refresh") {
    result = commands::refresh(args, std::nullopt);
  }
  else if (command == "dep") {
    // TODO: Implement
    // gojo dep search <pkg>
    // gojo dep install <pkg> [version]
    // gojo dep add <pkg> [version]
    // gojo dep remove <pkg>
    // gojo dep uninstall <pkg>
    result = commands::dep(args);
  }
  /*
  else if (command == "publish") {
    // TODO: Implement
    // gojo publish
  }
  else if (command == "install") {
    // TODO: refactor
    // gojo install . [--replace|-r]
    result = commands::install(args);
  }
  */
  else if (command == "profile") {
    result = commands::profile(args);
  }
  else if (command == "info") {
    commands::info();
  }
  else if (command == "help") {
    commands::help();
  }
  else if (command == "--version") {
    std::println(VERSION_STRING);
  }
  else {
    std::println(stderr,
                 RED("[Error]: command not recognized: 'gojo {}'"),
                 command);
  }

  if (result.has_value()) {
    std::println(stderr, RED("[Error]: {}"), result.value());
    std::println(stderr, YELLOW("see gojo {} --help for more info"), command);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
