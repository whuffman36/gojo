#include "src/cmd/commands.hpp"

#include <cerrno>
#include <cstring>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string_view>

#include "src/build_systems/cmake.hpp"
#include "src/package_managers/conan.hpp"
#include "src/utils.hpp"


namespace {

constexpr std::string_view GITIGNORE {
R"(build
deps)"
};

constexpr std::string_view HELP {
R"({1}Command:{0}
  gojo {2}init{0} {3}<name>{0} {4}[options]{0}

{1}Summary:{0}
  Create a new gojo project in the current directory. A name arguemnt
  is required. If no options are supplied, a default C++ project is
  created. The settings for a default C++ project can be found in the
  'gojo info' command.

{1}Arguments:{0}
  {3}name{0}            Name of the gojo project to be created.

{1}Options:{0}
  {4}--file | -f{0}       Create a new gojo project using a .gojo config
                    file located in the current directory. This option
                    syngergizes well with 'gojo template'. All other
                    arguments and options are ignored when using --file,
                    including <name>. This command will fail if no .gojo
                    config file is found in the current directory.

  {4}-i{0}                Create a new gojo project by answering several
                    interactive prompts. All other arguments and options
                    are ignored when using -i, including <name>.

  {4}--lib{0}             Set the project to be a library. By default, a new
                    gojo project is a binary executable.

  {4}--lang=<c|c++>{0}    Set the language of the project. The language options
                    are 'c' or 'c++'. By default, a new gojo project is C++.

  {4}--profile=<name>{0}  Create a new gojo project from an existing profile.
                    The profile must exist. When combining this option with
                    options, this one must come first, ie
                    gojo init --profile=default --lib
)"
};


std::string cmakelists_root(const cfg::Config& cfg) {
  std::string_view testing_enabled {};
  if (cfg.testing_enabled) {
    testing_enabled = 
R"(# Bring test directory into scope.
# Bring other directories into scope before this line to use them in tests.
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif())";
  } // if

  constexpr std::string_view template_str {
R"(cmake_minimum_required(VERSION {0})

project({1})

# Export compilation information for
# clangd, .clang-tidy, .clang-format, and cppcheck
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Do not remove comments below.
# Add dependencies here, ex:
# find_package(<package_name> REQUIRED)

# Bring src directory into scope
add_subdirectory(src)

# Add subdirectories here.

{2}
)"
  }; // template_str

  return std::format(template_str,
                     cmake::get_version(), // {0}
                     cfg.name,    // {1}
                     testing_enabled);    // {2}
}


std::string cmakelists_src(const cfg::Config& cfg) {
  std::string_view target { "executable" };
  std::string_view target_name { cfg.name };
  std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
  std::string_view link_type {};

  if (cfg.target == cfg::Target::Library) {
    target = "library";
    include_dirs = "\"${CMAKE_SOURCE_DIR}\"\n"
                   "  \"${CMAKE_SOURCE_DIR}/include\"";
    link_type = " STATIC";
  }

  constexpr std::string_view template_str {
R"(add_{0}({1}{2})

target_sources(
  {1}
  PRIVATE
  # Add source files here
  {1}.{3}
)

target_link_libraries(
  {1}
  PRIVATE
  # Add libraries here
)

target_include_directories(
  {1}
  PRIVATE
  # Add include directories here
  {4}
)

# Add other targets here, ex.
# add_library(<lib_name> STATIC)
#
# target_sources(
#   <lib_name>
#   PRIVATE
#   lib.cpp
#   ...
# )
# ...
)"
  }; // template_str

  return std::format(template_str,
                     target,         // {0}
                     target_name,    // {1}
                     link_type,      // {2}
                     cfg.get_source_extension(),    // {3}
                     include_dirs);  // {4}
}


