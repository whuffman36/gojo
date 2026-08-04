#include "src/templates.hpp"

#include <format>
#include <string>
#include <string_view>

#include "src/config.hpp"
#include "src/utils.hpp"


namespace {

std::string_view get_cmake_version() {
  // Version 4.0 cutoff support for version 3.5 and under and
  // was released in March 2025.
  // Most recent version (July 2026): 4.3.4
  return "4.0...4.3.4";
}

}  // namespace

namespace templates {

std::string cmakelists_root(const config::GojoConfig& cfg) {
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
                     get_cmake_version(), // {0}
                     cfg.project_name,    // {1}
                     testing_enabled);    // {2}
}


std::string cmakelists_src(const config::GojoConfig& cfg) {
  std::string_view target { "executable" };
  std::string_view target_name { cfg.executable_name };
  std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
  std::string_view link_type {};

  if (cfg.is_library) {
    target = "library";
    target_name = cfg.project_name;
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
                     cfg.src_ext,    // {3}
                     include_dirs);  // {4}
}


std::string cmakelists_test(const config::GojoConfig& cfg) {
  std::string_view test_framework_package {};
  std::string_view include_dirs { "\"${CMAKE_SOURCE_DIR}\"" };
  std::string_view lib {};
  std::string_view name { cfg.executable_name };
  std::string add_test {};

  if (cfg.is_library) {
    name = cfg.project_name;
    lib = cfg.project_name;
    include_dirs = "\"${CMAKE_SOURCE_DIR}\"\n"
                   "  \"${CMAKE_SOURCE_DIR}/include\"";
  }

  if (cfg.testing_enabled) {
    if (cfg.testing_framework == "GoogleTest") {
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

    if (cfg.testing_framework == "Unity") {
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
                     cfg.src_ext,             // {2}
                     lib,                     // {3}
                     include_dirs,            // {4}
                     add_test);               // {5}
}


std::string source_file(const config::GojoConfig& cfg) {
  std::string str {};

  // This might be the most convoluted, fucked up control statement
  // block I've ever written...
  // Maybe I should split this up into several functions?
  if (cfg.is_library) {
    if (cfg.project_lang == "C++") {
      str = std::format(
R"(#include "{0}/{0}.{1}"


namespace {0} {{

const char* hello_world() {{
  return "Hello world!";
}}

}}  // namespace {0}
)",
        cfg.project_name,  // {0}
        cfg.hdr_ext        // {1}
      );
    } // project_lang == C++
    else if (cfg.project_lang == "C") {
      str = std::format(
R"(#include "{0}/{0}.{1}"


const char* hello_world(void) {{
  return "Hello world!";
}}
)",
        cfg.project_name,  // {0}
        cfg.hdr_ext        // {1}
      );
    } // project_lang == C
  } // is_library
  else {
    if (cfg.project_lang == "C++") {
      if (cfg.lang_standard >= utils::STD23) {
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
    else if (cfg.project_lang == "C") {
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


std::string header_file(const config::GojoConfig& cfg) {
  std::string str {};

  if (cfg.project_lang == "C++") {
    str = std::format(
R"(#ifndef {0}_{0}_{1}
#define {0}_{0}_{1}


namespace {0} {{

// Returns "Hello world!".
const char* hello_world();

}}  // namespace {0}

#endif
)",
      cfg.project_name,  // {0}
      cfg.hdr_ext        // {1}
    );
  }
  else if (cfg.project_lang == "C") {
    str = std::format(
R"(#ifndef {0}_{0}_{1}
#define {0}_{0}_{1}


// Returns "Hello world!".
const char* hello_world(void);

#endif
)",
      cfg.project_name,  // {0}
      cfg.hdr_ext        // {1}
    );
  }

  return str;
}


std::string test_file(const config::GojoConfig& cfg) {
  // is library
  //   is C++
  //     is framework enabled
  std::string str {};

  if (cfg.is_library) {
    if (cfg.project_lang == "C++") {
      if (cfg.testing_framework == "none") {
        str = std::format(
R"(#include <cassert>

#include "{0}/{0}.{1}"


int main() {{
  assert({0}::hello_world() == "Hello world!");
  return 0;
}}
)",
          cfg.project_name,
          cfg.hdr_ext
        );
      }
      else if (cfg.testing_framework == "GoogleTest") {
        str = std::format(
R"(#include <gtest/gtest.h>

#include "{0}/{0}.{1}"


TEST({0}TestSuite, HelloWorld) {{
  EXPECT_STREQ("Hello world!", {0}::hello_world());
}}
)",
          cfg.project_name,
          cfg.hdr_ext
        );
      } // gtest
    } // C++
    else if (cfg.project_lang == "C") {
      if (cfg.testing_framework == "none") {
        str = std::format(
R"(#include <assert.h>
#include <string.h>

#include "{0}/{0}.{1}"


int main(void) {{
    assert(strcmp(hello_world(), "Hello world!") == 0);
    return 0;
}}
)",
          cfg.project_name,
          cfg.hdr_ext
        );
      }
      else if (cfg.testing_framework == "Unity") {
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
          cfg.project_name,
          cfg.hdr_ext
        );
      } // unity
    } // C
  } // is_library
  else {
    if (cfg.project_lang == "C++") {
      if (cfg.testing_framework == "none") {
        str =
R"(
int main() {
  static_assert(5 + 5 == 10);
  return 0;
}
)";
      }
      else if (cfg.testing_framework == "GoogleTest") {
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
    else if (cfg.project_lang == "C") {
      if (cfg.testing_framework == "none") {
        str =
R"(#include <assert.h>
#include <string.h>


int main(void) {
    assert(5 + 5 == 10);
    return 0;
}
)";
      }
      else if (cfg.testing_framework == "Unity") {
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


std::string clangd(const config::GojoConfig& cfg) {
  std::string compilation_db {};
  if (cfg.multi_config) {
    compilation_db = std::format("{}/build", cfg.project_root);
  }
  else {
    compilation_db = std::format("{}/build/{}",
                                 cfg.project_root,
                                 cfg.build_type);
  }

  return std::format(
R"(CompileFlags:
  CompilationDatabase: "{}"
)",
    compilation_db
  );
}


std::string clangtidy(const config::GojoConfig& cfg) {
  std::string_view header_filter { "src/.*" };
  if (cfg.is_library) {
    header_filter = "include/.*|src/.*";
  }

  std::string_view checks {};
  if (cfg.project_lang == "C++") {
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
  if (cfg.project_lang == "C") {
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


std::string conanfile(const config::GojoConfig& cfg) {
  std::string_view reqs { "pass" };
  if (cfg.testing_enabled) {
    if (cfg.project_lang == "C++") {
      reqs = "self.requires(\"gtest/1.17.0\")";
    } 
    else if (cfg.project_lang == "C") {
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
    cfg.project_name,  // {0}
    reqs               // {1}
  );
}

}  // namespace templates
