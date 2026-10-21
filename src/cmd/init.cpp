#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <print>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "src/build_systems/cmake.hpp"
#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/cmd/commands.hpp"
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
    }  // if

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
    };  // template_str

    return std::format(template_str,
                       cmake::get_version(),  // {0}
                       cfg.name,              // {1}
                       testing_enabled);      // {2}
}

std::string cmakelists_src(const cfg::Config& cfg) {
    std::string_view target { "executable" };
    std::string_view target_name { cfg.name };
    std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
    std::string_view link_type {};

    if (cfg.target == cfg::Target::Library) {
        target = "library";
        include_dirs =
            "\"${CMAKE_SOURCE_DIR}\"\n"
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
    };  // template_str

    return std::format(template_str,
                       target,                      // {0}
                       target_name,                 // {1}
                       link_type,                   // {2}
                       cfg.get_source_extension(),  // {3}
                       include_dirs);               // {4}
}

std::string cmakelists_test(const cfg::Config& cfg) {
    std::string_view test_framework_package {};
    std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
    std::string_view lib {};
    std::string_view name { cfg.name };
    std::string add_test {};

    if (cfg.target == cfg::Target::Library) {
        lib = cfg.name;
        include_dirs =
            "\"${CMAKE_SOURCE_DIR}\"\n"
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
        }  // project_lang == C++

        if (cfg.test_framework.value() == cfg::TestFramework::Unity) {
            test_framework_package =
                R"(find_package(unity REQUIRED)

# Link all following targets to Unity.
link_libraries(unity::unity)
)";
        }  // project_lang == C
    }  // test_framework

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
    };  // template_str

    return std::format(template_str,
                       test_framework_package,      // {0}
                       name,                        // {1}
                       cfg.get_source_extension(),  // {2}
                       lib,                         // {3}
                       include_dirs,                // {4}
                       add_test);                   // {5}
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
                cfg.name,                   // {0}
                cfg.get_header_extension()  // {1}
            );
        }  // project_lang == C++
        else if (cfg.language == cfg::Language::C) {
            str = std::format(
                R"(#include "{0}/{0}.{1}"


const char* hello_world(void) {{
  return "Hello world!";
}}
)",
                cfg.name,                   // {0}
                cfg.get_header_extension()  // {1}
            );
        }  // project_lang == C
    }  // is_library
    else {
        if (cfg.language == cfg::Language::Cpp) {
            str =
                R"(#include <iostream>

int main() {
  std::cout << "Hello world!\n";
  return 0;
}
)";
        }  // project_lang == C++
        else if (cfg.language == cfg::Language::C) {
            str =
                R"(#include <stdio.h>

int main(void) {
  printf("Hello world!\n");
  return 0;
}
)";
        }  // project_lang == C
    }  // is_binary

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
            cfg.name,                   // {0}
            cfg.get_header_extension()  // {1}
        );
    } else if (cfg.language == cfg::Language::C) {
        str = std::format(
            R"(#ifndef {0}_{0}_{1}
#define {0}_{0}_{1}


// Returns "Hello world!".
const char* hello_world(void);

#endif
)",
            cfg.name,                   // {0}
            cfg.get_header_extension()  // {1}
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
                    cfg.name, cfg.get_header_extension()
                );
            } else if (cfg.test_framework == cfg::TestFramework::GoogleTest) {
                str = std::format(
                    R"(#include <gtest/gtest.h>

#include "{0}/{0}.{1}"


TEST({0}TestSuite, HelloWorld) {{
  EXPECT_STREQ("Hello world!", {0}::hello_world());
}}
)",
                    cfg.name, cfg.get_header_extension()
                );
            }  // gtest
        }  // C++
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
                    cfg.name, cfg.get_header_extension()
                );
            } else if (cfg.test_framework == cfg::TestFramework::Unity) {
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
                    cfg.name, cfg.get_header_extension()
                );
            }  // unity
        }  // C
    }  // is_library
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
            } else if (cfg.test_framework == cfg::TestFramework::GoogleTest) {
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
            }  // gtest
        }  // C++
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
            } else if (cfg.test_framework == cfg::TestFramework::Unity) {
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
            }  // unity
        }  // C
    }  // is_binary

    return str;
}

