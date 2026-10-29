#include "src/cmd/enums.hpp"

#include <flat_map>
#include <optional>
#include <string_view>

namespace cmd {

std::optional<Command> command_from_str(std::string_view str) {
    static const std::flat_map<std::string_view, Command> map {
        { "init", Command::Init },     { "build", Command::Build },
        { "run", Command::Run },       { "test", Command::Test },
        { "clean", Command::Clean },   { "fmt", Command::Format },
        { "check", Command::Check },   { "refresh", Command::Refresh },
        { "dep", Command::Dep },       { "profile", Command::Profile },
        { "info", Command::Info },     { "help", Command::Help },
        { "new", Command::New },       { "--version", Command::Version },
        { "update", Command::Update },
    };

    const auto result { map.find(str) };
    if (result == map.end()) {
        return std::nullopt;
    }

    return result->second;
}

}  // namespace cmd
