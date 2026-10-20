#ifndef GOJO_SRC_CFG_ENUMS_HPP
#define GOJO_SRC_CFG_ENUMS_HPP

#include <cstdint>
#include <optional>
#include <string_view>

namespace cfg {

// Language
enum class Language : std::uint8_t {
    Cpp,
    C
};

constexpr std::uint8_t LANGUAGE_OPTIONS { 2 };

std::optional<Language> language_from_str(std::string_view str);

const std::string_view to_str(const Language language);

// Language Standard
enum class LanguageStandard : std::uint8_t {
    C99,
    C11,
    C17,
    C23,
    Cpp11,
    Cpp14,
    Cpp17,
    Cpp20,
    Cpp23,
    Cpp26
};

constexpr std::uint8_t LANGUAGE_STD_OPTIONS { 10 };

std::optional<LanguageStandard> language_std_from_int(std::uint8_t val,
                                                      Language language);

const std::string_view to_str(const LanguageStandard language_standard);

// Compiler
enum class Compiler : std::uint8_t {
    Default,
    Clang,
    ClangPp,
    AppleClang,
    AppleClangPp,
    HomebrewClang,
    HomebrewClangPp,
    Gcc,
    Gpp,
    HomebrewGcc,
    HomebrewGpp
};

constexpr std::uint8_t COMPILER_OPTIONS { 11 };

std::optional<Compiler> compiler_from_str(std::string_view str);

const std::string_view to_str(const Compiler compiler);

const std::string_view to_absolute_path(const Compiler compiler);

// Target
enum class Target : std::uint8_t {
    Binary,
    Library,
};

constexpr std::uint8_t TARGET_OPTIONS { 2 };

std::optional<Target> target_from_str(std::string_view str);

const std::string_view to_str(const Target target);

// Build System
enum class BuildSystem : std::uint8_t {
    CMake,
};

constexpr std::uint8_t BUILD_SYSTEM_OPTIONS { 1 };

std::optional<BuildSystem> build_system_from_str(std::string_view str);

const std::string_view to_str(const BuildSystem build_system);

// Generator
enum class Generator : std::uint8_t {
    Default,
    Ninja,
    NinjaMultiConfig,

    BorlandMakefiles,
    MSYSMakefiles,
    MinGWMakefiles,
    NMakeMakefiles,
    NMakeMakefilesJOM,
    UnixMakefiles,
    WatcomWMake,

    // ??
    FASTBuild,

    VisualStudio,
    VisualStudio6,
    VisualStudio7,
    VisualStudio7_NET_2003,
    VisualStudio8_2005,
    VisualStudio9_2008,
    VisualStudio10_2010,
    VisualStudio11_2012,
    VisualStudio12_2013,
    VisualStudio14_2015,
    VisualStudio15_2017,
    VisualStudio16_2019,
    VisualStudio17_2022,
    VisualStudio18_2026,

    XCode,

    // WTF is this?
    GreenHillsMULTI,
};

constexpr std::uint8_t GENERATOR_OPTIONS { 27 };

std::optional<Generator> generator_from_str(std::string_view str);

const std::string_view to_str(const Generator generator);

// Build Type
enum class BuildType : std::uint8_t {
    Debug,
    Release,
    RelWithDebInfo,
    MinSizeRel,
};

constexpr std::uint8_t BUILD_TYPE_OPTIONS { 4 };

std::optional<BuildType> build_type_from_str(std::string_view str);

const std::string_view to_str(const BuildType build_type);

// Package Manager
enum class PackageManager : std::uint8_t {
    Conan,
    Vcpkg,
};

constexpr std::uint8_t PACKAGE_MANAGER_OPTIONS { 2 };

std::optional<PackageManager> package_manager_from_str(std::string_view str);

const std::string_view to_str(const PackageManager package_manager);

// Format Style
enum class FormatStyle : std::uint8_t {
    Custom,
    Google,
    Llvm,
    Chromium,
    Mozilla,
    Webkit,
    Microsoft,
    Gnu,
};

constexpr std::uint8_t FORMAT_STYLE_OPTIONS { 8 };

std::optional<FormatStyle> fromat_style_from_str(std::string_view str);

const std::string_view to_str(const FormatStyle format_style);

// Test Framework
enum class TestFramework : std::uint8_t {
    Unity,
    GoogleTest,
};

constexpr std::uint8_t TEST_FRAMEWORK_OPTIONS { 2 };

std::optional<TestFramework> test_framework_from_str(std::string_view str);

const std::string_view to_str(const TestFramework test_framework);

};  // namespace cfg

#endif
