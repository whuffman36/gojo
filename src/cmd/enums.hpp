#ifndef GOJO_SRC_CMD_ENUMS_HPP
#define GOJO_SRC_CMD_ENUMS_HPP

#include <cstdint>
#include <optional>
#include <string_view>

namespace cmd {

enum class Command : std::uint8_t {
    Init,
    New,
    Build,
    Run,
    Test,
    Clean,
    Format,
    Check,
    Refresh,
    Dep,
    Profile,
    Info,
    Help,
    Version,
    Update,
};

std::optional<Command> command_from_str(std::string_view str);

};  // namespace cmd

#endif
