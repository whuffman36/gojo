#include <string_view>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view INFO {
    R"(
{1}gojo{0} man page

{1}About{0}
Gojo is a CLI for C and C++ projects heavily inspired by rust's cargo CLI. It
allows the freedom, complexity, and customizability of C/C++ projects while
simplifying the repetitve development tasks to a few simple commands. Gojo
does this by wrapping several open source C and C++ tools into one tool.
Gojo relies on several tool dependencies:

  - {3}CMake (meta build system){0}
  - {3}clang-tidy (static analyzer){0}
  - {3}cppcheck (static analyzer){0}
  - {3}clang-format (code formatter){0}
  - {3}conan (package manager){0}

For most use cases, gojo handles much of the complexity of handling several
tools in one project and allows you to focus on writing code for your project.
Gojo synergizes best with the clang compiler and toolchain, though it works
just fine with other C and C++ compilers respectively.

Most of the functionality of gojo relies on the .gojo config file placed in
your projects root directory. From there, Configuring and reconfiguring your
project is a matter of editing a few config files and running gojo commands.

Gojo is designed for relatively simple projects. If your project has complex
build requirements or dependency management, you should probably look into
another tool or set up your project manually.

Gojo is an open source project, so if you notice any bugs or want to add your
own set of tools or functionality, please consider contributing! You can find
the link to the github repo at the end of this file.

{1}.gojo Config Files{0}
.gojo config files are meant to be placed at the root of your gojo project and
consist of several fields that are used to simplify common processes such as
building, testing, and analyzing your codebase. To see a blank .gojo config
file, you can use {1}gojo{0} {3}template{0}.

