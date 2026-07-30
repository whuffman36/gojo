#include "src/config.hpp"

#include <array>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "src/utils.hpp"


namespace {

// Valid values for config file labels.
constexpr std::array<std::string_view, 2> TARGETS       { "library", "binary" };
constexpr std::array<std::string_view, 2> LANGS         { "C++", "C" };
constexpr std::array<std::string_view, 5> STDS_CPP      { "11", "14", "17", "20", "23" };
constexpr std::array<std::string_view, 4> STDS_C        { "99", "11", "17", "23" };
constexpr std::array<std::string_view, 3> COMPILERS_CPP { "system default", "clang++", "g++" };
constexpr std::array<std::string_view, 3> COMPILERS_C   { "system default", "clang", "gcc" };
constexpr std::array<std::string_view, 1> BUILD_SYSTEMS { /*"Meson",*/ "CMake" };  // TODO: Add support for Meson
constexpr std::array<std::string_view, 13> GENERATORS {
  // Unix Makefiles for Linux, MacOS, Visual Studio X for Windows
  "system default",

  // Ninja
  "Ninja",
  "Ninja Multi-Config",

  // Makefiles
  "Borland Makefiles",
  "MSYS Makefiles",
  "MinGW Makefiles",
  "NMake Makefiles",
  "NMake Makefiles JOM",
  "Unix Makefiles",
  "Watcom WMake",

  // FASTBuild (??)
  "FASTBuild",

  // TODO: Add support for windows.
  // Visual Studio
  /*
  "Visual Studio 6",
  "Visual Studio 7",
  "Visual Studio 7 .NET 2003",
  "Visual Studio 8 2005",
  "Visual Studio 9 2008",
  "Visual Studio 10 2010",
  "Visual Studio 11 2012",
  "Visual Studio 12 2013",
  "Visual Studio 14 2015",
  "Visual Studio 15 2017",
  "Visual Studio 16 2019",
  "Visual Studio 17 2022",
  "Visual Studio 18 2026",
  */

  // XCode
  "XCode",

  // Green Hills (WTF is this?)
  "Green Hills MULTI"
};

constexpr std::array<std::string_view, 4> BUILD_TYPES  { "Debug", "Release", "RelWithDebInfo", "MinSizeRel" };
constexpr std::array<std::string_view, 3> PKG_MANAGERS { "conan", /*"vcpkg",*/ "none" };  // TODO: Add support for vcpkg
constexpr std::array<std::string_view, 4> SRC_EXTS_CPP { "cpp", "cc", "cxx", "c++" };
constexpr std::array<std::string_view, 1> SRC_EXTS_C   { "c" };
constexpr std::array<std::string_view, 4> HDR_EXTS_CPP { "hpp", "h", "hxx", "h++" };
constexpr std::array<std::string_view, 1> HDR_EXTS_C   { "h" };
constexpr std::array<std::string_view, 8> FMT_STYLES {
  "google",
  "llvm",
  "chromium",
  "mozilla",
  "webkit",
  "microsoft",
  "gnu",
  "custom"
};

constexpr std::array<std::string_view, 2> TEST_CPP   { "GoogleTest", "none" };
constexpr std::array<std::string_view, 2> TEST_C     { "Unity", "none" };
constexpr std::array<std::string_view, 2> TRUE_FALSE { "true", "false" };

// Config file labels.
constexpr std::string_view PROJECT_NAME   { "project name" };
constexpr std::string_view PROJECT_TARGET { "project target" };
constexpr std::string_view PROJECT_ROOT   { "project root directory" };
constexpr std::string_view EXE_NAME       { "executable name" };
constexpr std::string_view PROJECT_LANG   { "project language" };
constexpr std::string_view LANG_STD       { "language standard" };
constexpr std::string_view GNU_EXTENSIONS { "GNU extensions enabled" };
constexpr std::string_view COMPILER       { "compiler" };
constexpr std::string_view CFLAGS         { "compiler flags" };
constexpr std::string_view BUILD_SYS      { "build system" };
constexpr std::string_view GENERATOR      { "generator" };
constexpr std::string_view MULTI_CONFIG   { "multi-config generator" };
constexpr std::string_view BUILD_TYPE     { "build type" };
constexpr std::string_view PKG_MAN        { "package manager" };
constexpr std::string_view PKG_BUILD_TYPE { "dependency build types" };
constexpr std::string_view SRC_EXT        { "source file extension" };
constexpr std::string_view HDR_EXT        { "header file extension" };
constexpr std::string_view FMT_STYLE      { "clang-format style" };
constexpr std::string_view CLANGD         { "clangd enabled" };
constexpr std::string_view C_TIDY         { "clang-tidy enabled" };
constexpr std::string_view C_TIDY_ARGS    { "clang-tidy arguments" };
constexpr std::string_view CPPCHECK       { "cppcheck enabled" };
constexpr std::string_view CHECKS         { "cppcheck enabled checks" };
constexpr std::string_view CPPCHECK_ARGS  { "cppcheck arguments" };
constexpr std::string_view TESTS          { "testing enabled" };
constexpr std::string_view TEST_FW        { "testing framework" };
constexpr std::string_view GIT            { "git repository" };

// This is probably the worst part of the whole project. It's very hard
// to read this and check that each {} corresponds to the right label
// and field.
constexpr std::string_view cfg_file_template {
R"(---
# gojo config file
# Run 'gojo info' to learn more about this file.
# See https://github.com/gojocli/gojo for more info.

{}={}

# Options: binary, library
{}={}

{}={}

{}={}

# Options: C, C++
{}={}

# Options   (C): 99, 11, 17, 23
# Options (C++): 11, 14, 17, 20, 23
{}={}

# Options: true, false
{}={}

# Options   (C): system default, clang, gcc
# Options (C++): system default, clang++, g++
{}={}

{}={}

# Options: CMake
{}={}

# Options: system default, or see the list of CMake generators here:
# <insert link here>
{}={}

# Depends on generator defined above. Do not manually edit.
{}={}

# Options: Debug, Release, RelWithDebInfo, MinSizeRel
{}={}

# Options: none, conan, vcpkg
{}={}

# Options: Debug, Release, RelWithDebInfo, MinSizeRel
# Note: any of the above options can be combined in a comma separated list
{}={}

# Options   (C): c
# Options (C++): cpp, cc, cxx, c++
{}={}

# Options   (C): h
# Options (C++): hpp, h, hxx, h++
{}={}

# Options: google, llvm, chromium, mozilla, webkit, microsoft, gnu, custom
{}={}

# Options: true, false
{}={}

# Options: true, false
{}={}

{}={}

# Options: true, false
{}={}

# Options: warning, style, performance, portability, information,
#          unusedFunction, missingInclude, all
# Note: any of the above options can be combined in a comma separated list
# Ex: warning,portability,missingInclude
{}={}

{}={}

# Options: true, false
{}={}

# Options   (C): Unity, none
# Options (C++): GoogleTest, none
{}={}

# Options: true, false
{}={}
)"
};