std::string clangd(const cfg::Config& cfg) {
    std::string_view remove {};
    switch (cfg.get_compiler()) {
        case cfg::Compiler::Gcc:
        case cfg::Compiler::Gpp:
        case cfg::Compiler::HomebrewGcc:
        case cfg::Compiler::HomebrewGpp:
            remove =
                R"(Remove:
    - -fmodules-ts
    - -fmodule-mapper=*
    - -fdeps-format=*
)";
            break;
        default: break;
    }
    constexpr std::string_view template_str {
        R"(CompileFlags:
  CompilationDatabase: "{}"
  {}
  )"
    };

    return std::format(template_str, cfg.get_build_dir().string(), remove);
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
        } else if (cfg.language == cfg::Language::C) {
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
        reqs       // {1}
    );
}

#define fill_config_field_from_input(field_label, num_options, option_type,  \
                                     field_name, default_value)              \
    while (!success) {                                                       \
        std::println("{}:\n\t[0] Skip", field_label);                        \
        for (std::uint8_t i { 1 }; i <= (num_options); i++) {                \
            std::println("\t[{}] {}", i,                                     \
                         cfg::to_str(static_cast<option_type>(i - 1)));      \
        }                                                                    \
        std::print("\nEnter option: ");                                      \
        std::getline(input_stream, input);                                   \
        if (input == "q") {                                                  \
            return std::nullopt;                                             \
        }                                                                    \
        try {                                                                \
            std::uint8_t parsed_input {                                      \
                static_cast<std::uint8_t>(std::stoi(input))                  \
            };                                                               \
            if (verbose) {                                                   \
                std::println("\noption entered: {}", parsed_input);          \
            }                                                                \
            if (parsed_input == 0) {                                         \
                /* Skip */                                                   \
                cfg.field_name = default_value;                              \
                if (verbose) {                                               \
                    std::println("\nskipping...");                           \
                }                                                            \
                success = true;                                              \
            } else if (parsed_input > (num_options)) {                       \
                /* Invalid option. */                                        \
                utils::gojo_print(                                           \
                    utils::LogLevel::Error,                                  \
                    "option must be one of the numbers in the list above"    \
                );                                                           \
                success = false;                                             \
            } else {                                                         \
                cfg.field_name = static_cast<option_type>(parsed_input - 1); \
                success = true;                                              \
            }                                                                \
        } catch (const std::invalid_argument& e) {                           \
            utils::gojo_print(utils::LogLevel::Error,                        \
                              "option must be an integer");                  \
            success = false;                                                 \
        } catch (const std::out_of_range& e) {                               \
            utils::gojo_print(                                               \
                utils::LogLevel::Error,                                      \
                "option must be one of the numbers in the list above"        \
            );                                                               \
            success = false;                                                 \
        }                                                                    \
        std::print("\n\n\n");                                                \
    }                                                                        \
    success = false;

#define fill_config_field_from_bool_input(field_label, field_name,           \
                                          default_value)                     \
    while (!success) {                                                       \
        std::print(                                                          \
            "Enable {}?\n\t[0] Skip\n\t[1] Yes\n\t[2] No\n\nEnter option: ", \
            field_label                                                      \
        );                                                                   \
        std::getline(input_stream, input);                                   \
        if (input == "q") {                                                  \
            return std::nullopt;                                             \
        }                                                                    \
        try {                                                                \
            std::uint8_t parsed_input {                                      \
                static_cast<std::uint8_t>(std::stoi(input))                  \
            };                                                               \
            if (verbose) {                                                   \
                std::println("\noption entered: {}", parsed_input);          \
            }                                                                \
            if (parsed_input == 0) {                                         \
                /* Skip */                                                   \
                cfg.field_name = default_value;                              \
                if (verbose) {                                               \
                    std::println("\nskipping...");                           \
                }                                                            \
                success = true;                                              \
            } else if (parsed_input > 2) {                                   \
                /* Invalid option. */                                        \
                utils::gojo_print(utils::LogLevel::Error,                    \
                                  "option must be 0, 1, or 2");              \
                success = false;                                             \
            } else {                                                         \
                cfg.field_name = parsed_input == 1;                          \
                success = true;                                              \
            }                                                                \
        } catch (const std::invalid_argument& e) {                           \
            utils::gojo_print(utils::LogLevel::Error,                        \
                              "option must be an integer");                  \
            success = false;                                                 \
        } catch (const std::out_of_range& e) {                               \
            utils::gojo_print(utils::LogLevel::Error,                        \
                              "option must be 0, 1, or 2");                  \
            success = false;                                                 \
        }                                                                    \
        std::print("\n\n\n");                                                \
    }                                                                        \
    success = false;