std::string cmakelists_test(const cfg::Config& cfg) {
  std::string_view test_framework_package {};
  std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
  std::string_view lib {};
  std::string_view name { cfg.name };
  std::string add_test {};

  if (cfg.target == cfg::Target::Library) {
    lib = cfg.name;
    include_dirs = "\"${CMAKE_SOURCE_DIR}\"\n"
                   "  \"${CMAKE_SOURCE_DIR}/include\"";
  }

  if (cfg.is_testing_enabled() && cfg.test_framework.has_value()) {
    if (cfg.test_framework.value() == cfg::TestFramework::GoogleTest) {
      test_framework_package =
R"(find_package(GTest REQUIRED)

# Link all following targets to GoogleTest.
link_libraries(gtest::gtest)
)";

      add_test = std::format(
R"(include(GoogleTest)
gtest_discover_tests({0}_test)
# Add more tests here, ex.
# gtest_discover_tests(<test_target>)
# ...
)",
        name  // {0}
      );
    } // project_lang == C++

    if (cfg.test_framework.value() == cfg::TestFramework::Unity) {
      test_framework_package =
R"(find_package(unity REQUIRED)

# Link all following targets to Unity.
link_libraries(unity::unity)
)";
    } // project_lang == C
  } // test_framework

  if (add_test.empty()) {
    add_test = std::format(
R"(add_test(NAME {0}Test COMMAND {0}_test)
# Add more tests here, ex.
# add_test(NAME <test_name> COMMAND <test_target>)
# ...
)",
      name  // {0}
    );
  }

  constexpr std::string_view template_str {
R"({0}
add_executable({1}_test)

target_sources(
  {1}_test
  PRIVATE
  # Add source files here
  {1}_test.{2}
)

target_link_libraries(
  {1}_test
  PRIVATE
  # Add libraries here
  {3}
)

target_include_directories(
  {1}_test
  PRIVATE
  # Add include directories here
  {4}
)

# Add more tests here

{5}
)"
  }; // template_str

  return std::format(template_str,
                     test_framework_package,  // {0}
                     name,                    // {1}
                     cfg.get_source_extension(),             // {2}
                     lib,                     // {3}
                     include_dirs,            // {4}
                     add_test);               // {5}
}


std::string source_file(const cfg::Config& cfg) {
  std::string str {};

  // This might be the most convoluted, fucked up control statement
  // block I've ever written...
  // Maybe I should split this up into several functions?
  if (cfg.target == cfg::Target::Library) {
    if (cfg.language == cfg::Language::Cpp) {
      str = std::format(
R"(#include "{0}/{0}.{1}"
#include <string>


namespace {0} {{

std::string hello_world() {{
  return "Hello world!";
}}

}}  // namespace {0}
)",
        cfg.name,  // {0}
        cfg.get_header_extension()        // {1}
      );
    } // project_lang == C++
    else if (cfg.language == cfg::Language::C) {
      str = std::format(
R"(#include "{0}/{0}.{1}"


const char* hello_world(void) {{
  return "Hello world!";
}}
)",
        cfg.name,  // {0}
        cfg.get_header_extension()        // {1}
      );
    } // project_lang == C
  } // is_library
  else {
    if (cfg.language == cfg::Language::Cpp) {
      if (cfg.get_language_standard() == cfg::LanguageStandard::Cpp23 || cfg.get_language_standard() == cfg::LanguageStandard::Cpp26) {
        str =
R"(#include <print>

int main() {
  std::println("Hello world!");
  return 0;
}
)";
      } // std >= 23
      else {
        str =
R"(#include <iostream>

int main() {
  std::cout << "Hello world!\n";
  return 0;
}
)";        
      } // std < 23
    } // project_lang == C++
    else if (cfg.language == cfg::Language::C) {
      str =
R"(#include <stdio.h>

int main(void) {
  printf("Hello world!\n");
  return 0;
}
)";
    } // project_lang == C
  } // is_binary

  return str;
}


std::string header_file(const cfg::Config& cfg) {
  std::string str {};

  if (cfg.language == cfg::Language::Cpp) {
    str = std::format(
R"(#ifndef {0}_{0}_{1}
#define {0}_{0}_{1}

#include <string>


namespace {0} {{

// Returns "Hello world!".
std::string hello_world();

}}  // namespace {0}

#endif
)",
      cfg.name,  // {0}
      cfg.get_header_extension()        // {1}
    );
  }
  else if (cfg.language == cfg::Language::C) {
    str = std::format(
R"(#ifndef {0}_{0}_{1}
#define {0}_{0}_{1}


// Returns "Hello world!".
const char* hello_world(void);

#endif
)",
      cfg.name,  // {0}
      cfg.get_header_extension()        // {1}
    );
  }

  return str;
}


