enable_testing()

find_package(GTest REQUIRED)

# Link all following targets to gtest
link_libraries(gtest::gtest)

# Config
add_executable(config_test)

target_sources(
  config_test
  PRIVATE
  config_test.cpp
)

target_include_directories(
  config_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  config_test
  PRIVATE
  cfg
)


# Utils
add_executable(utils_test)

target_sources(
  utils_test
  PRIVATE
  utils_test.cpp
)

target_include_directories(
  utils_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  utils_test
  PRIVATE
  build_systems
  package_managers
  utils
  cmd
  cfg
)


# CMake
add_executable(cmake_test)

target_sources(
  cmake_test
  PRIVATE
  cmake_test.cpp
)

target_include_directories(
  cmake_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  cmake_test
  PRIVATE
  cfg
  cmd
  package_managers
  build_systems
)


# Conan
add_executable(conan_test)

target_sources(
  conan_test
  PRIVATE
  conan_test.cpp
)

target_include_directories(
  conan_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  conan_test
  PRIVATE
  build_systems
  cfg
  cmd
  package_managers
  utils
)


# Init
add_executable(init_test)

target_sources(
  init_test
  PRIVATE
  init_test.cpp
)

target_include_directories(
  init_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  init_test
  PRIVATE
  cmd
  cfg
  build_systems
  package_managers
)


# Build
add_executable(build_test)

target_sources(
  build_test
  PRIVATE
  build_test.cpp
)

target_include_directories(
  build_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  build_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


# Run
add_executable(run_test)

target_sources(
  run_test
  PRIVATE
  run_test.cpp
)

target_include_directories(
  run_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  run_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


# Test
add_executable(test_test)

target_sources(
  test_test
  PRIVATE
  test_test.cpp
)

target_include_directories(
  test_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  test_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


# Clean
add_executable(clean_test)

target_sources(
  clean_test
  PRIVATE
  clean_test.cpp
)

target_include_directories(
  clean_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  clean_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


# Fmt
add_executable(fmt_test)

target_sources(
  fmt_test
  PRIVATE
  fmt_test.cpp
)

target_include_directories(
  fmt_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  fmt_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


# Check
add_executable(check_test)

target_sources(
  check_test
  PRIVATE
  check_test.cpp
)

target_include_directories(
  check_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  check_test
  PRIVATE
  cmd
  utils
  cfg
  build_systems
  package_managers
)


include(GoogleTest)
gtest_discover_tests(config_test)
gtest_discover_tests(utils_test)
gtest_discover_tests(cmake_test)
gtest_discover_tests(conan_test)
gtest_discover_tests(init_test)
gtest_discover_tests(build_test)
gtest_discover_tests(run_test)
gtest_discover_tests(test_test)
gtest_discover_tests(clean_test)
gtest_discover_tests(fmt_test)
gtest_discover_tests(check_test)
