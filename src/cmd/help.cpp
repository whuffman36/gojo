#include "src/cmd/commands.hpp"

#include <string_view>

#include "src/utils.hpp"


namespace {

constexpr std::string_view HELP { 
R"(
{1}gojo:{0} a C/C++ CLI

{1}Usage:{0} gojo {2}<command>{0} {3}<arg(s)>{0} {4}[options]{0}

{1}Commands:{0}
    {2}init{0} {3}<name>{0} {4}[-f | -i | ...]{0}     create a new gojo project in the current directory
    {2}build{0} {4}[--release | ...]{0}         build project
    {2}run{0} {4}[options]{0}                   run compiled executable
    {2}test{0} {4}[--target=<target>]{0}        run unit test(s)
    {2}clean{0} {4}[--deep | --all]{0}          clean build directory caches
    {2}fmt{0} {4}[options]{0}                   auto-format your code
    {2}check{0} {4}[--verbose]{0}               run static code analyzers
    {2}template{0}                        create a .gojo config file template in the current directory
    {2}refresh{0} {4}[--deps]{0}                reconfigure project with updated an updated .gojo config file
    {2}install{0}                         install dependencies with the selected package manager
    {2}info{0}                            print the manual page for gojo
    {2}help{0}                            print help

See 'gojo {2}<command>{0} {4}--help{0}' for more information on a specific command
)"
};


}

namespace cmd {

void help() {
  utils::print_info(HELP);
}

}