struct ValidationResult {
  std::string msg;
  bool success;
};

// This could be done using maps for the valid values, but since the sets of
// valid values are quite small, it is likely faster to just use arrays.
ValidationResult
validate_config_values(std::string_view label,
                       std::string_view value,
                       std::span<const std::string_view> values) {
  std::string out {
    std::format("label '{}' must be one of the following values:\n\t{{ ",
                label)
  };
  for (const auto item : values) {
    if (value == item) {
      return ValidationResult { .msg={}, .success=true };
    }
    out.append(item);
    out.append(", ");
  }
  out.append(" }");
  return ValidationResult { .msg=std::move(out), .success=false };
}

}  // namespace


namespace config {

bool is_multi_config(std::string_view generator) {
  if (generator == "XCode") {
    return true;
  }
  if (generator == "Ninja Multi-Config") {
    return true;
  }
  if (generator.starts_with("Visual Studio")) {
    return true;
  }
  // TODO: Add support for windows.
  /*
  if (generator == "system default") {
    // Windows visual studio defaults to using a multi-config
    // setup. Apple does not default to XCode, but rather Makefiles.
    #if defined (_WIN32) || defined (_WIN64)
      return true;
    #else
      return false;
    #endif
  }*/
  return false;
}


std::expected<GojoConfig, std::string>
read_from_file(const std::filesystem::path& cfg_file) {
  if (!std::filesystem::exists(cfg_file)) {
    return std::unexpected {
      std::format("file not found: {}", cfg_file.string())
    };
  }

  std::ifstream cfg_stream(cfg_file);
  if (!cfg_stream.is_open()) {
    return std::unexpected {
      std::format("failed to open file: {}", cfg_file.string())
    };
  }

  GojoConfig cfg { DEFAULT };
  std::string line {};
  std::string label {};
  std::string value {};
  ValidationResult result {};

  while (std::getline(cfg_stream, line)) {
    // Ignore comments and newlines.
    if (line.starts_with("#") || line.empty() || line == "---") {
      continue;
    }

    // Split line by '='.
    std::stringstream ss { line };
    std::getline(ss, label, '=');

    if (label == PROJECT_NAME) {
      std::getline(ss, cfg.project_name);
      if (cfg.project_name.empty()) {
        result.msg = std::format("label '{}' must not be empty", PROJECT_NAME);
        result.success = false;
      }
      else {
        result.success = true;
      }
    }
    else if (label == PROJECT_TARGET) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TARGETS);
      cfg.is_library = value == "library" ? true : false;
    }
    else if (label == PROJECT_ROOT){
      std::getline(ss, cfg.project_root);
      if (cfg.project_root.empty()) {
        result.msg = std::format("label '{}' must not be empty", PROJECT_ROOT);
        result.success = false;
      }
      else {
        result.success = true;
      }
      if (cfg.project_root == ".") {
        cfg.project_root = std::filesystem::current_path().string();
      }
    }
    else if (label == PROJECT_LANG) {
      std::getline(ss, cfg.project_lang);
      result = validate_config_values(label, cfg.project_lang, LANGS);
    }
    else if (label == EXE_NAME) {
      std::getline(ss, cfg.executable_name);
      if (cfg.executable_name.empty()) {
        result.msg = std::format("label '{}' must not be empty", EXE_NAME);
        result.success = false;
      }
      else {
        result.success = true;
      }
    }
    else if (label == LANG_STD) {
      std::getline(ss, value);
      if (cfg.project_lang == "C++") {
        result = validate_config_values(label, value, STDS_CPP);
      }
      if (cfg.project_lang == "C") {
        result = validate_config_values(label, value, STDS_C);
      }
      cfg.lang_standard = std::stoi(value);
    }
    else if (label == GNU_EXTENSIONS) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.gnu_extensions = value == "true" ? true : false;
    }
    else if (label == COMPILER) {
      std::getline(ss, cfg.compiler);
      if (cfg.project_lang == "C++") {
        result = validate_config_values(label, cfg.compiler, COMPILERS_CPP);
      }
      if (cfg.project_lang == "C") {
        result = validate_config_values(label, cfg.compiler, COMPILERS_C);
      }
    }
    else if (label == CFLAGS) {
      // Way too many flags to validate.
      std::getline(ss, cfg.compiler_flags);
      result.success = true;
    }
    else if (label == BUILD_SYS) {
      std::getline(ss, cfg.build_system);
      result = validate_config_values(label, cfg.build_system, BUILD_SYSTEMS);
    }
    else if (label == GENERATOR) {
      std::getline(ss, cfg.generator);
      result = validate_config_values(label, cfg.generator, GENERATORS);
    }
    else if (label == MULTI_CONFIG) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.multi_config = value == "true" ? true : false;
    }
    else if (label == BUILD_TYPE) {
      std::getline(ss, cfg.build_type);
      result = validate_config_values(label, cfg.build_type, BUILD_TYPES);
    }
    else if (label == PKG_MAN) {
      std::getline(ss, cfg.pkg_manager);
      result = validate_config_values(label, cfg.pkg_manager, PKG_MANAGERS);
    }
    else if (label == PKG_BUILD_TYPE) {
      if (cfg.pkg_manager == "none") {
        cfg.dep_build_types = "";
        continue;
      }
      
      std::getline(ss, cfg.dep_build_types);
      if (cfg.dep_build_types.empty()) {
        cfg.dep_build_types = "Debug,Release";
        result.success = true;
      }
      else {
        std::stringstream ss2 { cfg.dep_build_types };
        std::string build_type {};
        while (std::getline(ss2, build_type, ',')) {
          result = validate_config_values(PKG_BUILD_TYPE, build_type, BUILD_TYPES);
        }
      }
    }
    else if (label == SRC_EXT) {
      std::getline(ss, cfg.src_ext);
      if (cfg.project_lang == "C++") {
        result = validate_config_values(label, cfg.src_ext, SRC_EXTS_CPP);
      }
      if (cfg.project_lang == "C") {
        result = validate_config_values(label, cfg.src_ext, SRC_EXTS_C);
      }
    }
    else if (label == HDR_EXT) {
      std::getline(ss, cfg.hdr_ext);
      if (cfg.project_lang == "C++") {
        result = validate_config_values(label, cfg.hdr_ext, HDR_EXTS_CPP);
      }
      if (cfg.project_lang == "C") {
        result = validate_config_values(label, cfg.hdr_ext, HDR_EXTS_C);
      }
    }
    else if (label == FMT_STYLE) {
      std::getline(ss, cfg.fmt_style);
      result = validate_config_values(label, cfg.fmt_style, FMT_STYLES);
    }
    else if (label == CLANGD) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.clangd_enabled = value == "true" ? true : false;
    }
    else if (label == C_TIDY) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.clang_tidy_enabled = value == "true" ? true : false;
    }
    else if (label == C_TIDY_ARGS) {
      std::getline(ss, cfg.clang_tidy_args);
      result.success = true;
    }
    else if (label == CPPCHECK) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.cppcheck_enabled = value == "true" ? true : false;
    }
    else if (label == CHECKS) {
      // There are too many combinations to check them all here,
      // just have to wait for a call to cppcheck to see if they
      // are valid.
      std::getline(ss, cfg.cppcheck_checks);
      result.success = true;
    }
    else if (label == CPPCHECK_ARGS) {
      std::getline(ss, cfg.cppcheck_args);
      result.success = true;
    }
    else if (label == TESTS) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.testing_enabled = value == "true" ? true : false;
    }
    else if (label == TEST_FW) {
      std::getline(ss, cfg.testing_framework);
      if (cfg.project_lang == "C++") {
        result = validate_config_values(label,
                                        cfg.testing_framework,
                                        TEST_CPP);
      }
      if (cfg.project_lang == "C") {
        result = validate_config_values(label, cfg.testing_framework, TEST_C);
      }
    }
    else if (label == GIT) {
      std::getline(ss, value);
      result = validate_config_values(label, value, TRUE_FALSE);
      cfg.git_repo = value == "true" ? true : false;
    }
    else {
      return std::unexpected {
        std::format("unrecognized label in gojo config file: {}", label)
      };
    }

    if (!result.success) {
      return std::unexpected { std::move(result.msg) };
    }
  }

  cfg.multi_config = is_multi_config(cfg.generator);

  return cfg;
}


