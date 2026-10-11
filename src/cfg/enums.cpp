#include "src/cfg/enums.hpp"

#include <string_view>


namespace cfg {

const std::string_view to_string(const Language language) {
  switch (language) {
    case Language::C:
      return "C";
      break;
    case Language::Cpp:
      return "C++";
      break;
  }
}


const std::string_view to_string(const LanguageStandard language_standard) {
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


const std::string_view to_string(const Compiler compiler) {
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
      return "/opt/homebrew/opt/gcc/bin/gcc-16";
      break;
  }
}


const std::string_view to_string(const Target target) {
  switch (target) {
    case Target::Binary:
      return "binary";
      break;
    case Target::Library:
      return "library";
      break;
  }
}


const std::string_view to_string(const BuildSystem build_system) {
  switch (build_system) {
    case BuildSystem::CMake:
      return "CMake";
      break;
  }
}


const std::string_view to_string(const Generator generator) {
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


const std::string_view to_string(const BuildType build_type) {
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


const std::string_view to_string(const PackageManager package_manager) {
  switch (package_manager) {
    case PackageManager::Conan:
      return "conan";
      break;
    case PackageManager::Vcpkg:
      return "vcpkg";
      break;
  }
}


const std::string_view to_string(const FormatStyle format_style) {
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


const std::string_view to_string(const TestFramework test_framework) {
  switch (test_framework) {
    case TestFramework::GoogleTest:
      return "GoogleTest";
      break;
    case TestFramework::Unity:
      return "Unity";
      break;
  }
}

};