#include "src/cfg/config.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

// NOLINTNEXTLINE(misc-include-cleaner)
#include <toml++/toml.hpp>
#include <toml++/impl/array.hpp>
#include <toml++/impl/parser.hpp>
#include <toml++/impl/table.hpp>
#define TOML_IMPLEMENTATION

#include "src/cfg/enums.hpp"
#include "src/utils.hpp"


namespace {

// Config file labels.
constexpr std::string_view PROJECT { "project" };
constexpr std::string_view PROJECT_NAME   { "name" };
constexpr std::string_view PROJECT_LANGUAGE { "language" };
constexpr std::string_view PROJECT_TARGET { "target" };
constexpr std::string_view PROJECT_VERBOSE { "verbose" };

constexpr std::string_view BUILD { "build" };
constexpr std::string_view BUILD_LANGUAGE_STD { "language_standard" };
constexpr std::string_view BUILD_COMPILER { "compiler" };
constexpr std::string_view BUILD_COMPILER_FLAGS { "compiler_flags" };
constexpr std::string_view BUILD_GNU_EXTENSIONS { "gnu_extensions_enabled" };
constexpr std::string_view BUILD_BUILD_SYSTEM { "build_system" };
constexpr std::string_view BUILD_GENERATOR { "generator" };
constexpr std::string_view BUILD_BUILD_TYPE { "build_type" };

constexpr std::string_view CHECKS { "checks" };
constexpr std::string_view CHECKS_CLANG_FORMAT_STYLE { "clang_format_style" };
constexpr std::string_view CHECKS_LANGUAGE_SERVER { "language_server" };
constexpr std::string_view CHECKS_CLANG_TIDY_ENABLED { "clang_tidy_enabled" };
constexpr std::string_view CHECKS_CLANG_TIDY_ARGS { "clang_tidy_args" };
constexpr std::string_view CHECKS_CPPCHECK_ENABLED { "cppcheck_enabled" };
constexpr std::string_view CHECKS_CPPCHECK_CHECKS { "cppcheck_checks" };
constexpr std::string_view CHECKS_CPPCHECK_ARGS { "cppcheck_args" };

constexpr std::string_view TESTS { "tests" };
constexpr std::string_view TESTS_TESTING_ENABLED { "testing_enabled" };
constexpr std::string_view TESTS_TEST_FRAMEWORK { "test_framework" };

constexpr std::string_view DEPS { "dependencies" };
constexpr std::string_view DEPS_PACKAGE_MANAGER { "package_manager" };
constexpr std::string_view DEPS_BUILD_TYPES { "build_types" };


toml::array split_string(const std::string& str) {
  toml::array array {};
  std::size_t start_idx { 0 };

  for (std::size_t idx { 0 }; idx < str.size(); ++idx) {
    if (str.at(idx) == ' ') {
      array.emplace_back(str.substr(start_idx, idx));
      start_idx = idx;
    }
  }
  if (start_idx != str.size() - 1) {
    array.emplace_back(str.substr(start_idx, str.size()));
  }
  if (array.empty()) {
    array.emplace_back(str);
  }

  return array;
}


// NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access,-warnings-as-errors)
void validate_project_section(cfg::Config& cfg, const toml::table& table) {
  auto sv_result { table[PROJECT][PROJECT_NAME].value<std::string_view>() };
  if (!sv_result.has_value()) {
    throw std::runtime_error { "project.name field missing in gojo.toml" };
  }
  cfg.name = sv_result.value();

  sv_result = table[PROJECT][PROJECT_LANGUAGE].value<std::string_view>();
  if (!sv_result.has_value()) {
    throw std::runtime_error { "project.language field missing in gojo.toml" };
  }
  const auto language { cfg::language_from_str(sv_result.value()) };
  if (!language.has_value()) {
    throw std::runtime_error { std::format("project.language not recognized: {}", sv_result.value()) };
  }
  cfg.language = language.value();

  sv_result = table[PROJECT][PROJECT_TARGET].value<std::string_view>();
  if (!sv_result.has_value()) {
    throw std::runtime_error { "project.target field missing in gojo.toml" };
  }
  const auto target { cfg::target_from_str(sv_result.value()) };
  if (!target.has_value()) {
    throw std::runtime_error { std::format("project.target not recognized: {}", sv_result.value()) };
  }
  cfg.target = target.value();

  const auto bool_result { table[PROJECT][PROJECT_VERBOSE].value<bool>() };
  if (!bool_result.has_value()) {
    cfg.verbose = std::nullopt;
  }
  else {
    cfg.verbose = bool_result;
  }
}


void validate_build_section(cfg::Config& cfg, const toml::table& table) {
  auto int_result { table[BUILD][BUILD_LANGUAGE_STD].value<std::uint8_t>() };
  if (!int_result.has_value()) {
    cfg.language_standard = std::nullopt;
  }
  else {
    const auto language_standard { cfg::language_std_from_int(int_result.value(), cfg.language) };
    if (!language_standard.has_value()) {
      throw std::runtime_error { std::format("build.language_standard not recognized: {}", int_result.value()) };
    }
    cfg.language_standard = language_standard;
  }

  auto sv_result { table[BUILD][BUILD_COMPILER].value<std::string_view>() };
  if (!sv_result.has_value()) {
    cfg.compiler = std::nullopt;
  }
  else {
    const auto compiler { cfg::compiler_from_str(sv_result.value()) };
    if (!compiler.has_value()) {
      throw std::runtime_error { std::format("build.compiler not recognized: {}", sv_result.value()) };
    }
    cfg.compiler = compiler;
  }

  // auto array_result { table[BUILD][BUILD_COMPILER_FLAGS].value<toml::array>() };
  auto array_result { table[BUILD][BUILD_COMPILER_FLAGS].as_array() };
  if (array_result == nullptr || array_result->empty()) {
    cfg.compiler_flags = std::nullopt;
  }
  else {
    std::string compiler_flags {};
    for (const auto& elem : *array_result) {
      auto flag { elem.value<std::string_view>() };
      if (flag.has_value()) {
        compiler_flags.append(flag.value());
        compiler_flags.push_back(' ');
      }
    }
    compiler_flags.pop_back();
    cfg.compiler_flags = std::move(compiler_flags);
  }

  const auto bool_result { table[BUILD][BUILD_GNU_EXTENSIONS].value<bool>() };
  if (!bool_result.has_value()) {
    cfg.gnu_extensions_enabled = std::nullopt;
  }
  else {
    cfg.gnu_extensions_enabled = bool_result;
  }

  sv_result = table[BUILD][BUILD_BUILD_SYSTEM].value<std::string_view>();
  if (!sv_result.has_value()) {
    cfg.build_system = std::nullopt;
  }
  else {
    const auto build_system { cfg::build_system_from_str(sv_result.value()) };
    if (!build_system .has_value()) {
      throw std::runtime_error { std::format("build.build_system not recognized: {}", sv_result.value()) };
    }
    cfg.build_system = build_system;
  }

  sv_result = table[BUILD][BUILD_GENERATOR].value<std::string_view>();
  if (!sv_result.has_value()) {
    cfg.generator = std::nullopt;
  }
  else {
    const auto generator { cfg::generator_from_str(sv_result.value()) };
    if (!generator.has_value()) {
      throw std::runtime_error { std::format("build.generator not recognized: {}", sv_result.value()) };
    }
    cfg.generator = generator;
  }

  sv_result = table[BUILD][BUILD_BUILD_TYPE].value<std::string_view>();
  if (!sv_result.has_value()) {
    cfg.build_type = std::nullopt;
  }
  else {
    const auto build_type { cfg::build_type_from_str(sv_result.value()) };
    if (!build_type.has_value()) {
      throw std::runtime_error { std::format("build.build_type not recognized: {}", sv_result.value()) };
    }
    cfg.build_type = build_type;
  }
}


void validate_checks_section(cfg::Config& cfg, const toml::table& table) {
  auto sv_result { table[CHECKS][CHECKS_CLANG_FORMAT_STYLE].value<std::string_view>() };
  if (!sv_result.has_value()) {
    cfg.clang_format_style = std::nullopt;
  }
  else {
    const auto format_style { cfg::fromat_style_from_str(sv_result.value()) };
    if (!format_style.has_value()) {
      throw std::runtime_error { std::format("checks.clang_format_style not recognized: {}", sv_result.value()) };
    }
    cfg.clang_format_style = format_style;
  }

  sv_result = table[CHECKS][CHECKS_LANGUAGE_SERVER].value<std::string_view>();
  if (!sv_result.has_value()) {
    cfg.language_server = std::nullopt;
  }
  else {
    cfg.language_server = sv_result.value();
  }

  auto bool_result { table[CHECKS][CHECKS_CLANG_TIDY_ENABLED].value<bool>() };
  if (!bool_result.has_value()) {
    cfg.clang_tidy_enabled = std::nullopt;
  }
  else {
    cfg.clang_tidy_enabled = bool_result;
  }

  auto array_result { table[CHECKS][CHECKS_CLANG_TIDY_ARGS].as_array() };
  if (array_result == nullptr || array_result->empty()) {
    cfg.clang_tidy_args = std::nullopt;
  }
  else {
    std::string clang_tidy_args {};
    for (const auto& elem : *array_result) {
      auto arg { elem.value<std::string_view>() };
      if (arg.has_value()) {
        clang_tidy_args.append(arg.value());
        clang_tidy_args.push_back(' ');
      }
    }
    clang_tidy_args.pop_back();
    cfg.clang_tidy_args = std::move(clang_tidy_args);
  }

  bool_result = table[CHECKS][CHECKS_CPPCHECK_ENABLED].value<bool>();
  if (!bool_result.has_value()) {
    cfg.cppcheck_enabled = std::nullopt;
  }
  else {
    cfg.cppcheck_enabled = bool_result;
  }

  array_result = table[CHECKS][CHECKS_CPPCHECK_CHECKS].as_array();
  if (array_result == nullptr || array_result->empty()) {
    cfg.cppcheck_checks = std::nullopt;
  }
  else {
    std::string cppcheck_checks {};
    for (const auto& elem : *array_result) {
      auto check { elem.value<std::string_view>() };
      if (check.has_value()) {
        cppcheck_checks.append(check.value());
        cppcheck_checks.push_back(' ');
      }
    }
    cppcheck_checks.pop_back();
    cfg.cppcheck_checks = std::move(cppcheck_checks);
  }

  array_result = table[CHECKS][CHECKS_CPPCHECK_ARGS].as_array();
  if (array_result == nullptr || array_result->empty()) {
    cfg.cppcheck_args = std::nullopt;
  }
  else {
    std::string cppcheck_args {};
    for (const auto& elem : *array_result) {
      auto arg { elem.value<std::string_view>() };
      if (arg.has_value()) {
        cppcheck_args.append(arg.value());
        cppcheck_args.push_back(' ');
      }
    }
    cppcheck_args.pop_back();
    cfg.cppcheck_args = std::move(cppcheck_args);
  }
}


void validate_tests_section(cfg::Config& cfg, const toml::table& table) {
  const auto bool_result { table[TESTS][TESTS_TESTING_ENABLED].value<bool>() };
  if (!bool_result.has_value()) {
    cfg.testing_enabled = std::nullopt;
  }
  else {
    cfg.testing_enabled = bool_result;
  }

  auto sv_result { table[TESTS][TESTS_TEST_FRAMEWORK].value<std::string_view>() };
  if (!sv_result.has_value()) {
    cfg.test_framework = std::nullopt;
  }
  else {
    const auto test_framework { cfg::test_framework_from_str(sv_result.value()) };
    if (!test_framework.has_value()) {
      throw std::runtime_error { std::format("tests.test_framework not recognized: {}", sv_result.value()) };
    }
    cfg.test_framework = test_framework;
  }
}


void validate_deps_section(cfg::Config& cfg, const toml::table& table) {
  auto sv_result { table[DEPS][DEPS_PACKAGE_MANAGER].value<std::string_view>() };
  if (!sv_result.has_value()) {
    cfg.package_manager = std::nullopt;
  }
  else {
    const auto package_manager { cfg::package_manager_from_str(sv_result.value()) };
    if (!package_manager.has_value()) {
      throw std::runtime_error { std::format("deps.package_manager not recognized: {}", sv_result.value()) };
    }
    cfg.package_manager = package_manager;
  }

  auto array_result { table[DEPS][DEPS_BUILD_TYPES].as_array() };
  if (array_result == nullptr || array_result->empty()) {
    cfg.build_types.at(0) = std::nullopt;
    cfg.build_types.at(1) = std::nullopt;
    cfg.build_types.at(2) = std::nullopt;
    cfg.build_types.at(3) = std::nullopt;
  }
  else {
    if (array_result->size() > 4) {
      throw std::runtime_error { "deps.build_types can only contain up to four build_types" };
    }

    std::size_t idx { 0 };
    for (const auto& elem : *array_result) {
      if (!elem.value<std::string_view>().has_value()) {
        break;
      }
      const auto build_type { cfg::build_type_from_str(elem.value<std::string_view>().value()) };
      if (!build_type.has_value()) {
        throw std::runtime_error { std::format("deps.build_types[{}] not recognized: {}", idx, elem.value<std::string_view>().value()) };
      }
      cfg.build_types.at(idx) = build_type;
      ++idx;
    }
  }
}
// NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access,-warnings-as-errors)


cfg::Config validate_config(const toml::table& table) {
  cfg::Config cfg {};

  validate_project_section(cfg, table);
  validate_build_section(cfg, table);
  validate_checks_section(cfg, table);
  validate_tests_section(cfg, table);
  validate_deps_section(cfg, table);

  return cfg;
}


toml::table create_project_table(const cfg::Config& cfg) {
  toml::table project_table {};

  const bool name_success { project_table.emplace(PROJECT_NAME, cfg.name).second };
  const bool language_success { project_table.emplace(PROJECT_LANGUAGE, cfg::to_str(cfg.language)).second };
  const bool target_success { project_table.emplace(PROJECT_TARGET, cfg::to_str(cfg.target)).second };
  bool verbose_success { true };

  if (cfg.verbose.has_value()) {
    verbose_success = project_table.emplace(PROJECT_VERBOSE, cfg.verbose.value()).second;
  }

  if (!(name_success && language_success && target_success && verbose_success)) {
    throw std::runtime_error { "failed to create project toml::table" };
  }

  return project_table;
}


toml::table create_build_table(const cfg::Config& cfg) {
  toml::table build_table {};
  bool language_standard_success { true };
  bool compiler_success { true };
  bool compiler_flags_success { true };
  bool gnu_extensions_success { true };
  bool build_system_success { true };
  bool generator_success { true };
  bool build_type_success { true };

  if (cfg.language_standard.has_value()) {
    language_standard_success = build_table.emplace(BUILD_LANGUAGE_STD, cfg::to_str(cfg.language_standard.value())).second;
  }

  if (cfg.compiler.has_value()) {
    compiler_success = build_table.emplace(BUILD_COMPILER, cfg::to_str(cfg.compiler.value())).second;
  }

  if (cfg.compiler_flags.has_value()) {
    compiler_flags_success = build_table.emplace(BUILD_COMPILER_FLAGS, split_string(cfg.compiler_flags.value())).second;
  }

  if (cfg.gnu_extensions_enabled.has_value()) {
    gnu_extensions_success = build_table.emplace(BUILD_GNU_EXTENSIONS, cfg.gnu_extensions_enabled.value()).second;
  }

  if (cfg.build_system.has_value()) {
    build_system_success = build_table.emplace(BUILD_BUILD_SYSTEM, cfg::to_str(cfg.build_system.value())).second;
  }

  if (cfg.generator.has_value()) {
    generator_success = build_table.emplace(BUILD_GENERATOR, cfg::to_str(cfg.generator.value())).second;
  }

  if (cfg.build_type.has_value()) {
    build_type_success = build_table.emplace(BUILD_BUILD_TYPE, cfg::to_str(cfg.build_type.value())).second;
  }

  if (!(language_standard_success && compiler_success && compiler_flags_success && gnu_extensions_success && build_system_success && generator_success && build_type_success)) {
    throw std::runtime_error { "failed to create build toml::table" };
  }

  return build_table;
}


toml::table create_checks_table(const cfg::Config& cfg) {
  toml::table checks_table {};
  bool clang_format_success { true };
  bool language_server_success { true };
  bool clang_tidy_success { true };
  bool clang_tidy_args_success { true };
  bool cppcheck_success { true };
  bool cppcheck_checks_success { true };
  bool cppcheck_args_success { true };

  if (cfg.clang_format_style.has_value()) {
    clang_format_success = checks_table.emplace(CHECKS_CLANG_FORMAT_STYLE, cfg::to_str(cfg.clang_format_style.value())).second;
  }

  if (cfg.language_server.has_value()) {
    language_server_success = checks_table.emplace(CHECKS_LANGUAGE_SERVER, cfg.language_server.value()).second;
  }

  if (cfg.clang_tidy_enabled.has_value()) {
    clang_tidy_success = checks_table.emplace(CHECKS_CLANG_TIDY_ENABLED, cfg.clang_tidy_enabled.value()).second;
  }

  if (cfg.clang_tidy_args.has_value()) {
    clang_tidy_args_success = checks_table.emplace(CHECKS_CLANG_TIDY_ARGS, split_string(cfg.clang_tidy_args.value())).second;
  }

  if (cfg.cppcheck_enabled.has_value()) {
    cppcheck_success = checks_table.emplace(CHECKS_CPPCHECK_ENABLED, cfg.cppcheck_enabled.value()).second;
  }

  if (cfg.cppcheck_checks.has_value()) {
    cppcheck_checks_success = checks_table.emplace(CHECKS_CPPCHECK_CHECKS, split_string(cfg.cppcheck_checks.value())).second;
  }

  if (cfg.cppcheck_args.has_value()) {
    cppcheck_args_success = checks_table.emplace(CHECKS_CPPCHECK_ARGS, split_string(cfg.cppcheck_args.value())).second;
  }

  if (!(clang_format_success && language_server_success && clang_tidy_success && clang_tidy_args_success && cppcheck_success && cppcheck_checks_success && cppcheck_args_success)) {
    throw std::runtime_error { "failed to create checks toml::table" };
  }

  return checks_table;
}


toml::table create_tests_table(const cfg::Config& cfg) {
  toml::table tests_table {};
  bool testing_success { true };
  bool framework_success { true };

  if (cfg.testing_enabled.has_value()) {
    testing_success = tests_table.emplace(TESTS_TESTING_ENABLED, cfg.testing_enabled.value()).second;
  }

  if (cfg.test_framework.has_value()) {
    framework_success = tests_table.emplace(TESTS_TEST_FRAMEWORK, cfg::to_str(cfg.test_framework.value())).second;
  }

  if (!(testing_success && framework_success)) {
    throw std::runtime_error { "failed to create tests toml::table" };
  }

  return tests_table;
}


toml::table create_deps_table(const cfg::Config& cfg) {
  toml::table deps_table {};
  bool package_manager_success { true };
  bool build_types_success { true };

  if (cfg.package_manager.has_value()) {
    package_manager_success = deps_table.emplace(DEPS_PACKAGE_MANAGER, cfg::to_str(cfg.package_manager.value())).second;
  }

  toml::array build_types {};
  for (std::size_t i { 0 }; i < cfg.build_types.size(); ++i) {
    if (cfg.build_types.at(i).has_value()) {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      build_types.emplace_back(cfg::to_str(cfg.build_types.at(i).value()));
    }
  }
  if (!build_types.empty()) {
    build_types_success = deps_table.emplace(DEPS_BUILD_TYPES, build_types).second;
  }

  if (!(package_manager_success && build_types_success)) {
    throw std::runtime_error { "failed to create deps toml::table" };
  }

  return deps_table;
}

}  // namespace

