cmake_minimum_required(VERSION 3.31.6...4.3.4)

project(example)

# Export compilation information for
# clangd, .clang-tidy, .clang-format, and cppcheck
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Do not remove comments below.
# Add dependencies here, ex:
# find_package(<package_name> REQUIRED)

# Bring src directory into scope
add_subdirectory(src)

# Add subdirectories here.

# Bring test directory into scope.
# Bring other directories into scope before this line to use them in tests.
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
