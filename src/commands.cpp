#include "src/commands.hpp"

#include <cstdlib>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include "src/cmake.hpp"
#include "src/conan.hpp"
#include "src/literals.hpp"
#include "src/config.hpp"
#include "src/templates.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view CONFIG_FILE { ".gojo" };

struct FileBatch {
  // Root.
  std::ofstream cmakelists_root;
  std::ofstream clangd;
  std::ofstream clang_format;
  std::ofstream clang_tidy;
  std::ofstream conanfile;
  std::ofstream gitignore;

  // src
  std::ofstream cmakelists_src;
  std::ofstream main;

  // lib
  std::ofstream lib;

  // test
  std::ofstream cmakelists_test;
  std::ofstream test;
};


std::expected<std::ofstream, std::string>
create_file(const std::filesystem::path& file) {
  std::ofstream filestream { file };
  if (!filestream.is_open()) {
    return std::unexpected {
      std::format("failed to open file: {}", file.string())
    };
  }

  return filestream;
}


std::expected<FileBatch, std::string>
create_files(const config::GojoConfig& cfg) {
  FileBatch files {};
  std::string name_lower {
    cfg.is_library ?
    utils::to_lower(cfg.project_name) :
    utils::to_lower(cfg.executable_name)
  };

  // Create project root directory if it does not exist.
  const std::filesystem::path root { cfg.project_root };
  if (!std::filesystem::exists(root)) {
    if (!std::filesystem::create_directories(root)) {
      return std::unexpected {
        std::format("failed to create directory: {}", root.string())
      };
    }
  }

  // TODO: Break up CMake file creation into its own block
  // TODO: Implement Meson file creation
  // CMakeLists.txt
  auto result { create_file(root / "CMakeLists.txt") };
  if (!result.has_value()) {
    return std::unexpected { std::move(result.error()) };
  }
  files.cmakelists_root = std::move(result.value());

  // .clang-format
  result = create_file(root / ".clang-format");
  if (!result.has_value()) {
    return std::unexpected { std::move(result.error()) };
  }
  files.clang_format = std::move(result.value());

  if (cfg.clangd_enabled) {
    result = create_file(root / ".clangd");
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.clangd = std::move(result.value());
  }


  // .clang-tidy
  if (cfg.clang_tidy_enabled) {
    result = create_file(root / ".clang-tidy");
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.clang_tidy = std::move(result.value());
  }

  // conanfile
  if (cfg.pkg_manager == "conan") {
    result = create_file(root / "conanfile.py");
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.conanfile = std::move(result.value());
  }

  if (cfg.pkg_manager == "vcpkg") {
    // TODO: Implement vcpkg
  }

  // .gitignore
  if (cfg.git_repo) {
    result = create_file(root / ".gitignore");
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.gitignore = std::move(result.value());
  }

  // Create src directory
  if (!std::filesystem::create_directory(root / "src")) {
    return std::unexpected { std::format("failed to create directory: {}/src", root.string()) };
  }

  // src/CMakeLists.txt
  result = create_file(root / "src" / "CMakeLists.txt");
  if (!result.has_value()) {
    return std::unexpected { std::move(result.error()) };
  }
  files.cmakelists_src = std::move(result.value());

  // src/main.cpp
  result = create_file(root / "src" / std::format("{}.{}", name_lower, cfg.src_ext));
  if (!result.has_value()) {
    return std::unexpected { std::move(result.error()) };
  }
  files.main = std::move(result.value());

  if (cfg.is_library) {
    // Create include/lib directory
    if (!std::filesystem::create_directories(root / "include" / name_lower)) {
      return std::unexpected { std::format("failed to create directory: {}/include/{}", root.string(), name_lower) };
    }

    // include/lib/lib.hpp
    result = create_file(root / "include" / name_lower / std::format("{}.{}", name_lower, cfg.hdr_ext));
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.lib = std::move(result.value());
  }

  if (cfg.testing_enabled) {
    // Create test directory
    if (!std::filesystem::create_directories(root / "test")) {
      return std::unexpected { std::format("failed to create directory: {}/test", root.string()) };
    }

    // test/CMakeLists.txt
    result = create_file(root / "test" / "CMakeLists.txt");
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.cmakelists_test = std::move(result.value());

    // test/main_test.cpp
    result = create_file(root / "test" / std::format("{}_test.{}", name_lower, cfg.src_ext));
    if (!result.has_value()) {
      return std::unexpected { std::move(result.error()) };
    }
    files.test = std::move(result.value());
  }

  return files;
}


std::optional<std::string> write_to_files(const config::GojoConfig& cfg) {
  auto result { create_files(cfg) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }

  FileBatch files { std::move(result.value()) };

  // root
  files.cmakelists_root << templates::cmakelists_root(cfg) << std::flush;

  if (cfg.fmt_style != "custom") {
    std::filesystem::current_path(cfg.project_root);
    const std::string cmd {
      std::format("clang-format -style={} -dump-config", cfg.fmt_style)
    };

    auto dump_result { utils::execute_command(cmd, true) };
    if (!dump_result.success) {
      return std::make_optional(std::move(dump_result.output));
    }
    files.clang_format << dump_result.output << std::flush;
  }

  if (cfg.clangd_enabled) {
    files.clangd << templates::clangd(cfg) << std::flush;
  }

  if (cfg.clang_tidy_enabled) {
    files.clang_tidy << templates::clangtidy(cfg) << std::flush;
  }

  if (cfg.pkg_manager == "conan") {
    files.conanfile << templates::conanfile(cfg) << std::flush;
  }

  if (cfg.git_repo) {
    files.gitignore << literals::GITIGNORE << std::flush;
  }

  // src
  files.cmakelists_src << templates::cmakelists_src(cfg) << std::flush;
  files.main << templates::source_file(cfg) << std::flush;

  // lib
  if (cfg.is_library) {
    files.lib << templates::header_file(cfg) << std::flush;
  }

  // test
  if (cfg.testing_enabled) {
    files.cmakelists_test << templates::cmakelists_test(cfg) << std::flush;
    files.test << templates::test_file(cfg) << std::flush;
  }

  return std::nullopt;
}


