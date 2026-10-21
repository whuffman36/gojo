#ifndef GOJO_TEST_UTILS_HPP
#define GOJO_TEST_UTILS_HPP

#include <filesystem>
#include <print>
#include <span>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

constexpr std::string_view CONFIG_MIN { "config_minimum.toml" };

constexpr std::string_view CONFIG_FULL { "config_full.toml" };

const std::filesystem::path get_assets_path() {
    // This code is run from <PROJECT_ROOT>/build/Debug/test
    // The assets path is <PROJECT_ROOT>/test/assets
    static const std::filesystem::path ASSETS_PATH { "../../../test/assets" };
    return std::filesystem::absolute(ASSETS_PATH);
}

const std::filesystem::path get_config(const std::filesystem::path filepath) {
    return get_assets_path() / "config" / filepath;
}

bool init_test_gojo_project(const std::filesystem::path filepath,
                            std::span<std::string_view> args) {
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove_all(filepath);
    }

    if (!std::filesystem::create_directories(filepath)) {
        return false;
    }
    std::filesystem::current_path(filepath);
    try {
        cmd::init(args);
    } catch (const std::exception& e) {
        std::println("{}", e.what());
        return false;
    }
    return true;
}

bool cleanup_test_gojo_project(const std::filesystem::path filepath) {
    return std::filesystem::remove_all(filepath);
}

bool create_test_gojo_profile(const cfg::Config& cfg) {
    if (!std::filesystem::exists(utils::get_profiles_dir())) {
        if (!std::filesystem::create_directories(utils::get_profiles_dir())) {
            return false;
        }
    }

    try {
        cfg.to_file(utils::get_profiles_dir() / "test");
    } catch (const std::exception& e) {
        std::println("{}", e.what());
        return false;
    }
    return true;
}

bool create_test_gojo_profile() {
    if (!std::filesystem::exists(utils::get_profiles_dir())) {
        if (!std::filesystem::create_directories(utils::get_profiles_dir())) {
            return false;
        }
    }

    try {
        cfg::Config::from_file(get_config(CONFIG_MIN))
            .to_file(utils::get_profiles_dir() / "test");
    } catch (const std::exception& e) {
        std::println("{}", e.what());
        return false;
    }
    return true;
}

bool remove_test_gojo_profile() {
    return std::filesystem::remove(utils::get_profiles_dir() / "test");
}

#endif