std::optional<std::string> write_to_file(const GojoConfig& cfg,
                                         const std::filesystem::path& file) {
  std::filesystem::path cfg_file {};
  if (!file.empty()) {
    cfg_file = file;
  }
  else {
    cfg_file = std::filesystem::path {
      std::format("{}/.gojo", cfg.project_root)
    };
  }
  std::ofstream cfg_stream(cfg_file);
  if (!cfg_stream.is_open()) {
    return std::make_optional(
      std::format("failed to open file: {}", cfg_file.string())
    );
  }

  const std::string new_cfg { std::format(cfg_file_template,
    PROJECT_NAME, cfg.project_name,
    PROJECT_TARGET, cfg.is_library ? "library" : "binary",
    PROJECT_ROOT, cfg.project_root,
    EXE_NAME, cfg.executable_name,
    PROJECT_LANG, cfg.project_lang,
    LANG_STD, cfg.lang_standard,
    GNU_EXTENSIONS, cfg.gnu_extensions ? "true" : "false",
    COMPILER, cfg.compiler,
    CFLAGS, cfg.compiler_flags,
    BUILD_SYS, cfg.build_system,
    GENERATOR, cfg.generator,
    MULTI_CONFIG, cfg.multi_config ? "true" : "false",
    BUILD_TYPE, cfg.build_type,
    PKG_MAN, cfg.pkg_manager,
    PKG_BUILD_TYPE, cfg.dep_build_types,
    SRC_EXT, cfg.src_ext,
    HDR_EXT, cfg.hdr_ext,
    FMT_STYLE, cfg.fmt_style,
    CLANGD, cfg.clangd_enabled ? "true" : "false",
    C_TIDY, cfg.clang_tidy_enabled ? "true" : "false",
    C_TIDY_ARGS, cfg.clang_tidy_args,
    CPPCHECK, cfg.cppcheck_enabled ? "true" : "false",
    CHECKS, cfg.cppcheck_checks,
    CPPCHECK_ARGS, cfg.cppcheck_args,
    TESTS, cfg.testing_enabled ? "true" : "false",
    TEST_FW, cfg.testing_framework,
    GIT, cfg.git_repo ? "true" : "false"
  )};

  cfg_stream << new_cfg << std::flush;

  return std::nullopt;
}


