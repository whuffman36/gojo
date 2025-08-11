pub fn root_cmake_lists_txt(name: &str, std: &str, cpp: &str, version: &str) -> String {
  format!(
    "cmake_minimum_required(VERSION {version})

project( {name}
  VERSION 1.0
  DESCRIPTION \"\" # TODO: Add a description.
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD {std})
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS \"${{CMAKE_CXX_FLAGS}} -Wall -Wextra -Werror\")

# Allow clangd and clang-tidy to do static analysis.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Run clang-tidy. Used in 'gojo check' command.
if(STATIC_CHECK)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# Bring subdirectories into scope.
add_subdirectory(src/lib)

add_executable( {name}
  src/main.{cpp}
)

# target_include_directories( {name}
    # Add include directories here.
# )

target_link_libraries( {name}
  lib
  # Add libraries here.
)
  
# Tests
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(test)
endif()"
  )
}

pub fn main_src(hpp: &str) -> String {
  format!(
    "#include \"lib/hello_world.{hpp}\"
#include <iostream>

int main() {{
  std::cout << hello_world() << \"\\n\";
  return 0;
}}
"
  )
}

pub fn lib_hello_world_src(hpp: &str) -> String {
  format!(
    "#include \"hello_world.{hpp}\"

const char* hello_world() {{
  return \"Hello World!\";
}}
"
  )
}

pub fn lib_hello_world_hdr(hpp: &str) -> String {
  let upper_hpp = hpp.to_uppercase();
  format!(
    "#ifndef LIB_HELLO_WORLD_{upper_hpp}
#define LIB_HELLO_WORLD_{upper_hpp}

const char* hello_world();

#endif
"
  )
}

pub fn lib_cmake_lists_txt(cpp: &str) -> String {
  format!(
    "add_library( lib
  STATIC
  hello_world.{cpp}
)

# target_include_directories( lib
    # Add include directories here.
# )

target_link_libraries( lib
  # Add libraries here.
)"
  )
}

pub fn test_hello_world_src(hpp: &str) -> String {
  format!(
    "#include \"../src/lib/hello_world.{hpp}\"

#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions) {{
  EXPECT_EQ(hello_world(), \"Hello World!\");
}}
"
  )
}

pub fn test_cmake_lists_txt(cpp: &str) -> String {
  format!(
    "enable_testing()
set(CMAKE_CXX_CLANG_TIDY \"\")

#include(FetchContent)
#FetchContent_Declare(
#  googletest
#  GIT_REPOSITORY https://github.com/google/googletest.git
#  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
#)
#FetchContent_MakeAvailable(googletest)

include_directories($ENV{{HOME}}/.gojo/include)

add_executable( hello_world_test
  hello_world_test.{cpp}
)

target_link_libraries( hello_world_test
  lib
  $ENV{{HOME}}/.gojo/lib/gtest/libgtest_main.a
  #GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_world_test)
# Add more tests here."
  )
}

pub fn readme(name: &str) -> String {
  format!(
    "# {name}

TODO: write a readme...
"
  )
}