std::string test_file(const cfg::Config& cfg) {
  // is library
  //   is C++
  //     is framework enabled
  std::string str {};

  if (cfg.target == cfg::Target::Library) {
    if (cfg.language == cfg::Language::Cpp) {
      if (!cfg.test_framework.has_value()) {
        str = std::format(
R"(#include <cassert>

#include "{0}/{0}.{1}"


int main() {{
  assert({0}::hello_world() == "Hello world!");
  return 0;
}}
)",
          cfg.name,
          cfg.get_header_extension()
        );
      }
      else if (cfg.test_framework == cfg::TestFramework::GoogleTest) {
        str = std::format(
R"(#include <gtest/gtest.h>

#include "{0}/{0}.{1}"


TEST({0}TestSuite, HelloWorld) {{
  EXPECT_STREQ("Hello world!", {0}::hello_world());
}}
)",
          cfg.name,
          cfg.get_header_extension()
        );
      } // gtest
    } // C++
    else if (cfg.language == cfg::Language::C) {
      if (!cfg.test_framework.has_value()) {
        str = std::format(
R"(#include <assert.h>
#include <string.h>

#include "{0}/{0}.{1}"


int main(void) {{
    assert(strcmp(hello_world(), "Hello world!") == 0);
    return 0;
}}
)",
          cfg.name,
          cfg.get_header_extension()
        );
      }
      else if (cfg.test_framework == cfg::TestFramework::Unity) {
        str = std::format(
R"(#include <unity.h>

#include "{0}/{0}.{1}"


void setUp(void) {{
    // Runs before every single test case
}}

void tearDown(void) {{
    // Runs after every single test case
}}

void test_hello_world(void) {{
    TEST_ASSERT_EQUAL_STRING("Hello world!", hello_world());
}}

int main(void) {{
    UNITY_BEGIN();
    RUN_TEST(test_hello_world);
    return UNITY_END();
}}
)",
          cfg.name,
          cfg.get_header_extension()
        );
      } // unity
    } // C
  } // is_library
  else {
    if (cfg.language == cfg::Language::Cpp) {
      if (!cfg.test_framework.has_value()) {
        str =
R"(
int main() {
  static_assert(5 + 5 == 10);
  return 0;
}
)";
      }
      else if (cfg.test_framework == cfg::TestFramework::GoogleTest) {
        str =
R"(#include <gtest/gtest.h>


// Demonstrate some basic assertions.
// Taken directly from GoogleTest website.
TEST(TestSuiteName, TestName) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
)";
      } // gtest
    } // C++
    else if (cfg.language == cfg::Language::C) {
      if (!cfg.test_framework.has_value()) {
        str =
R"(#include <assert.h>
#include <string.h>


int main(void) {
    assert(5 + 5 == 10);
    return 0;
}
)";
      }
      else if (cfg.test_framework == cfg::TestFramework::Unity) {
        str =
R"(#include <unity.h>


void setUp(void) {
    // Runs before every single test case
}

void tearDown(void) {
    // Runs after every single test case
}

void test_example(void) {
    TEST_ASSERT_EQUAL_INT(5, 3 + 2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_example);
    return UNITY_END();
}
)";
      } // unity
    } // C
  } // is_binary

  return str;
}


std::string clangd(const cfg::Config& cfg) {
  constexpr std::string_view template_str {
R"(CompileFlags:
  CompilationDatabase: "{}")"
  };

  return std::format(template_str, cfg.get_build_dir().string());
}


