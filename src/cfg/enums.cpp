#include "src/cfg/enums.hpp"

#include <cstdint>
#include <flat_map>
#include <optional>
#include <string_view>


namespace cfg {

// Language
std::optional<Language> language_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, Language> map {
    { "C", Language::C },
    { "C++", Language::Cpp }
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const Language language) {
  switch (language) {
    case Language::C:
      return "C";
      break;
    case Language::Cpp:
      return "C++";
      break;
  }
}


// Language Standard
std::optional<LanguageStandard> language_std_from_int(std::uint8_t val, Language language) {
  static const std::flat_map<std::uint8_t, LanguageStandard> c_map {
    { 99, LanguageStandard::C99 },
    { 11, LanguageStandard::C11 },
    { 17, LanguageStandard::C17 },
    { 23, LanguageStandard::C23 }
  };

  static const std::flat_map<std::uint8_t, LanguageStandard> cpp_map {
    { 11, LanguageStandard::Cpp11 },
    { 14, LanguageStandard::Cpp14 },
    { 17, LanguageStandard::Cpp17 },
    { 20, LanguageStandard::Cpp20 },
    { 23, LanguageStandard::Cpp23 },
    { 26, LanguageStandard::Cpp26 }
  };

  switch (language) {
    case cfg::Language::C: {
      const auto result { c_map.find(val) };
      if (result == c_map.end()) {
        return std::nullopt;
      }
      return result->second;
      break;
    }
    case cfg::Language::Cpp: {
      const auto result { cpp_map.find(val) };
      if (result == cpp_map.end()) {
        return std::nullopt;
      }
      return result->second;
      break;
    }
  }
}

const std::string_view to_str(const LanguageStandard language_standard) {
  switch (language_standard) {
    case LanguageStandard::C99:
      return "99";
      break;
    case LanguageStandard::C11:
      return "11";
      break;
    case LanguageStandard::C17:
      return "17";
      break;
    case LanguageStandard::C23:
      return "23";
      break;
    case LanguageStandard::Cpp11:
      return "11";
      break;
    case LanguageStandard::Cpp14:
      return "14";
      break;
    case LanguageStandard::Cpp17:
      return "17";
      break;
    case LanguageStandard::Cpp20:
      return "20";
      break;
    case LanguageStandard::Cpp23:
      return "23";
      break;
    case LanguageStandard::Cpp26:
      return "26";
      break;
  }
}


// Compiler
std::optional<Compiler> compiler_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, Compiler> map {
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

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const Compiler compiler) {
  switch (compiler) {
    case Compiler::Default:
      return "default";
      break;
    case Compiler::Clang:
      return "clang";
      break;
    case Compiler::ClangPp:
      return "clang++";
      break;
    case Compiler::AppleClang:
      return "apple-clang";
      break;
    case Compiler::AppleClangPp:
      return "apple-clang++";
      break;
    case Compiler::HomebrewClang:
      return "homebrew-clang";
      break;
    case Compiler::HomebrewClangPp:
      return "homebrew-clang++";
      break;
    case Compiler::Gcc:
      return "gcc";
      break;
    case Compiler::Gpp:
      return "g++";
      break;
    case Compiler::HomebrewGcc:
      return "homebrew-gcc";
      break;
    case Compiler::HomebrewGpp:
      return "homebrew-g++";
      break;
  }
}


const std::string_view to_absolute_path(const Compiler compiler) {
  switch (compiler) {
    case Compiler::Default:
      return "default";
      break;
    case Compiler::Clang:
      return "/usr/bin/clang";
      break;
    case Compiler::ClangPp:
      return "/usr/bin/clang++";
      break;
    case Compiler::AppleClang:
      return "/Library/Developer/CommandLineTools/usr/bin/clang";
      break;
    case Compiler::AppleClangPp:
      return "/Library/Developer/CommandLineTools/usr/bin/clang++";
      break;
    case Compiler::HomebrewClang:
      return "/opt/homebrew/opt/llvm/bin/clang";
      break;
    case Compiler::HomebrewClangPp:
      return "/opt/homebrew/opt/llvm/bin/clang++";
      break;
    case Compiler::Gcc:
      return "/usr/bin/gcc";
      break;
    case Compiler::Gpp:
      return "/usr/bin/g++";
      break;
    case Compiler::HomebrewGcc:
      return "/opt/homebrew/opt/gcc/bin/gcc-16";
      break;
    case Compiler::HomebrewGpp:
      return "/opt/homebrew/opt/gcc/bin/g++-16";
      break;
  }
}


// Target
std::optional<Target> target_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, Target> map {
    { "binary", Target::Binary },
    { "bin", Target::Binary },
    { "library", Target::Library },
    { "lib", Target::Library }
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const Target target) {
  switch (target) {
    case Target::Binary:
      return "binary";
      break;
    case Target::Library:
      return "library";
      break;
  }
}


// Build System
std::optional<BuildSystem> build_system_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, BuildSystem> map {
    { "CMake", BuildSystem::CMake },
    { "cmake", BuildSystem::CMake },
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const BuildSystem build_system) {
  switch (build_system) {
    case BuildSystem::CMake:
      return "CMake";
      break;
  }
}


// Generator
std::optional<Generator> generator_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, Generator> map {
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

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const Generator generator) {
  switch (generator) {
    case Generator::Default:
      return "default";
      break;
    case Generator::Ninja:
      return "Ninja";
      break;
    case Generator::NinjaMultiConfig:
      return "Ninja Multi-Config";
      break;
    case Generator::BorlandMakefiles:
      return "Borland Makefiles";
      break;
    case Generator::MSYSMakefiles:
      return "MSYS Makefiles";
      break;
    case Generator::MinGWMakefiles:
      return "MinGW Makefiles";
      break;
    case Generator::NMakeMakefiles:
      return "NMake Makefiles";
      break;
    case Generator::NMakeMakefilesJOM:
      return "NMake Makefiles JOM";
      break;
    case Generator::UnixMakefiles:
      return "Unix Makefiles";
      break;
    case Generator::WatcomWMake:
      return "Watcom WMake";
      break;
    case Generator::FASTBuild:
      return "FASTBuild";
      break;
    case Generator::XCode:
      return "XCode";
      break;
    case Generator::GreenHillsMULTI:
      return "GreenHillsMULTI";
      break;
    default:
      return "default";
      break;
  }
}


// Build Type
std::optional<BuildType> build_type_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, BuildType> map {
    { "Debug", BuildType::Debug },
    { "debug", BuildType::Debug },
    { "Release", BuildType::Release },
    { "release", BuildType::Release },
    { "RelWithDebInfo", BuildType::RelWithDebInfo },
    { "MinSizeRel", BuildType::MinSizeRel }
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const BuildType build_type) {
  switch (build_type) {
    case BuildType::Debug:
      return "Debug";
      break;
    case BuildType::Release:
      return "Release";
      break;
    case BuildType::RelWithDebInfo:
      return "RelWithDebInfo";
      break;
    case BuildType::MinSizeRel:
      return "MinSizeRel";
      break;
  }
}


// Package Manager
std::optional<PackageManager> package_manager_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, PackageManager> map {
    { "conan", PackageManager::Conan },
    { "Conan", PackageManager::Conan },
    { "vcpkg", PackageManager::Vcpkg },
    { "Vcpkg", PackageManager::Vcpkg },
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const PackageManager package_manager) {
  switch (package_manager) {
    case PackageManager::Conan:
      return "conan";
      break;
    case PackageManager::Vcpkg:
      return "vcpkg";
      break;
  }
}


// Format Style
std::optional<FormatStyle> fromat_style_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, FormatStyle> map {
    { "custom", FormatStyle::Custom },
    { "google", FormatStyle::Google },
    { "llvm", FormatStyle::Llvm },
    { "chromium", FormatStyle::Chromium },
    { "mozilla", FormatStyle::Mozilla },
    { "webkit", FormatStyle::Webkit },
    { "microsoft", FormatStyle::Microsoft },
    { "gnu", FormatStyle::Gnu }
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const FormatStyle format_style) {
  switch (format_style) {
    case FormatStyle::Custom:
      return "custom";
      break;
    case FormatStyle::Google:
      return "google";
      break;
    case FormatStyle::Llvm:
      return "llvm";
      break;
    case FormatStyle::Chromium:
      return "chromium";
      break;
    case FormatStyle::Mozilla:
      return "mozilla";
      break;
    case FormatStyle::Webkit:
      return "webkit";
      break;
    case FormatStyle::Microsoft:
      return "microsoft";
      break;
    case FormatStyle::Gnu:
      return "gnu";
      break;
  }
}


// Test Framework
std::optional<TestFramework> test_framework_from_str(std::string_view str) {
  static const std::flat_map<std::string_view, TestFramework> map {
    { "Unity", TestFramework::Unity },
    { "unity", TestFramework::Unity },
    { "GoogleTest", TestFramework::GoogleTest },
    { "googletest", TestFramework::GoogleTest },
  };

  const auto result { map.find(str) };
  if (result == map.end()) {
    return std::nullopt;
  }

  return result->second;
}


const std::string_view to_str(const TestFramework test_framework) {
  switch (test_framework) {
    case TestFramework::GoogleTest:
      return "GoogleTest";
      break;
    case TestFramework::Unity:
      return "Unity";
      break;
  }
}

}
