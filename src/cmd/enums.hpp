#ifndef GOJO_SRC_CMD_ENUMS_HPP
#define GOJO_SRC_CMD_ENUMS_HPP

#include <cstdint>
#include <optional>
#include <string_view>


namespace cmd {

enum class Command : std::uint8_t {
  Init,
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
};

std::optional<Command> command_from_str(std::string_view str);

};

#endif