std::string clangtidy(const cfg::Config& cfg) {
  std::string_view header_filter { "src/.*" };
  if (cfg.target == cfg::Target::Library) {
    header_filter = "include/.*|src/.*";
  }

  std::string_view checks {};
  if (cfg.language == cfg::Language::Cpp) {
    checks =
  R"(>
  -*,
  bugprone-*,
  modernize-*,
  -modernize-use-trailing-return-type,
  performance-*,
  cppcoreguidelines-*,
  misc-*)";
  }

  // clang-tidy is designed for C++, these were specifically chosen
  // from https://nrk.neocities.org/articles/c-static-analyzers#clang-tidy
  if (cfg.language == cfg::Language::C) {
    checks =
R"(>
  performance-*,
  misc-*,
  android-cloexec-*,
  readability-duplicate-include,
  readability-misleading-indentation,
  bugprone-assert-side-effect,
  bugprone-macro-repeated-side-effects,
  bugprone-infinite-loop,
  bugprone-macro-parentheses,
  bugprone-posix-return,
  bugprone-reserved-identifier,
  bugprone-signal-handler,
  bugprone-signed-char-misuse,
  bugprone-sizeof-expression,
  bugprone-branch-clone,
  -clang-analyzer-security.insecureAPI.*,
  -misc-no-recursion)";
  }

  return std::format(
R"(---
# .clang-tidy defines predetermined static analysis checks that will be run
# by clang-tidy, a program provided by the LLVM suite. Run these checks by
# running 'gojo check' at the project root. Make sure clang-tidy is installed
# on your system and enabled in your .gojo config file.
#
# See the whole list of available checks here:
# https://clang.llvm.org/extra/clang-tidy/checks/list.html
Checks: {0}
#
# Set any of the checks above to be treated as errors instead of warnings.
# Must be in the list defined above.
# '*' Sets all enabled checks to be errors
# Leaving all checks as warnings causes clang-tidy to return successfully,
# even if errors are found. Set these as errors, or use the --verbose flag
# to see them when running 'gojo check'.
WarningsAsErrors: '*'
#
# Header files to be checked. By default, only source files are checked 
# by clang-tidy. For example:
# '.*' checks every single header file included in your project excluding
#      system headers
# 'include/.*|src/.*' checks any header files in the 'include' or 'src'
#                     directories
HeaderFilterRegex: '{1}'
#
# Turns on colors in the terminal output.
UseColor: true
)",
    checks,        // {0}
    header_filter  // {1}
  );
}


std::string conanfile(const cfg::Config& cfg) {
  std::string_view reqs { "pass" };
  if (cfg.is_testing_enabled()) {
    if (cfg.language == cfg::Language::Cpp) {
      reqs = "self.requires(\"gtest/1.17.0\")";
    } 
    else if (cfg.language == cfg::Language::C) {
      reqs = "self.requires(\"unity/2.6.0\")";
    }
  }

  return std::format(
R"(from conan import ConanFile
from conan.tools.cmake import cmake_layout


# This class defines the rules for any dependencies you
# bring into your project. You can find more info at
# https://docs.conan.io/2/reference/conanfile.html
class {0}Recipe(ConanFile):
    # Each time you change one of these in your .gojo
    # config file, conan will need to reinstall and
    # rebuild your dependencies.
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # Add dependencies here, ex.
        # self.requires(<package_name>/<version>)
        {1}

    def layout(self):
        cmake_layout(self)
)",
    cfg.name,  // {0}
    reqs               // {1}
  );
}


std::optional<cfg::Config> parse_arguments(std::span<std::string_view> args) {
  cfg::Config cfg {};
  cfg.language = cfg::Language::Cpp;

  for (const auto arg : args) {
    if (arg == "--lib") {
      cfg.target = cfg::Target::Library;
    }
    else if (arg.starts_with("--lang=")) {
      if (arg.ends_with("c")) {
        cfg.language = cfg::Language::C;
      }
      else if (arg.ends_with("c++")) {
        cfg.language = cfg::Language::Cpp;
      }
      else {
        constexpr std::string_view fmt_str {
          "gojo init --lang: argument not recognized: {}\n"
          "\targument must be 'c' or 'c++'"
        };
        
        // Length of "--lang=".
        constexpr std::size_t lang_arg_len { 7 };
        throw std::runtime_error { std::format(fmt_str, arg.substr(lang_arg_len)) };
      }
    }
    else if (arg == "--tests") {
      cfg.testing_enabled = true;
    }
    else if (arg.starts_with("--profile=")) {
      // length of "--profile=".
      constexpr size_t profile_arg_len { 10 };
      std::string_view profile_name { arg.substr(profile_arg_len) };
      std::string_view tmp_name { cfg.name };
      cfg = cfg::Config::from_file(utils::get_profiles_dir() / profile_name);
      cfg.name = tmp_name;
    }
    else if (arg == "--help") {
      utils::print_info(HELP);
      return std::nullopt;
    }
    else if (arg.starts_with("--") || arg.starts_with("-")) {
      throw std::runtime_error { std::format("gojo init: option not recognized: {}", arg) };
    }
    else {
      cfg.name = arg;
    }
  }

  return cfg;
}


