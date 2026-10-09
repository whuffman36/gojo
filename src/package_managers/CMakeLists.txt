add_library(package_managers)

target_sources(
  package_managers
  PRIVATE
  conan.cpp
)

target_include_directories(
  package_managers
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  package_managers
  PRIVATE
  cfg
  utils
)
