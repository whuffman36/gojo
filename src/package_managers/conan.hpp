/// @file      src/conan.hpp
/// @brief     Functionality for installing dependencies and managing profiles
///            through the conan package manager.
/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#ifndef GOJO_SRC_PACKAGE_MANAGERS_CONAN_HPP
#define GOJO_SRC_PACKAGE_MANAGERS_CONAN_HPP

#include <string>
#include <string_view>

#include "src/cfg/config.hpp"

namespace conan {

class Profile {
   public:
    Profile() = default;
    Profile(Profile&& profile) = default;
    Profile& operator=(Profile&& profile) = default;
    ~Profile() = default;

    Profile(const Profile& profile) = delete;
    Profile& operator=(const Profile& profile) = delete;

    static Profile from_file(std::string_view profile_name);

    void to_file(const cfg::Config& cfg);

   private:
    std::string arch;
    std::string build_type;
    std::string compiler;
    std::string std;
    std::string lib;
    std::string compiler_version;
    std::string os;
    std::string other_settings;
};

void search(std::string_view package);

void install(const cfg::Config& cfg, bool verbose);

void uninstall(std::string_view package);

void add(const std::filesystem::path& conanfile, std::string_view package,
         std::string_view version = "latest");

void remove(const std::filesystem::path& conanfile, std::string_view package);

}  // namespace conan

#endif
