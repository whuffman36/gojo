/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <format>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "src/cfg/config.hpp"
#include "src/cfg/enums.hpp"
#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"({1}Command:{0}
    gojo {2}run{0} [options]

{1}Summary:{0}
    Compile the project and immediately run the resulting executable. This
    command has no effect if the project is a library target.

{1}Options:{0}
    Any options passed to this command will be forwarded to the executable.        
)"
};

}

namespace cmd {

void run(std::span<std::string_view> args) {
    for (const auto arg : args) {
        if (arg == "--help") {
            utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
            return;
        }
    }

    const cfg::Config cfg { cfg::Config::this_project() };
    if (cfg.target == cfg::Target::Library) {
        utils::gojo_print(
            utils::LogLevel::Warning,
            "cannot run a library target. See 'gojo run --help' for more information"
        );
        return;
    }

    build({});

    // Can't think of a better way to write this since the format
    // string needs to be constexpr...
    std::string cmd {};
    if (cfg.is_multi_config()) {
        cmd = std::format("./build/src/{}/{} {}",
                          cfg::to_str(cfg.get_build_type()), cfg.name,
                          args_to_string(args));
    }
    else {
        cmd = std::format("./build/{}/src/{} {}",
                          cfg::to_str(cfg.get_build_type()), cfg.name,
                          args_to_string(args));
    }

    const auto cmd_result { utils::execute_command(
        cmd.c_str(), false, true, cfg.get_build_dir() / "log.txt"
    ) };
    if (!cmd_result.success) {
        throw std::runtime_error { std::format("failed to run {} executable",
                                               cfg.name) };
    }
}

}  // namespace cmd