std::optional<std::string>
create_template(const std::filesystem::path& project_root) {
  if (!std::filesystem::exists(project_root)) {
    if (!std::filesystem::create_directories(project_root)) {
      return std::make_optional(
        std::format("failed to create path: {}", project_root.string())
      );
    }
  }
  const std::filesystem::path cfg_file { project_root / ".gojo" };
  std::ofstream cfg_stream(cfg_file);
  if (!cfg_stream.is_open()) {
    return std::make_optional(
      std::format("failed to open file: {}", cfg_file.string())
    );
  }

  const std::string cfg_template { std::format(cfg_file_template,
    PROJECT_NAME, "",
    PROJECT_TARGET, "",
    PROJECT_ROOT, "",
    EXE_NAME, "",
    PROJECT_LANG, "",
    LANG_STD, "",
    GNU_EXTENSIONS, "",
    COMPILER, "",
    CFLAGS, "",
    BUILD_SYS, "",
    GENERATOR, "",
    MULTI_CONFIG, "",
    BUILD_TYPE, "",
    PKG_MAN, "",
    PKG_BUILD_TYPE, "",
    SRC_EXT, "",
    HDR_EXT, "",
    FMT_STYLE, "",
    CLANGD, "",
    C_TIDY, "",
    C_TIDY_ARGS, "",
    CPPCHECK, "",
    CHECKS, "",
    CPPCHECK_ARGS, "",
    TESTS, "",
    TEST_FW, "",
    GIT, ""
  )};

  cfg_stream << cfg_template << std::flush;

  return std::nullopt;
}


std::optional<std::string> create_profile(std::string_view profile_name) {
  auto read_result {
    read_from_file(std::filesystem::current_path() / ".gojo")
  };
  if (!read_result.has_value()) {
    return std::make_optional(std::move(read_result.error()));
  }

  GojoConfig cfg { std::move(read_result.value()) };

  // Strip out project specific strings for the profile.
  cfg.project_name = "TODO: replace";
  cfg.project_root = "TODO: replace";
  cfg.executable_name = "TODO: replace";

  const std::filesystem::path profile {
    std::format("{}/{}", utils::get_profiles_dir(), profile_name)
  };

  auto result { write_to_file(cfg, profile) };
  if (result.has_value()) {
    return result;
  }

  return std::nullopt;
}


std::expected<GojoConfig, std::string>
read_profile(std::string_view profile_name) {
  const std::filesystem::path profile {
    std::format("{}/{}", utils::get_profiles_dir(), profile_name)
  };

  return read_from_file(profile);
}

}  // namespace config