void create_file(std::filesystem::path filepath, std::string_view contents) {
  std::filesystem::create_directories(filepath.parent_path());
  std::ofstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error { std::format("failed to open file: {}\n{}", filepath.string(), std::strerror(errno)) };
  }

  file << contents << std::flush;
}


void create_files(const cfg::Config& cfg) {
  const std::string main { std::format("{}.{}", cfg.name, cfg.get_source_extension()) };
  const std::string lib { std::format("{}.{}", cfg.name, cfg.get_header_extension()) };
  const std::string test { std::format("{}_test.{}", cfg.name, cfg.get_source_extension()) };

  create_file(utils::get_root_dir() / "src" / main, source_file(cfg));

  if (cfg.target == cfg::Target::Library) {
    create_file(utils::get_root_dir() / "include" / cfg.name / lib, header_file(cfg));
  }

  switch (cfg.get_build_system()) {
    case cfg::BuildSystem::CMake:
      create_file(utils::get_root_dir() / "CMakeLists.txt", cmakelists_root(cfg));
      create_file(utils::get_root_dir() / "src" / "CMakeLists.txt", cmakelists_src(cfg));
      if (cfg.is_testing_enabled()) {
        create_file(utils::get_root_dir() / "test" / "CMakeLists.txt", cmakelists_test(cfg));
        create_file(utils::get_root_dir() / "test" / test, test_file(cfg));
      }
      break;
  }

  if (cfg.package_manager.has_value()) {
    switch (cfg.package_manager.value()) {
      case cfg::PackageManager::Conan:
        create_file(utils::get_root_dir() / "conanfile.py", conanfile(cfg));
        break;
      case cfg::PackageManager::Vcpkg:
        break;
    }
  }

  if (cfg.is_clang_tidy_enabled()) {
    create_file(utils::get_root_dir() / ".clang-tidy", clangtidy(cfg));
  }

  if (cfg.language_server.has_value() && cfg.language_server == "clangd") {
    create_file(utils::get_root_dir() / ".clangd", clangd(cfg));
  }

  create_file(utils::get_root_dir() / ".gitignore", GITIGNORE);
}

}

namespace cmd {

// --lib (creates library)
// --lang=<c/c++> (sets language)
// --profile=<name> (creates project from a profile in ~/.gojo/profiles/<name>)
// --help (displays help info)
void init(std::span<std::string_view> args) {
  if (args.empty()) {
    throw std::runtime_error { "gojo init: missing name argument" };
  }

  auto result { parse_arguments(args) };
  if (!result.has_value()) {
    return;
  }

  cfg::Config cfg { std::move(result.value()) }; 

  create_files(cfg);

  // init conan
  if (cfg.package_manager.has_value()) {
    switch (cfg.package_manager.value()) {
      case cfg::PackageManager::Conan: {
        conan::install(cfg, false);
        break;
      }
      case cfg::PackageManager::Vcpkg:
        break;
    }
  }

  // init cmake
  switch (cfg.get_build_system()) {
    case cfg::BuildSystem::CMake:
      cmake::configure(cfg, false);
      break;
  }

  auto git_result { utils::execute_command("git init") };
  if (!git_result.success) {
    throw std::runtime_error { "failed to initialize git repository" };
  }

  cfg.to_file(utils::get_config_path());
}

};