namespace cfg {

Config Config::this_project() {
  return Config::from_file(utils::get_config_path());
}


Config Config::from_file(const std::filesystem::path& file) {
  if (!std::filesystem::exists(file)) {
    throw std::runtime_error { std::format("file not found: {}", file.string()) };
  }

  return validate_config(toml::parse_file(file.string()));
}


void Config::to_file(const std::filesystem::path& file) const {
  if (std::filesystem::exists(file)) {
    std::filesystem::remove(file);
  }
  std::ofstream cfg_stream(file, std::ios::app);
  if (!cfg_stream.is_open()) {
    throw std::runtime_error { std::format("failed to open file: {}", file.string()) };
  }

  toml::table table {};
  const bool project_success { table.emplace(PROJECT, create_project_table(*this)).second };

  const bool build_success { table.emplace(BUILD, create_build_table(*this)).second };

  const bool checks_success { table.emplace(CHECKS, create_checks_table(*this)).second };

  const bool tests_success { table.emplace(TESTS, create_tests_table(*this)).second };

  const bool deps_success { table.emplace(DEPS, create_deps_table(*this)).second };

  if (!(project_success && build_success && checks_success && tests_success && deps_success)) {
    throw std::runtime_error { "failed to create config toml::table" };
  }

  // The default << overload for toml::table sorts the fields alphabetically.
  // I want the tables to have a specific order in the file so I have to print it
  // manually.
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  cfg_stream << '[' << PROJECT << "]\n" << table[PROJECT] << "\n";
  if (!table[BUILD].as_table()->empty()) {
    cfg_stream << "\n[" << BUILD << "]\n" << table[BUILD] << "\n";
  }
  if (!table[CHECKS].as_table()->empty()) {
    cfg_stream << "\n[" << CHECKS << "]\n" << table[CHECKS] << "\n";
  }
  if (!table[TESTS].as_table()->empty()) {
    cfg_stream << "\n[" << TESTS << "]\n" << table[TESTS] << "\n";
  }
  if (!table[DEPS].as_table()->empty()) {
    cfg_stream << "\n[" << DEPS << "]\n" << table[DEPS] << "\n";
  }
  // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  cfg_stream << std::flush;
}


Compiler Config::get_compiler() const {
  if (compiler.has_value() && compiler.value() != Compiler::Default) {
    return compiler.value();
  }
  switch (language) {
    case Language::C:
      #if defined(__APPLE__)
        return Compiler::AppleClang;
      #elif defined(__linux__)
        return Compiler::Gcc;
      #endif
      break;
    case Language::Cpp:
      #if defined(__APPLE__)
        return Compiler::AppleClangPp;
      #elif defined(__linux__)
        return Compiler::Gpp;
      #endif
      break;
  }
}


Generator Config::get_generator() const {
  if (generator.has_value() && generator.value() != Generator::Default) {
    return generator.value();
  }
  #if defined(__APPLE__)
    return Generator::UnixMakefiles;
  #elif defined(__linux__)
    return Generator::UnixMakefiles;
  #endif
}


LanguageStandard Config::get_language_standard() const {
  if (language_standard.has_value()) {
    return language_standard.value();
  }

  switch(language) {
    case Language::C:
      return LanguageStandard::C23;
      break;
    case Language::Cpp:
      return LanguageStandard::Cpp23;
      break;
  }
}


BuildSystem Config::get_build_system() const {
  if (build_system.has_value()) {
    return build_system.value();
  }
  return BuildSystem::CMake;
}


BuildType Config::get_build_type() const {
  if (build_type.has_value()) {
    return build_type.value();
  }
  return BuildType::Debug;
}


const std::filesystem::path Config::get_build_dir() const {
  if (is_multi_config()) {
    return utils::get_root_dir() / "build";
  }
  return utils::get_root_dir() / "build" / cfg::to_str(get_build_type());
}


const std::string_view Config::get_header_extension() const {
  switch (language) {
    case Language::C:
      return "h";
      break;
    case Language::Cpp:
      return "hpp";
      break;
  }
}


const std::string_view Config::get_source_extension() const {
  switch (language) {
    case Language::C:
      return "c";
      break;
    case Language::Cpp:
      return "cpp";
      break;
  }
}


bool Config::is_multi_config() const {
  Generator tmp_generator {};
  if (generator.has_value()) {
    tmp_generator = generator.value();
  }
  else {
    tmp_generator = Generator::Default;
  }

  switch (tmp_generator) {
    case Generator::XCode:
    case Generator::NinjaMultiConfig:
      return true;
      break;
    default:
      return false;
      break;
  }
}


bool Config::are_gnu_extensions_enabled() const {
  return gnu_extensions_enabled.has_value() && gnu_extensions_enabled.value();
}


bool Config::is_testing_enabled() const {
  return testing_enabled.has_value() && testing_enabled.value();
}


bool Config::is_clang_tidy_enabled() const {
  return clang_tidy_enabled.has_value() && clang_tidy_enabled.value();
}


bool Config::is_cppcheck_enabled() const {
  return cppcheck_enabled.has_value() && cppcheck_enabled.value();
}


bool Config::is_verbose_enabled() const {
  return verbose.has_value() && verbose.value();
}

}