#define fill_config_field_from_str_input(field_label, field_name, \
                                         default_value)           \
    while (!success) {                                            \
        std::print("Enter {}: ", field_label);                    \
        std::getline(input_stream, input);                        \
        if (input == "q") {                                       \
            return std::nullopt;                                  \
        }                                                         \
        if (input.empty()) {                                      \
            cfg.field_name = default_value;                       \
            if (verbose) {                                        \
                std::println("\nskipping...");                    \
            }                                                     \
            success = true;                                       \
        } else {                                                  \
            cfg.field_name = input;                               \
            if (verbose) {                                        \
                std::println("\nentered: {}", input);             \
            }                                                     \
            success = true;                                       \
        }                                                         \
        std::print("\n\n\n");                                     \
    }                                                             \
    success = false;

std::optional<cfg::Config> interactive_mode(std::istream& input_stream,
                                            bool verbose) {
    cfg::Config cfg {};

    std::string input {};
    bool success { false };

    while (!success) {
        std::print("Project name:  ");
        std::getline(input_stream, input);
        if (input == "q") {
            return std::nullopt;
        } else if (input.empty()) {
            utils::gojo_print(utils::LogLevel::Error,
                              "Project name is a required field");
            success = false;
            break;
        } else if (input.contains(' ')) {
            utils::gojo_print(utils::LogLevel::Error,
                              "Project name must not contain spaces");
            success = false;
            break;
        } else {
            cfg.name = input;
            if (verbose) {
                std::println("\nentered: {}", input);
            }
            success = true;
        }
        std::print("\n\n\n");
    }
    success = false;

    fill_config_field_from_input("Project Language", cfg::LANGUAGE_OPTIONS,
                                 cfg::Language, language, cfg::Language::Cpp);
    fill_config_field_from_input("Project Target", cfg::TARGET_OPTIONS,
                                 cfg::Target, target, cfg::Target::Binary);

    fill_config_field_from_bool_input("Verbose Mode", verbose, std::nullopt);

    // Build
    fill_config_field_from_input("Language Standard", cfg::LANGUAGE_STD_OPTIONS,
                                 cfg::LanguageStandard, language_standard,
                                 std::nullopt);
    fill_config_field_from_input("Compiler", cfg::COMPILER_OPTIONS,
                                 cfg::Compiler, compiler, std::nullopt);
    fill_config_field_from_str_input("Compiler Flags in a Space Separated List",
                                     compiler_flags, std::nullopt);
    fill_config_field_from_bool_input("GNU Extensions", gnu_extensions_enabled,
                                      std::nullopt);
    fill_config_field_from_input("Build System", cfg::BUILD_SYSTEM_OPTIONS,
                                 cfg::BuildSystem, build_system, std::nullopt);
    fill_config_field_from_input("Generator", cfg::GENERATOR_OPTIONS,
                                 cfg::Generator, generator, std::nullopt);
    fill_config_field_from_input("Initial Build Type", cfg::BUILD_TYPE_OPTIONS,
                                 cfg::BuildType, build_type, std::nullopt);

    // Checks
    fill_config_field_from_input("clang-format Style",
                                 cfg::FORMAT_STYLE_OPTIONS, cfg::FormatStyle,
                                 clang_format_style, std::nullopt);
    fill_config_field_from_str_input("Desired Language Server", language_server,
                                     std::nullopt);
    fill_config_field_from_bool_input("'clang-tidy'", clang_tidy_enabled,
                                      std::nullopt);
    fill_config_field_from_str_input(
        "Arguments for 'clang-tidy' in a Space Separated List", clang_tidy_args,
        std::nullopt
    );
    fill_config_field_from_bool_input("'cppcheck'", cppcheck_enabled,
                                      std::nullopt);
    fill_config_field_from_str_input(
        "Checks for 'cppcheck' in a Comma Separated List", cppcheck_checks,
        std::nullopt
    );
    fill_config_field_from_str_input(
        "Arguments for 'cppcheck' in a Space Separated List", cppcheck_args,
        std::nullopt
    );

    // Tests
    fill_config_field_from_bool_input("Testing", testing_enabled, std::nullopt);
    fill_config_field_from_input("Unit Test Framwork",
                                 cfg::TEST_FRAMEWORK_OPTIONS,
                                 cfg::TestFramework, test_framework,
                                 std::nullopt);

    // Dependencies
    fill_config_field_from_input("Package Manager",
                                 cfg::PACKAGE_MANAGER_OPTIONS,
                                 cfg::PackageManager, package_manager,
                                 std::nullopt);

    // Special logic for cfg.build_types
    while (!success) {
        std::print(
            "Enter All Build Types to Build Dependencies With in a Comma Separated List: "
        );
        std::getline(input_stream, input);
        if (input == "q") {
            return std::nullopt;
        }
        if (verbose) {
            std::println("entered: {}", input);
        }

        std::size_t start { 0 };
        std::vector<std::string_view> build_types {};
        for (std::size_t idx { 0 }; idx < input.size(); idx++) {
            if (input.at(idx) == ',') {
                build_types.emplace_back(&input.at(start), idx - start);
                start = idx + 1;
            }
        }
        build_types.emplace_back(&input.at(start), input.size() - start);

        std::size_t num_build_types { 0 };
        for (const auto build_type : build_types) {
            if (!cfg::build_type_from_str(build_type).has_value()) {
                utils::gojo_print(utils::LogLevel::Error,
                                  "build type not recognized: {}", build_type);
                success = false;
                break;
            } else {
                // clang-format off
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index, cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
                cfg.build_types[num_build_types] =
                    // clang-format on
                    cfg::build_type_from_str(build_type);
                if (verbose) {
                    std::println("build type added: {}", build_type);
                }
                num_build_types++;
                if (num_build_types >= cfg.build_types.size()) {
                    success = true;
                    break;
                }
            }
        }
        success = true;
        std::print("\n\n\n");
    }

    return cfg;
}