Hopefully most of the fields in the config file are self-explanatory,
especially if you already have experience working with C, C++, and some of its
open source toolchains. Below is an explanation of each field:

  - {2}project name{0}
    This is the name of the project you are working on. This field is used when
    creating the file templates upon project creation. By default, this name is
    used as the default build target for library projects. Changing this value
    after project creation should have no effect.

  - {2}project target{0}
    This determines whether your project is a binary program or a library. If
    you change this field after project creation, you will need to edit the
    file(s) of your meta build system to reflect the change, and you will also
    need to reconfigure your build system (which can be done with
    {1}gojo{0} {3}refresh{0}).

  - {2}project root directory{0}
    This is the path of the project root directory. This value should only be
    modified if the rest of the project structure remains intact, as gojo uses
    this value to find all relevant project files. If this value is changed,
    you will need to reconfigure your build system with
    {1}gojo{0} {3}refresh{0}.

  - {2}executable name{0}
    This is the name of the executable that will be run when calling
    {1}gojo{0} {3}run{0}. This field is only relevant for
    binary projects, and has no effect for library projects. Additionally,
    changing this value does not require you to reconfigure the project. By
    default, gojo searches for the executable in either

      - {3}build/src/<build_type>/  (multi-config generator){0}
      - {3}build/<build_type>/src   (single-config generator){0}

    depending on your build system generator. That is, if you want to run a
    specific executable that exists in a child directory of src/, you need
    to provide the relative path from build/src/<build_type>/ or
    build/<build_type>/src/, where <build_type> is either 'Release' or 'Debug'
    You can see which kind of generator your project is using by looking at
    the {2}multi-config generator{0} field in the config file.

    This field comes from the executable target name defined in your build
    system file. If you change the target name there, you need to change this
    field to reflect that.

  - {2}project language{0}
    This tells gojo whether your project is in C or C++. This value is used
    when configuring and building the project, and must be changed along with
    the {2}compiler{0} (and likely {2}language standard{0}) field. The project must be
    reconfigured if this value changes.

  - {2}language standard{0}
    This field determines which standard of either C or C++ you want to build
    with. The project must be reconfigured if this value changes.

  - {2}GNU extensions enabled{0}
    This field determines whether or not your project will be compiled with
    GNU extensions enabled. GNU extensions are specific compiler extensions
    that add features not included in the base C and C++ standards. Some
    compilers may not support GNU extensions, but clang, gcc, and apple-clang
    do. The project must be reconfigured if this value changes.

  - {2}compiler{0}
    This field tells gojo which compiler to use. This compiler must already
    be available on your system. The value '{3}system default{0}' defaults to

      - {3}gcc/g++ on linux{0}
      - {3}clang/clang++ on macOS{0}

    though clang actually refers to apple-clang on macOS. The project must be
    reconfigured if this value changes.

  - {2}compiler flags{0}
    This field forwards any flags to the compiler set in the {2}compiler{0} field.
    The flags are forwarded exactly as they are written in the file, so the
    '-' characters are required. The project must be reconfigured if this value
    changes.

  - {2}build system{0}
    This field tells gojo which build system to use to configure, build, and
    test the project. As of now, '{3}CMake{0}' is the only accepted value. Changing
    this value will result in gojo not working at all.

  - {2}generator{0}
    This field tells gojo which generator to use with the meta build system
    declared in the {2}build system{0} field. For a full list of acceptable
    generators, see
    {3}https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html{0}
    The project must be reconfigured if this value changes.

  - {2}multi-config generator{0}
    This field tells gojo whether or not the generator being used with the
    build system is single or multi config. You should not change this value
    manually, it is managed by gojo. It is exposed in the .gojo config file
    purely for transparency.

  - {2}build type{0}
    This field determines the optimization level when compiling your project
    and any dependencies. There are four options:

      - {3}Debug (-O0 -g){0}
        Unoptimized, debug symbols included

      - {3}Release (-03 -DNDEBUG){0}
        Fully optimized, no debug symbols included

      - {3}RelWithDebInfo (-02 -g -DNDEBUG){0}
        Optimized with debug symbols included

      - {3}MinSizeRel (-0s -DNDEBUG){0}
        Optimized for final binary size rather than speed, no debug symbols
        included

    The project must be reconfigured if this value is changed. If you have not
    built your dependencies with the build type you changed to, you will need
    to install and/or build them again as well. It is recommended to use the
    build_types=<> option when running {1}gojo{0} {3}install{0} to install
    your dependencies with every build type that you want to use so that you
    install them up front instead of when switching to a new build type for
    the first time.

  - {2}package manager{0}
    This field determines if your project is using a package manager. If no
    package manager is used, this value is set to '{3}none{0}'. The command
    {1}gojo{0} {3}install{0} has no effect if no package manager is used.

  - {2}dependency build types{0}
    This field determines the build type(s) that your dependendencies should
    build with. If this field is left empty, and a package manager is being
    used, the default value is '{3}Debug,Release{0}'. The value for this field
    must be a comma separated list of one or more build types, the selection
    being the same as the {2}build type{0} field. This is used to make
    switching between build types easier - the dependencies are built with the
    defined build_type(s) at installation time rather than when the project
    switches its build type. Each dependency should be built with the same
    build type as your project.

  - {2}source file extension{0}
    This field determines the file extension for C++ source code files. This
    field is not used for C projects. This field is only used during project
    creation and changing its value has no effect.

  - {2}header file extension{0}
    This field determines the file extension for C++ header files. This
    field is not used for C projects. This field is only used during project
    creation and changing its value has no effect.

  - {2}clang-format style{0}
    This field determines the style that clang-format will use to format your
    code. This field is used during project creation to create a .clang-format
    file in the project root directory outlining all of the format rules,
    which you can tweak to your preferences. If this value is set to '{3}custom{0}',
    then the .clang-format file remains empty. This value is ignored if any
    arguments are passed to {1}gojo{0} {3}fmt{0}.

  - {2}clangd enabled{0}
    This field tells gojo whether or not your project is using clangd as your
    LSP. If it is enabled, a .clangd config file is created in the project
    root directory helping clangd to find the 'compile_commands.json' file
    to enable linting in your editor. Changing this value has no effect.

  - {2}clang-tidy enabled{0}
    This field tells gojo whether or not your project is using clang-tidy
    to statically analyze your code. If it is enabled, a .clang-tidy file is
    created in the project root directory allowing you to customize the
    analyzer. This value also determines if clang-tidy is run during the
    {1}gojo{0} {3}check{0} command. If clang-tidy and cppcheck are both
    disabled, that command has no effect.

  - {2}clang-tidy arguments{0}
    This field is forwarded directly to clang-tidy when running the
    {1}gojo{0} {3}check{0} command. You can see a list of valid
    clang-tidy arguments here: {3}https://clang.llvm.org/extra/clang-tidy/{0}

  - {2}cppcheck enabled{0}
    This field tells gojo whether or not your project is using cppcheck
    to statically analyze your code. If it is enabled, cppcheck will run
    using the values defined in {2}cppcheck enabled checks{0} and
    {2}cppcheck arguments{0} to run static analysis checks on your project in
    the {1}gojo{0} {3}check{0} command. If cppcheck and clang-tidy are both
    disabled, the check command has no effect.

  - {2}cppcheck enabled checks{0}
    This field tells cppcheck which static analysis checks to run when using
    the {1}gojo{0} {3}check{0} command. You can find a list of available checks
    here: {3}https://linux.die.net/man/1/cppcheck{0}

  - {2}cppcheck arguments{0}
    This field is forwarded directly to cppcheck when running the
    {1}gojo{0} {3}check{0} command. You can see a list of available cppcheck
    arguments here: {3}https://linux.die.net/man/1/cppcheck{0}
    Note that the '--enable=' flag corresponds to {2}cppcheck enabled checks{0} in
    the .gojo config file and should not be duplicated here.

  - {2}testing enabled{0}
    This field tells gojo whether to build your project's unit tests. If
    testing is not enabled, unit tests are not built and
    {1}gojo{0} {3}test{0} has no effect. The project must be reconfigured
    if this value changes.

  - {2}testing framework{0}
    This field tells gojo if a third party testing framework is going to be
    used in your project. This field is only used during project creation
    to make test file templates. Changing this value has no effect.

  - {2}git repository{0}
    This field tells gojo if a git repository should be created in the
    project root directory. This field is only used during project creation
    to make a .gitignore file template. Changing this value has no effect.

{1}gojo Profiles{0}
Because gojo works by reading and writing to a .gojo config file, most of the
settings in a project can be saved and reused for future projects. Gojo allows
you to store these configurations in gojo profiles, located in
~/.gojocli/profiles. Using an existing gojo profile, you can create a new
project with the {1}gojo{0} {3}init --profile=<name>{0} command, create a profile
from your current project settings with the {1}gojo{0} {3}profile create{0}
command, or see your current profiles with the {1}gojo{0} {3}profile list{0}
command. Using gojo profiles should make project setup even faster, assuming
you want to use many of the same settings as a previous project.

{1}Questions, Concerns, Bugs, and Contributions{0}
You can submit any bugs, questions, or other issues at the gojo github
repository, as well as become a contributor:
{3}https://github.com/gojocli/gojo{0}
)"
};

}

namespace cmd {

void info() {
    utils::gojo_print(utils::LogLevel::Info, INFO, INFO_COLORS);
}

}  // namespace cmd