std::string args_to_string(std::span<std::string_view> args) {
  std::string str {};
  for (const auto arg : args) {
    str.append(arg);
    str.push_back(' ');
  }
  return str;
}


config::GojoConfig init_interactive() {
  config::GojoConfig cfg { config::DEFAULT };
  int num_choice {};
  char char_choice {};
  bool success { false };

  std::print(MAGENTA("Enter the project name: "));
  // Read in any extra whitespace the user may have entered.
  std::getline(std::cin >> std::ws, cfg.project_name);

  std::print(
    MAGENTA("\nEnter the relative path where the project should be created: ")
  );
  std::getline(std::cin >> std::ws, cfg.project_root);

  if (cfg.project_root == ".") {
    cfg.project_root = std::filesystem::current_path().string();
  } else {
    cfg.project_root = std::format("{}/{}",
                                   std::filesystem::current_path().string(),
                                   cfg.project_root);
  }

  // clang-tidy flags the language standards and the case numbers as
  // magic numbers.
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  while (!success) {
    std::print(MAGENTA("\nEnter the project target:")
              "\n\n\t1. " CYAN("binary")
                "\n\t2. " CYAN("library")"\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        cfg.is_library = false;
        success = true;
        break;
      case 2:
        cfg.is_library = true;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;

  if (!cfg.is_library) {
    std::print(MAGENTA("Enter the main executable name: "));
    // Read in any extra whitespace the user may have entered.
    std::getline(std::cin >> std::ws, cfg.executable_name);
  }

  while (!success) {
    std::print(
      MAGENTA("\nEnter the language you want to use for your project:")
              "\n\n\t1. " CYAN("C++")
                "\n\t2. " CYAN("C")"\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        cfg.project_lang = "C++";
        success = true;
        break;
      case 2:
        cfg.project_lang = "C";
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;

  if (cfg.project_lang == "C++") {
    while (!success) {
      std::print(MAGENTA("\nEnter the C++ standard you want to use:")
                "\n\n\t1. " CYAN("C++23")
                  "\n\t2. " CYAN("C++20")
                  "\n\t3. " CYAN("C++17")
                  "\n\t4. " CYAN("C++14")
                  "\n\t5. " CYAN("C++11")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.lang_standard = 23;
          success = true;
          break;
        case 2:
          cfg.lang_standard = 20;
          success = true;
          break;
        case 3:
          cfg.lang_standard = 17;
          success = true;
          break;
        case 4:
          cfg.lang_standard = 14;
          success = true;
          break;
        case 5:
          cfg.lang_standard = 11;
          success = true;
          break;
        default:
          std::println(RED("\noption not recognized: {}"), num_choice);
          break;
      }
    }
    success = false;

    while (!success) {
      std::print(MAGENTA("\nEnter the C++ compiler you want to use:")
                "\n\n\t1. " CYAN("system default")
                  "\n\t2. " CYAN("clang++")
                  "\n\t3. " CYAN("g++")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.compiler = "system default";
          success = true;
          break;
        case 2:
          cfg.compiler = "clang++";
          success = true;
          break;
        case 3:
          cfg.compiler = "g++";
          success = true;
          break;
        default:
          std::print("\noption not recognized: {}\n", num_choice);
          break;
      }
    }
    success = false;
  }
  else {
    while (!success) {
      std::print(MAGENTA("\nEnter the C standard you want to use:")
                "\n\n\t1. " CYAN("C23")
                  "\n\t2. " CYAN("C17")
                  "\n\t3. " CYAN("C11")
                  "\n\t4. " CYAN("C99")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.lang_standard = 23;
          success = true;
          break;
        case 2:
          cfg.lang_standard = 17;
          success = true;
          break;
        case 3:
          cfg.lang_standard = 11;
          success = true;
          break;
        case 4:
          cfg.lang_standard = 99;
          success = true;
          break;
        default:
          std::println(RED("\noption not recognized: {}"), num_choice);
          break;
      }
    }
    success = false;

    while (!success) {
      std::print(MAGENTA("\nEnter the C compiler you want to use:")
                "\n\n\t1. " CYAN("system default")
                  "\n\t2. " CYAN("clang")
                  "\n\t3. " CYAN("gcc")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.compiler = "system default";
          success = true;
          break;
        case 2:
          cfg.compiler = "clang";
          success = true;
          break;
        case 3:
          cfg.compiler = "gcc";
          success = true;
          break;
        default:
          std::print("\noption not recognized: {}\n", num_choice);
          break;
      }
    }
    success = false;
  } // if lang == C++

  while (!success) {
    std::print(MAGENTA("\nDo you want to use GNU extensions? ")
               CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.gnu_extensions = true;
        success = true;
        break;
      case 'n':
        cfg.gnu_extensions = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;


  std::print(MAGENTA("\nEnter flags to be passed to the compiler "
                     "(these can be edited later): "));
  std::getline(std::cin >> std::ws, cfg.compiler_flags);

  // TODO: Implement Meson
  /*
  while (!success) {
    std::print("\nEnter the build system you want to use:\n\n\t1. CMake\n\t2. Meson\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        settings.build_system = "CMake";
        success = true;
        break;
      case 2:
        settings.build_system = "Meson";
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;
  */

  while (!success) {
    std::print(MAGENTA("\nEnter the CMake generator system you want to use:")
              "\n\n\t1. " CYAN("system default")
                "\n\t2. " CYAN("Ninja")
                "\n\t3. " CYAN("Ninja Multi-Config")"\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        cfg.generator = "system default";
        success = true;
        break;
      case 2:
        cfg.generator = "Ninja";
        success = true;
        break;
      case 3:
        cfg.generator = "Ninja Multi-Config";
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;

  while (!success) {
    std::print(MAGENTA("\nEnter the package manager you want to use:")
              "\n\n\t1. " CYAN("none")
                "\n\t2. " CYAN("conan")
                "\n\t3. " CYAN("vcpkg")"\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        cfg.pkg_manager = "none";
        success = true;
        break;
      case 2:
        cfg.pkg_manager = "conan";
        success = true;
        break;
      case 3:
        cfg.pkg_manager = "vcpkg";
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;

  if (cfg.project_lang == "C++") {
    while (!success) {
      std::print(MAGENTA("\nEnter the source file extension you want to use:")
                "\n\n\t1. " CYAN("cpp")
                  "\n\t2. " CYAN("cc")
                  "\n\t3. " CYAN("cxx")
                  "\n\t4. " CYAN("c++")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.src_ext = "cpp";
          success = true;
          break;
        case 2:
          cfg.src_ext = "cc";
          success = true;
          break;
        case 3:
          cfg.src_ext = "cxx";
          success = true;
          break;
        case 4:
          cfg.src_ext = "c++";
          success = true;
          break;
        default:
          std::println(RED("\noption not recognized: {}"), num_choice);
          break;
      }
    }
    success = false;

    while (!success) {
      std::print(MAGENTA("\nEnter the header file extension you want to use:")
                "\n\n\t1. " CYAN("hpp")
                  "\n\t2. " CYAN("h")
                  "\n\t3. " CYAN("hxx")
                  "\n\t4. " CYAN("h++")"\n\n\t");
      std::cin >> num_choice;
      switch (num_choice) {
        case 1:
          cfg.hdr_ext = "hpp";
          success = true;
          break;
        case 2:
          cfg.hdr_ext = "h";
          success = true;
          break;
        case 3:
          cfg.hdr_ext = "hxx";
          success = true;
          break;
        case 4:
          cfg.hdr_ext = "h++";
          success = true;
          break;
        default:
          std::println(RED("\noption not recognized: {}"), num_choice);
          break;
      }
    }
    success = false;
  }  // if
  else if (cfg.project_lang == "C") {
    cfg.src_ext = "c";
    cfg.hdr_ext = "h";
  }

  while (!success) {
    std::print(MAGENTA("\nEnter the clang format style you want to use:")
              "\n\n\t1. " CYAN("Google")
                "\n\t2. " CYAN("LLVM")
                "\n\t3. " CYAN("Chromium")
                "\n\t4. " CYAN("Mozilla")
                "\n\t5. " CYAN("WebKit")
                "\n\t6. " CYAN("Microsoft")
                "\n\t7. " CYAN("GNU")
                "\n\t8. " CYAN("Custom")"\n\n\t");
    std::cin >> num_choice;
    switch (num_choice) {
      case 1:
        cfg.fmt_style = "google";
        success = true;
        break;
      case 2:
        cfg.fmt_style = "llvm";
        success = true;
        break;
      case 3:
        cfg.fmt_style = "chromium";
        success = true;
        break;
      case 4:
        cfg.fmt_style = "mozilla";
        success = true;
        break;
      case 5:
        cfg.fmt_style = "webkit";
        success = true;
        break;
      case 6:
        cfg.fmt_style = "microsoft";
        success = true;
        break;
      case 7:
        cfg.fmt_style = "gnu";
        success = true;
        break;
      case 8:
        cfg.fmt_style = "custom";
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), num_choice);
        break;
    }
  }
  success = false;

  while (!success) {
    std::print(MAGENTA("\nDo you want to use clangd? ") CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.clangd_enabled = true;
        success = true;
        break;
      case 'n':
        cfg.clangd_enabled = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;

  while (!success) {
    std::print(MAGENTA("\nDo you want to use clang-tidy? ") CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.clang_tidy_enabled = true;
        success = true;
        break;
      case 'n':
        cfg.clang_tidy_enabled = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;

  while (!success) {
    std::print(MAGENTA("\nDo you want to use cppcheck? ") CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.cppcheck_enabled = true;
        success = true;
        break;
      case 'n':
        cfg.cppcheck_enabled = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;

  while (!success) {
    std::print(MAGENTA("\nDo you want to enable tests? ") CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.testing_enabled = true;
        success = true;
        break;
      case 'n':
        cfg.testing_enabled = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;

  if (cfg.testing_enabled) {
    if (cfg.project_lang == "C++") {
      while (!success) {
        std::print(MAGENTA("\nDo you want to use GoogleTest for unit tests? "
                           "(must have a package manager enabled) ") 
                           CYAN("[y/n] "));
        std::cin >> char_choice;
        switch (char_choice) {
          case 'y':
            cfg.testing_framework = "GoogleTest";
            success = true;
            break;
          case 'n':
            cfg.testing_framework = "none";
            success = true;
            break;
          default:
            std::println(RED("\noption not recognized: {}"), char_choice);
            break;
        }
      }
      success = false;
    }
    else if (cfg.project_lang == "C") {
      while (!success) {
        std::print(MAGENTA("\nDo you want to use Unity for unit tests? "
                           "(must have a package manager enabled) ") 
                           CYAN("[y/n] "));
        std::cin >> char_choice;
        switch (char_choice) {
          case 'y':
            cfg.testing_framework = "Unity";
            success = true;
            break;
          case 'n':
            cfg.testing_framework = "none";
            success = true;
            break;
          default:
            std::println(RED("\noption not recognized: {}"), char_choice);
            break;
        }
      }
      success = false;
    }  // if
  }  // if

  while (!success) {
    std::print(MAGENTA("\nDo you want to create a git repo? ") CYAN("[y/n] "));
    std::cin >> char_choice;
    switch (char_choice) {
      case 'y':
        cfg.git_repo = true;
        success = true;
        break;
      case 'n':
        cfg.git_repo = false;
        success = true;
        break;
      default:
        std::println(RED("\noption not recognized: {}"), char_choice);
        break;
    }
  }
  success = false;
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

  std::println();
  return cfg;
}


void clean_build_dir(const config::GojoConfig& cfg) {
  const std::filesystem::path build { cfg.project_root + "/build" };
  std::filesystem::remove_all(build);
  std::filesystem::create_directory(build);
}

}  // namespace


namespace commands {

std::optional<std::string> init(std::span<std::string_view> args) {
  if (args.empty()) {
    return std::make_optional("gojo init: missing name argument");
  }

  config::GojoConfig cfg { config::DEFAULT };

  for (const auto arg : args) {
    if (arg == "-f" || arg == "--file") {
      auto result {
        config::read_from_file(std::filesystem::current_path() / CONFIG_FILE)
      };
      if (!result.has_value()) {
        return std::make_optional(std::move(result.error()));
      }
      cfg = std::move(result.value());
      break;
    }
    else if (arg == "-i") {
      cfg = init_interactive();
      break;
    }
    else if (arg == "--lib") {
      cfg.is_library = true;
      cfg.executable_name = "none";
    }
    else if (arg.starts_with("--lang=")) {
      if (arg.ends_with("c")) {
        cfg.project_lang = "C";
        cfg.src_ext = "c";
        cfg.hdr_ext = "h";

        if (cfg.compiler == "clang++") {
          cfg.compiler = "clang";
        }
        if (cfg.compiler == "g++") {
          cfg.compiler = "gcc";
        }
      }
      else if (arg.ends_with("c++")) {
        cfg.project_lang = "C++";
        cfg.src_ext = "cpp";
        cfg.hdr_ext = "hpp";
      }
      else {
        constexpr std::string_view fmt_str {
          "gojo init --lang: argument not recognized: {}\n"
          "\targument must be 'c' or 'c++'"
        };
        
        // Length of "--lang=".
        constexpr std::size_t lang_arg_len { 7 };
        return std::make_optional(
          std::format(fmt_str, arg.substr(lang_arg_len))
        );
      }
    }
    else if (arg.starts_with("--profile=")) {
      // length of "--profile=".
      constexpr std::size_t profile_arg_len { 10 };
      std::string_view profile_name { arg.substr(profile_arg_len) };
      auto result { config::read_profile(profile_name) };
      if (!result.has_value()) {
        if (result.error().starts_with("file not found")) {
          return std::make_optional(
            std::format("gojo init: no profile with name '{}'", profile_name)
          );
        }
        return std::make_optional(std::move(result.error()));
      }

      cfg = std::move(result.value());
    }
    else if (arg == "--help") {
      std::println(literals::INIT_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else if (arg.starts_with("--") || arg.starts_with("-")) {
      return std::make_optional(
        std::format("gojo init: option not recognized: {}", arg)
      );
    }
    else {
      cfg.project_name = arg;
      if (!cfg.is_library) {
        cfg.executable_name = arg;
      }
    }
  }

  // The default config leaves the root as ".". There shouldn't be a case
  // where the root is empty, but just in case.
  if (cfg.project_root == "." || cfg.project_root.empty()) {
    cfg.project_root = std::filesystem::current_path().string();
  }
  cfg.multi_config = config::is_multi_config(cfg.generator);

  auto file_result { write_to_files(cfg) };
  if (file_result.has_value()) {
    return file_result;
  }

  // init conan
  if (cfg.pkg_manager == "conan") {
    auto conan_result { conan::install(cfg, false) };
    if (conan_result.has_value()) {
      return conan_result;
    }
  }
  if (cfg.pkg_manager == "vcpkg") {
    // TODO: Implement vcpkg.
  }

  // init cmake
  if (cfg.build_system == "CMake") {
    auto cmake_result { cmake::configure(cfg, false) };
    if (cmake_result.has_value()) {
      return cmake_result;
    }
  }
  if (cfg.build_system == "Meson") {
    // TODO: Implement Meson.
  }

  if (cfg.git_repo) {
    std::filesystem::current_path(cfg.project_root);
    const std::string cmd { "git init" };
    auto git_result { utils::execute_command(cmd) };
    if (!git_result.success) {
      return std::make_optional("failed to initialize git repository");
    }
  }

  return config::write_to_file(cfg);
}


std::optional<std::string> build(std::span<std::string_view> args) {
  std::string_view build_type {};
  std::string_view target {};
  bool testing_enabled { false };
  bool clean { false };
  bool reconfigure { false };
  bool verbose { false };

  for (const auto arg : args) {
    if (arg == "--release") {
      build_type = "Release";
    }
    else if (arg == "--debug") {
      build_type = "Debug";
    }
    else if (arg == "--tests") {
      testing_enabled = true;
    }
    else if (arg == "--no-tests") {
      testing_enabled = false;
    }
    else if (arg == "--clean") {
      clean = true;
    }
    else if (arg == "--verbose" || arg == "-v") {
      verbose = true;
    }
    else if (arg.starts_with("--target=")) {
      // Length of "--target="
      constexpr std::size_t target_arg_len { 9 };
      target = arg.substr(target_arg_len);
    }
    else if (arg == "--help") {
      std::println(literals::BUILD_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      return std::make_optional(
        std::format("gojo build: argument not recognized: {}", arg)
      );
    }
  }

  auto config_result { config::read_from_file(CONFIG_FILE) };
  if (!config_result.has_value()) {
    return std::make_optional(std::move(config_result.error()));
  }

  config::GojoConfig cfg { std::move(config_result.value()) };

  reconfigure = ((cfg.build_type != build_type) && !cfg.multi_config) ||
                (cfg.testing_enabled != testing_enabled);

  if (reconfigure) {
    if (cfg.build_system == "CMake") {
      auto conf_result { cmake::configure(cfg, verbose, true) };
      if (conf_result.has_value()) {
        return conf_result;
      }
    }

    if (cfg.build_system == "Meson") {
      // TODO: Implement Meson
    }

    auto write_result { config::write_to_file(cfg) };
    if (write_result.has_value()) {
      return write_result;
    };
  } // reconfigure

  if (cfg.build_system == "CMake") {
    auto build_result { cmake::build(cfg, target, clean, verbose) };
    if (build_result.has_value()) {
      return build_result;
    }
  } // CMake

  if (cfg.build_system == "Meson") {
    // TODO: Implement Meson
  }

  return std::nullopt;
}


std::optional<std::string> run(std::span<std::string_view> args) {
  for (const auto arg : args) {
    if (arg == "--help") {
      std::println(literals::RUN_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }

  const config::GojoConfig cfg { std::move(result.value()) };

  // Can't think of a better way to write this since the format
  // string needs to be constexpr...
  std::string cmd {};
  if (cfg.multi_config) {
    cmd = std::format("./build/src/{}/{} {}",
                      cfg.build_type,
                      cfg.executable_name,
                      args_to_string(args));
  }
  else {
    cmd = std::format("./build/{}/src/{} {}",
                      cfg.build_type,
                      cfg.executable_name,
                      args_to_string(args));
  }

  std::filesystem::current_path(cfg.project_root);
  auto cmd_result { utils::execute_command(cmd) };
  if (!cmd_result.success) {
    return std::make_optional(
      std::format("failed to run {} executable", cfg.executable_name)
    );
  }

  return std::nullopt;
}


std::optional<std::string> test(std::span<std::string_view> args) {
  std::string target {};
  for (const auto arg : args) {
    if (arg.starts_with("--target=")) {
      // Length of "--target="
      constexpr std::size_t target_arg_len { 9 };
      target = std::format("-R \"{}\"", arg.substr(target_arg_len));
    }
    else if (arg == "--help") {
      std::println(literals::TEST_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      return std::make_optional(
        std::format("gojo test: argument not recognized: {}", arg)
      );
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }

  const config::GojoConfig cfg { result.value() };
  if (!cfg.testing_enabled) {
    return std::make_optional("testing not enabled");
  }

  std::filesystem::path build { cfg.project_root + "/build" };
  if (!std::filesystem::exists(build)) {
    return std::make_optional(
      std::format("directory not found: {}", build.string())
    );
  }

  if (cfg.multi_config) {
    build /= "test";
  }
  else {
    build /= cfg.build_type + "/test";
  }
  if (!std::filesystem::exists(build)) {
    return std::make_optional(
      std::format("directory not found: {}", build.string())
    );
  }

  std::string build_type {};
  if (cfg.multi_config) {
    build_type = std::format("-C {}", cfg.build_type);
  }

  std::filesystem::current_path(build);
  const std::string cmd {
    std::format("ctest {} {}", build_type, target)
  };
  auto cmd_result { utils::execute_command(cmd) };
  if (!cmd_result.success) {
    return std::make_optional("failed to run tests");
  }

  return std::nullopt;
}


std::optional<std::string> clean(std::span<std::string_view> args) {
  bool deep { false };
  bool all { false };

  for (const auto arg : args) {
    if (arg == "--deep") {
      deep = true;
      break;
    }
    else if (arg == "--all") {
      all = true;
      break;
    }
    else if (arg == "--help") {
      std::println(literals::CLEAN_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      return std::make_optional(
        std::format("gojo clean: argument not recognized: {}", arg)
      );
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }

  const config::GojoConfig cfg { std::move(result.value()) };

  if (cfg.build_system == "CMake") {
    if (all) {
      // Delete entire build folder.
      clean_build_dir(cfg);
      return cmake::configure(cfg, false);
    }
    else {
      return cmake::clean(cfg, deep);
    }
  }

  if (cfg.build_system == "Meson") {
    // TODO: Implement Meson
  }

  return std::nullopt;
}


std::optional<std::string> fmt(std::span<std::string_view> args) {
  std::string fmt_args { "-style=file -i" };

  for (const auto arg : args) {
    if (arg == "--help") {
      std::println(literals::FMT_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      fmt_args = args_to_string(args);
      break;
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }

  config::GojoConfig cfg { std::move(result.value()) };

  // TODO: Write command option to change style
  /*
  if (!style.empty()) {
    std::string cmd {
      std::format("clang-format -style={} -dump-config > .clang-format", style)
    };
    std::filesystem::current_path(cfg.project_root);
    auto cmd_result { utils::execute_command(cmd, true, true) };
    if (!cmd_result.success) {
      return std::make_optional(
        std::format("gojo fmt: style not recognized: {}", style)
      );
    }
    cfg.fmt_style = style;
    config::write_to_file(cfg);
  }
  */

  constexpr std::string_view fmt_str {
    "find {} "
    "-iname '*.{}' -o "
    "-iname '*.{}' "
    "| xargs clang-format {}"
  };
  const std::string cmd {
    std::format(fmt_str,
                cfg.project_root,  // {0}
                cfg.hdr_ext,       // {1}
                cfg.src_ext,       // {2}
                fmt_args)          // {3}
  };

  std::filesystem::current_path(cfg.project_root);
  auto cmd_result { utils::execute_command(cmd) };
  if (!cmd_result.success) {
    return std::make_optional("failed to format");
  }

  return std::nullopt;
}


std::optional<std::string> check(std::span<std::string_view> args) {
  bool verbose { false };

  for (const auto arg : args) {
    if (arg == "--verbose" || arg == "-v") {
      verbose = true;
    }
    else if (arg == "--help") {
      std::println(literals::CHECK_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      return std::make_optional(
        std::format("gojo check: argument not recognized: {}", arg)
      );
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }
  config::GojoConfig cfg { std::move(result.value()) };

  if (!cfg.clang_tidy_enabled && !cfg.cppcheck_enabled) {
    return std::make_optional("no checks enabled");
  }

  std::filesystem::path build { 
    std::filesystem::current_path() / "build" / cfg.build_type
  };
  if (cfg.multi_config) {
    build = std::filesystem::current_path() / "build";
  }
  if (!std::filesystem::exists(build)) {
    return std::make_optional(
      std::format("directory not found: {}", build.string())
    );
  }
  if (!std::filesystem::exists(build / "compile_commands.json")) {
    return std::make_optional(
      std::format("file not found: {}/compile_commands.json", build.string())
    );
  }

  unsigned int num_cores { std::thread::hardware_concurrency() };

  if (cfg.clang_tidy_enabled) {
    if (!std::filesystem::exists(cfg.project_root + "/.clang-tidy")) {
      return std::make_optional(
        std::format("file not found: {}/.clang-tidy", cfg.project_root)
      );
    }

    std::string_view ignore {};
    if (!cfg.compiler.starts_with("clang")) {
      // Some flags may be used in other compilers that clang does not
      // recognize. This flag ignores any unknown flags.
      ignore = "-extra-arg=-Wno-unknown-warning-option";
    }

    // Run clang-tidy
    constexpr std::string_view cmd_str {
      "run-clang-tidy "
      "-p {0} "
      "-j {1} "
      "{2} "
      "{3}"
    };
    const std::string cmd {
      std::format(cmd_str,
                  build.string(),      // {0}
                  num_cores,           // {1}
                  ignore,              // {2}
                  cfg.clang_tidy_args) // {3}
    };
    std::filesystem::current_path(cfg.project_root);
    if (verbose) {
      std::println(MAGENTA("Running clang-tidy...\n"));
    }
    auto cmd_result { utils::execute_command(cmd, !verbose) };
    if (!cmd_result.success) {
      return std::make_optional("failed clang-tidy checks");
    }
    if (verbose) {
      std::println(GREEN("No problems found!"));
    }
  }

  if (cfg.cppcheck_enabled) {
    int lang_standard { cfg.lang_standard };
    std::string_view lang { utils::to_lower(cfg.project_lang) };
    if (cfg.project_lang == "C++" && cfg.lang_standard > utils::STD20) {
      lang_standard = utils::STD20;
      lang = "c++";
    } else if (cfg.project_lang == "C" && cfg.lang_standard > utils::STD11) {
      lang_standard = utils::STD11;
      lang = "c";
    }

    // Run cppcheck
    constexpr std::string_view cmd_str {
      "cppcheck "
      "--project={0}/compile_commands.json "
      "-j {1} "
      "--std={2}{3} "
      "--enable={4} "
      "{5}"
    };
    const std::string cmd {
      std::format(cmd_str,
                  build.string(),      // {0}
                  num_cores,           // {1}
                  lang,                // {2}
                  lang_standard,       // {3}
                  cfg.cppcheck_checks, // {4}
                  cfg.cppcheck_args)   // {5}
    };
    std::filesystem::current_path(cfg.project_root);
    if (verbose) {
      std::println(MAGENTA("Running cppcheck...\n"));
    }
    auto cmd_result { utils::execute_command(cmd, !verbose) };
    if (!cmd_result.success) {
      return std::make_optional("failed cppcheck checks");
    }
    if (verbose) {
      std::println(GREEN("No problems found!"));
    }
  }

  return std::nullopt;
}


std::optional<std::string> file_template(std::span<std::string_view> args) {
  for (const auto arg : args) {
    if (arg == "--help") {
      std::println(literals::TEMPLATE_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else {
      return std::make_optional(
        std::format("gojo template: argument not recognized: {}", arg)
      );
    }
  }
  return config::create_template(std::filesystem::current_path());
}


std::optional<std::string> refresh(std::span<std::string_view> args,
                                   std::optional<config::GojoConfig> cfg_in) {
  std::string_view profile {};
  bool verbose { false };

  for (const auto arg : args) {
    if (arg == "--help") {
      std::println(literals::REFRESH_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else if ( arg == "--verbose" || arg == "-v") {
      verbose = true;
    }
    else if (arg.starts_with("--profile=")) {
      // length of "--profile=".
      constexpr std::size_t profile_arg_len { 10 };
      profile = arg.substr(profile_arg_len);
    }
    else {
      return std::make_optional(
        std::format("gojo refresh: argument not recognized: {}", arg)
      );
    }
  }
  bool reconfigure { true };

  config::GojoConfig cfg {};
  if (cfg_in.has_value()) {
    cfg = std::move(cfg_in.value());
    // Build directory has been wiped, need to configure from
    // scratch instead of reconfigure.
    reconfigure = false;
  }
  else {
    if (!profile.empty()) {
      auto result { config::read_profile(profile) };
      if (!result.has_value()) {
        if (result.error().starts_with("file not found")) {
          return std::make_optional(
            std::format("gojo init: no profile with name '{}'", profile)
          );
        }
        return std::make_optional(std::move(result.error()));
      }

      std::string project_name { cfg.project_name };
      std::string project_root { cfg.project_root };
      std::string executable_name { cfg.executable_name };

      clean_build_dir(cfg);
      reconfigure = false;

      cfg = std::move(result.value());
      cfg.project_name = std::move(project_name);
      cfg.project_root = std::move(project_root);
      cfg.executable_name = std::move(executable_name);
    }
    else {
      auto result { config::read_from_file(CONFIG_FILE) };
      if (!result.has_value()) {
        return std::make_optional(std::move(result.error()));
      }
      cfg = std::move(result.value());
    }
  }

  if (cfg.build_system == "CMake") {
    cfg.multi_config = config::is_multi_config(cfg.generator);
    auto cfg_result { cmake::configure(cfg, verbose, reconfigure) };
    if (cfg_result.has_value()) {
      return cfg_result;
    }
  }

  if (cfg.build_system == "Meson") {
    // TODO: Implement Meson
  }

  return config::write_to_file(cfg);
}


std::optional<std::string> dep(std::span<std::string_view> args) {
  if (args.empty()) {
    return std::make_optional("gojo dep: missing option");
  }

  std::string_view dep_option = args.front();
  if (dep_option == "--help") {
    std::println(literals::DEP_HELP,
                 utils::CLEAR,
                 utils::MAGENTA_BOLD,
                 utils::MAGENTA,
                 utils::YELLOW,
                 utils::CYAN);
    return std::nullopt;
  }
  else if (dep_option == "search") {
    if (args.size() > 2) {
      return std::make_optional("gojo dep search: too many arguments");
    }
    auto result { config::read_from_file(CONFIG_FILE) };
    if (!result.has_value()) {
      return std::make_optional(std::move(result.error()));
    }

    config::GojoConfig cfg { std::move(result.value()) };

    if (cfg.pkg_manager == "none") {
      // Check for any local gojo packages.
    }
    if (cfg.pkg_manager == "conan") {
      // clang tidy doesn't like the use of unchecked access in containers.
      // There is literally no other way to access std::span in C++23.
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
      return conan::search(args[1]);
    }
    else if (cfg.pkg_manager == "vcpkg") {
      // TODO: Implement vcpkg.
    }
  }
  else if (dep_option == "install") {
    auto result { config::read_from_file(CONFIG_FILE) };
    if (!result.has_value()) {
      return std::make_optional(std::move(result.error()));
    }

    config::GojoConfig cfg { std::move(result.value()) };

    if (cfg.pkg_manager == "none") {
      // Check for any local gojo packages.
    }
    else if (cfg.pkg_manager == "conan") {
      return conan::install(cfg, false);
    }
    else if (cfg.pkg_manager == "vcpkg") {
      // TODO: Implement vcpkg.
    }
  }
  else if (dep_option == "add") {
    if (args.size() < 2) {
      return std::make_optional("gojo dep add: missing argument");
    }
    if (args.size() > 3) {
      return std::make_optional("gojo dep add: too many arguments");
    }
    std::string_view version { "latest" };
    if (args.size() == 3) {
      version = args[2];
    }
    auto result { config::read_from_file(CONFIG_FILE) };
    if (!result.has_value()) {
      return std::make_optional(std::move(result.error()));
    }

    config::GojoConfig cfg { std::move(result.value()) };

    if (cfg.pkg_manager == "none") {
      // Check for any local gojo packages.
    }
    else if (cfg.pkg_manager == "conan") {
      std::filesystem::path conanfile { cfg.project_root + "/conanfile.py" };
      if (!std::filesystem::exists(conanfile)) {
        conanfile = cfg.project_root + "/conanfile.txt";
        if (!std::filesystem::exists(conanfile)) {
          return std::make_optional(
            std::format("file not found: {}", conanfile.string())
          );
        }
      }

      return conan::add(conanfile, args[1], version);
    }
    else if (cfg.pkg_manager == "vcpkg") {
      // TODO: Implement vcpkg.
    }
  }
  else if (dep_option == "remove") {
    if (args.size() < 2) {
      return std::make_optional("gojo dep remove: missing argument");
    }
    if (args.size() > 2) {
      return std::make_optional("gojo dep remove: too many arguments");
    }

    auto result { config::read_from_file(CONFIG_FILE) };
    if (!result.has_value()) {
      return std::make_optional(std::move(result.error()));
    }

    config::GojoConfig cfg { std::move(result.value()) };

    std::filesystem::path conanfile { cfg.project_root + "/conanfile.py" };
    if (!std::filesystem::exists(conanfile)) {
      conanfile = cfg.project_root + "/conanfile.txt";
      if (!std::filesystem::exists(conanfile)) {
        return std::make_optional(
          std::format("file not found: {}", conanfile.string())
        );
      }
    }

    if (cfg.pkg_manager == "none") {
      // Check for any local gojo packages.
    }
    else if (cfg.pkg_manager == "conan") {
      return conan::remove(conanfile, args[1]);
    }
    else if (cfg.pkg_manager == "vcpkg") {
      // TODO: Implement vcpkg.
    }
  }
  else if (dep_option == "uninstall") {
    if (args.size() < 2) {
      return std::make_optional("gojo dep remove: missing argument");
    }
    if (args.size() > 2) {
      return std::make_optional("gojo dep remove: too many arguments");
    }

    auto result { config::read_from_file(CONFIG_FILE) };
    if (!result.has_value()) {
      return std::make_optional(std::move(result.error()));
    }

    config::GojoConfig cfg { std::move(result.value()) };

    if (cfg.pkg_manager == "none") {
      // Check for any local gojo packages.
    }
    else if (cfg.pkg_manager == "conan") {
      return conan::uninstall(args[1]);
    }
    else if (cfg.pkg_manager == "vcpkg") {
      // TODO: Implement vcpkg.
    }
  }
  else {
    return std::make_optional(
      std::format("gojo dep: option not recognized: {}", dep_option)
    );
  }

  return std::nullopt;
}


std::optional<std::string> install(std::span<std::string_view> args) {
  bool verbose { false };
  for (const auto arg : args) {
    if (arg == "--help") {
      std::println(literals::INSTALL_HELP,
                   utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN);
      return std::nullopt;
    }
    else if (arg == "--verbose" || arg == "-v") {
      verbose = true;
    }
    else {
      return std::make_optional(
        std::format("gojo install: argument not recognized: {}", arg)
      );
    }
  }

  auto result { config::read_from_file(CONFIG_FILE) };
  if (!result.has_value()) {
    return std::make_optional(std::move(result.error()));
  }
  config::GojoConfig cfg { std::move(result.value()) };

  if (cfg.pkg_manager == "none") {
    return std::make_optional("gojo install: no dependencies detected");
  }

  clean_build_dir(cfg);

  if (cfg.pkg_manager == "conan") {
    auto conan_result { conan::install(cfg, verbose) };
    if (conan_result.has_value()) {
      return conan_result;
    }
  } // conan

  if (cfg.pkg_manager == "vcpkg") {
    // TODO: Implement vcpkg
  } // vcpkg

  return refresh({}, std::make_optional(std::move(cfg)));
}


std::optional<std::string> profile(std::span<std::string_view> args) {
  if (args.empty()) {
    return std::make_optional("gojo profile: missing option");
  }

  std::string_view profile_option = args.front();
  if (profile_option == "--help") {
    std::println(literals::PROFILE_HELP,
                 utils::CLEAR,
                 utils::MAGENTA_BOLD,
                 utils::MAGENTA,
                 utils::YELLOW,
                 utils::CYAN);
    return std::nullopt;
  }
  else if (profile_option == "create") {
    if (args.size() == 1) {
      return std::make_optional("gojo profile create: missing name argument");
    }
    if (args.size() > 2) {
      return std::make_optional("gojo profile create: too many arguments");
    }
    // clang tidy doesn't like the use of unchecked access in containers.
    // There is literally no other way to access std::span in C++23.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    auto result { config::create_profile(args[1]) };
    if (result.has_value()) {
      return result;
    }
  }
  else if (profile_option == "list") {
    if (args.size() > 1) {
      return std::make_optional("gojo profile list: too many arguments");
    }
    // list files in ~/.gojocli/profiles
    const std::filesystem::path profiles { utils::get_profiles_dir() };
    for (const auto& profile : std::filesystem::directory_iterator{profiles}) {
      std::println("{}", profile.path().filename().string());
    }
  }
  else if (profile_option == "show") {
    if (args.size() == 1) {
      return std::make_optional("gojo profile show: missing name argument");
    }
    if (args.size() > 2) {
      return std::make_optional("gojo profile show: too many arguments");
    }

    // clang tidy doesn't like the use of unchecked access in containers.
    // There is literally no other way to access std::span in C++23.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    const std::string_view name { args[1] };
    const std::filesystem::path profiles { utils::get_profiles_dir() };

    std::ifstream profile_stream { profiles / name };
    if (!profile_stream.is_open()) {
      return std::make_optional(
        std::format("failed to open file: {}", (profiles / name).string())
      );
    }
    const std::string content(
      (std::istreambuf_iterator<char>(profile_stream)),
      std::istreambuf_iterator<char>()
    );

    std::println("{}", content);
  }
  else if (profile_option == "delete") {
    if (args.size() == 1) {
      return std::make_optional("gojo profile delete: missing name argument");
    }
    if (args.size() > 2) {
      return std::make_optional("gojo profile delete: too many arguments");
    }

    // clang tidy doesn't like the use of unchecked access in containers.
    // There is literally no other way to access std::span in C++23.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    const std::string_view name { args[1] };
    const std::filesystem::path profiles { utils::get_profiles_dir() };

    if (name == "all") {
      // delete all current profiles
      std::filesystem::remove_all(profiles);
      if (!std::filesystem::create_directory(profiles)) {
        return std::make_optional(
          std::format("failed to create directory: {}", profiles.string())
        );
      }
    }
    else {
      if (!std::filesystem::remove(profiles / name)) {
        return std::make_optional(
          std::format("gojo profile delete: no profile with name '{}'", name)
        );
      }
    }
  }
  else {
    return std::make_optional(
      std::format("gojo profile: option not recognized: {}", profile_option)
    );
  }

  return std::nullopt;
}


void info() {
  std::println(literals::INFO,
               utils::CLEAR,
               utils::MAGENTA_BOLD,
               utils::MAGENTA,
               utils::YELLOW,
               utils::CYAN);
}


void help() {
  std::println(literals::HELP,
               utils::CLEAR,
               utils::MAGENTA_BOLD,
               utils::MAGENTA,
               utils::YELLOW,
               utils::CYAN);
}

}  // namespace commands