std::optional<cfg::Config> parse_arguments(std::span<std::string_view> args,
                                           std::istream& input) {
    cfg::Config cfg {};
    cfg.language = cfg::Language::Cpp;

    for (const auto arg : args) {
        if (arg == "--verbose" || arg == "-v") {
            cfg.verbose = true;
        } else if (arg == "--interactive" || arg == "-i") {
            return interactive_mode(input, cfg.is_verbose_enabled());
        } else if (arg == "--lib") {
            cfg.target = cfg::Target::Library;
        } else if (arg.starts_with("--lang=")) {
            if (arg.ends_with('c')) {
                cfg.language = cfg::Language::C;
            } else if (arg.ends_with("c++")) {
                cfg.language = cfg::Language::Cpp;
            } else {
                constexpr std::string_view fmt_str {
                    "gojo init --lang: argument not recognized: {}\n"
                    "\targument must be 'c' or 'c++'"
                };

                // Length of "--lang=".
                constexpr std::size_t lang_arg_len { 7 };
                throw std::runtime_error {
                    std::format(fmt_str, arg.substr(lang_arg_len))
                };
            }
        } else if (arg == "--tests") {
            cfg.testing_enabled = true;
        } else if (arg.starts_with("--profile=")) {
            // length of "--profile=".
            constexpr size_t profile_arg_len { 10 };
            const std::string_view profile_name { arg.substr(profile_arg_len) };
            const std::string_view tmp_name { cfg.name };
            cfg = cfg::Config::from_file(utils::get_profiles_dir() /
                                         profile_name);
            cfg.name = tmp_name;
        } else if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return std::nullopt;
        } else if (arg.starts_with("--") || arg.starts_with('-')) {
            throw std::runtime_error {
                std::format("gojo init: option not recognized: {}", arg)
            };
        } else {
            cfg.name = arg;
            if (cfg.name.contains(' ')) {
                throw std::runtime_error {
                    "gojo init: name must not contain spaces"
                };
            }
        }
    }

    return cfg;
}

