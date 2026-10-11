#ifndef GOJO_SRC_CONFIG_ENUMS_HPP
#define GOJO_SRC_CONFIG_ENUMS_HPP

#include <cstdint>
#include <flat_map>
#include <string_view>


namespace cfg {

enum class Language : std::uint8_t {
  C,
  Cpp
};

const std::flat_map<std::string_view, Language> language_map {
  { "C", Language::C },
  { "C++", Language::Cpp }
};

const std::string_view to_string(const Language language);



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

const std::flat_map<std::uint8_t, LanguageStandard> c_language_standard_map {
  { 99, LanguageStandard::C99 },
  { 11, LanguageStandard::C11 },
  { 17, LanguageStandard::C17 },
  { 23, LanguageStandard::C23 }
};

const std::flat_map<std::uint8_t, LanguageStandard> cpp_language_standard_map {
  { 11, LanguageStandard::Cpp11 },
  { 14, LanguageStandard::Cpp14 },
  { 17, LanguageStandard::Cpp17 },
  { 20, LanguageStandard::Cpp20 },
  { 23, LanguageStandard::Cpp23 },
  { 26, LanguageStandard::Cpp26 }
};

const std::string_view to_string(const LanguageStandard language_standard);



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

const std::flat_map<std::string_view, Compiler> compiler_map {
  { "default", Compiler::Default },
  { "clang", Compiler::Clang },
  { "/usr/bin/clang", Compiler::Clang },
  { "clang++", Compiler::ClangPp },
  { "/usr/bin/clang++", Compiler::ClangPp },
  { "apple-clang", Compiler::AppleClang },
  { "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang", Compiler::AppleClang },
  { "/Library/Developer/CommandLineTools/usr/bin/clang", Compiler::AppleClang },
  { "apple-clang++", Compiler::AppleClangPp },
  { "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++", Compiler::AppleClangPp },
  { "/Library/Developer/CommandLineTools/usr/bin/clang++", Compiler::AppleClangPp },
  { "homebrew-clang", Compiler::HomebrewClang },
  { "/opt/homebrew/opt/llvm/bin/clang", Compiler::HomebrewClang },
  { "homebrew-clang++", Compiler::HomebrewClangPp },
  { "/opt/homebrew/opt/llvm/bin/clang++", Compiler::HomebrewClangPp },
  { "gcc", Compiler::Gcc },
  { "/usr/bin/gcc", Compiler::Gcc },
  { "g++", Compiler::Gpp },
  { "/usr/bin/g++", Compiler::Gpp },
  { "homebrew-gcc", Compiler::HomebrewGcc },
  { "/opt/homebrew/opt/gcc/bin/gcc-16", Compiler::HomebrewGcc },
  { "homebrew-g++", Compiler::HomebrewGpp },
  { "/opt/homebrew/opt/gcc/bin/g++-16", Compiler::HomebrewGcc },
};

const std::string_view to_string(const Compiler compiler);
const std::string_view to_absolute_path(const Compiler compiler);



enum class Target : std::uint8_t {
  Binary,
  Library,
};

const std::flat_map<std::string_view, Target> target_map {
  { "binary", Target::Binary },
  { "bin", Target::Binary },
  { "library", Target::Library },
  { "lib", Target::Library }
};

const std::string_view to_string(const Target target);



enum class BuildSystem : std::uint8_t {
  CMake,
};

const std::flat_map<std::string_view, BuildSystem> build_system_map {
  { "CMake", BuildSystem::CMake },
  { "cmake", BuildSystem::CMake },
};

const std::string_view to_string(const BuildSystem build_system);



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

const std::flat_map<std::string_view, Generator> generator_map {
  { "default", Generator::Default },
  { "Ninja", Generator::Ninja },
  { "Ninja Multi-Config", Generator::NinjaMultiConfig },
  { "BorlandMakefiles", Generator::BorlandMakefiles },
  { "MSYSMakefiles", Generator::MSYSMakefiles },
  { "MinGWMakefiles", Generator::MinGWMakefiles },
  { "NMakeMakefiles", Generator::NMakeMakefiles },
  { "NMakeMakefilesJOM", Generator::NMakeMakefilesJOM },
  { "UnixMakefiles", Generator::UnixMakefiles },
  { "WatcomWMake", Generator::WatcomWMake },
  { "FASTBuild", Generator::FASTBuild },
  { "XCode", Generator::XCode },
  { "GreenHillsMULTI", Generator::GreenHillsMULTI }
};

const std::string_view to_string(const Generator generator);



enum class BuildType : std::uint8_t {
  Debug,
  Release,
  RelWithDebInfo,
  MinSizeRel,
};

const std::flat_map<std::string_view, BuildType> build_type_map {
  { "Debug", BuildType::Debug },
  { "debug", BuildType::Debug },
  { "Release", BuildType::Release },
  { "release", BuildType::Release },
  { "RelWithDebInfo", BuildType::RelWithDebInfo },
  { "MinSizeRel", BuildType::MinSizeRel }
};

const std::string_view to_string(const BuildType build_type);



enum class PackageManager : std::uint8_t {
  Conan,
  Vcpkg,
};

const std::flat_map<std::string_view, PackageManager> package_manager_map {
  { "conan", PackageManager::Conan },
  { "Conan", PackageManager::Conan },
  { "vcpkg", PackageManager::Vcpkg },
  { "Vcpkg", PackageManager::Vcpkg },
};

const std::string_view to_string(const PackageManager package_manager);



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

const std::flat_map<std::string_view, FormatStyle> format_style_map {
  { "custom", FormatStyle::Custom },
  { "google", FormatStyle::Google },
  { "llvm", FormatStyle::Llvm },
  { "chromium", FormatStyle::Chromium },
  { "mozilla", FormatStyle::Mozilla },
  { "webkit", FormatStyle::Webkit },
  { "microsoft", FormatStyle::Microsoft },
  { "gnu", FormatStyle::Gnu }
};

const std::string_view to_string(const FormatStyle format_style);



enum class TestFramework : std::uint8_t {
  Unity,
  GoogleTest,
};

const std::flat_map<std::string_view, TestFramework> test_framework_map {
  { "Unity", TestFramework::Unity },
  { "unity", TestFramework::Unity },
  { "GoogleTest", TestFramework::GoogleTest },
  { "googletest", TestFramework::GoogleTest },
};

const std::string_view to_string(const TestFramework test_framework);

};

#endif
