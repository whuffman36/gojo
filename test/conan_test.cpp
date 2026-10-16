#include "src/package_managers/conan.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/utils.hpp"
#include "test/utils.hpp"

namespace {

constexpr std::string_view TEST_PROFILE_CONTENTS {
    R"([settings]
arch=armv8
build_type=Release
compiler=clang
compiler.cppstd=gnu23
compiler.libcxx=libc++
compiler.version=22
os=Macos)"
};

const std::filesystem::path get_profiles_dir() {
    static const std::filesystem::path profile_dir { utils::get_home_dir() /
                                                     ".conan2" / "profiles" };

    return std::filesystem::absolute(profile_dir);
}

}  // namespace

TEST(ConanTest, ProfileFromFile) {
    const std::filesystem::path profile_path { get_profiles_dir() };
    EXPECT_TRUE(std::filesystem::exists(profile_path));
    std::ofstream profile_file { profile_path / "test" };
    EXPECT_TRUE(profile_file.is_open());
    profile_file << TEST_PROFILE_CONTENTS << std::flush;
    conan::Profile profile {};
    EXPECT_NO_THROW(profile = conan::Profile::from_file("test"));
    EXPECT_TRUE(std::filesystem::remove(profile_path / "test"));
}

TEST(ConanTest, ProfileToFile) {
    cfg::Config cfg {};
    EXPECT_NO_THROW(
        cfg = cfg::Config::from_file(get_config("config_minimum.toml"))
    );

    EXPECT_TRUE(std::filesystem::exists(get_profiles_dir()));

    conan::Profile profile {};
    EXPECT_NO_THROW(profile = conan::Profile::from_file("default"));

    EXPECT_NO_THROW(profile.to_file(cfg));
}