void create_file(const std::filesystem::path& filepath,
                 std::string_view contents) {
    std::filesystem::create_directories(filepath.parent_path());
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error { std::format("failed to open file: {}\n{}",
                                               filepath.string(),
                                               std::strerror(errno)) };
    }

    file << contents << std::flush;
}

void create_files(const cfg::Config& cfg) {
    const std::string main { std::format("{}.{}", cfg.name,
                                         cfg.get_source_extension()) };
    const std::string lib { std::format("{}.{}", cfg.name,
                                        cfg.get_header_extension()) };
    const std::string test { std::format("{}_test.{}", cfg.name,
                                         cfg.get_source_extension()) };

    if (!std::filesystem::create_directory(std::filesystem::current_path() /
                                           cfg.name)) {
        throw std::runtime_error { std::format("failed to create directory: {}",
                                               cfg.name) };
    }

    std::filesystem::current_path(std::filesystem::current_path() / cfg.name);

    create_file(utils::get_root_dir() / "src" / main, source_file(cfg));

    if (cfg.target == cfg::Target::Library) {
        create_file(utils::get_root_dir() / "include" / cfg.name / lib,
                    header_file(cfg));
    }

    switch (cfg.get_build_system()) {
        case cfg::BuildSystem::CMake:
            create_file(utils::get_root_dir() / "CMakeLists.txt",
                        cmakelists_root(cfg));
            create_file(utils::get_root_dir() / "src" / "CMakeLists.txt",
                        cmakelists_src(cfg));
            if (cfg.is_testing_enabled()) {
                create_file(utils::get_root_dir() / "test" / "CMakeLists.txt",
                            cmakelists_test(cfg));
                create_file(utils::get_root_dir() / "test" / test,
                            test_file(cfg));
            }
            break;
    }

    if (cfg.package_manager.has_value()) {
        switch (cfg.package_manager.value()) {
            case cfg::PackageManager::Conan:
                create_file(utils::get_root_dir() / "conanfile.py",
                            conanfile(cfg));
                break;
            case cfg::PackageManager::Vcpkg: break;
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

}  // namespace

namespace cmd {

void init(std::span<std::string_view> args, std::istream& input) {
    if (args.empty()) {
        throw std::runtime_error { "gojo init: missing name argument" };
    }

    auto result { parse_arguments(args, input) };
    if (!result.has_value()) {
        return;
    }

    cfg::Config cfg { std::move(result.value()) };

    create_files(cfg);

    // init conan
    if (cfg.package_manager.has_value()) {
        switch (cfg.package_manager.value()) {
            case cfg::PackageManager::Conan:
                {
                    conan::install(cfg, cfg.is_verbose_enabled());
                    break;
                }
            case cfg::PackageManager::Vcpkg: break;
        }
    }

    // init cmake
    switch (cfg.get_build_system()) {
        case cfg::BuildSystem::CMake:
            cmake::configure(cfg, cfg.is_verbose_enabled());
            break;
    }

    const auto git_result {
        utils::execute_command("git init", false, cfg.is_verbose_enabled(),
                               cfg.get_build_dir() / "log.txt")
    };
    if (!git_result.success) {
        throw std::runtime_error { "failed to initialize git repository" };
    }

    cfg.to_file(utils::get_config_path());
}

};  // namespace cmd
