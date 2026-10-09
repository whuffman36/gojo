#ifndef GOJO_SRC_COMMANDS_ENUMS_HPP
#define GOJO_SRC_COMMANDS_ENUMS_HPP

#include <cstdint>
#include <flat_map>
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


const std::flat_map<std::string_view, Command> command_map {
  { "init", Command::Init },
  { "build", Command::Build },
  { "run", Command::Run },
  { "test", Command::Test },
  { "clean", Command::Clean },
  { "fmt", Command::Format },
  { "check", Command::Check },
  { "refresh", Command::Refresh },
  { "dep", Command::Dep },
  { "profile", Command::Profile },
  { "info", Command::Info },
  { "help", Command::Help },
  { "--version", Command::Version },
};

};

#endif