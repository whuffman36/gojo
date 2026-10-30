#include <string_view>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view HELP {
    R"(
gojo: a C/C++ CLI

Usage: gojo {2}<command>{0} {3}<arg(s)>{0} [options]

Commands:
    {2}init{0} {3}<name>{0} [-i -v ...]             Create new gojo project in current working directory
    {2}new{0} {3}<name> <path>{0} [-i -v ...]       Create new gojo project at <path>
    {2}build{0} [--release ...]               Compile a gojo project
    {2}run{0} [...]                           Compile and run a binary gojo project
    {2}test{0} [--target=<target> --build]    Run unit test(s)
    {2}clean{0} [--deps]                      Clean build directory
    {2}fmt{0} [--use= --save-as= ...]         Auto-format the project, manage custom format files
    {2}check{0} [--verbose]                   Run static code analyzer(s)
    {2}refresh{0} [--verbose]                 Reconfigure the project
    {2}dep{0} {3}<search | add | ...>{0} [...]      Manage dependencies for the project
    {2}profile{0} {3}<create | ...>{0} [...]        Manage gojo profiles
    {2}update{0}                              Install the most recent version of gojo-upgrade
    {2}info{0}                                Print the manual page for the gojo config file
    {2}help{0}                                Print this help page

See 'gojo {2}<command>{0} --help' for more information on a specific command
)"
};

}

namespace cmd {

void help() {
    utils::gojo_print(utils::LogLevel::Info, HELP, INFO_COLORS);
}

}  // namespace cmd
