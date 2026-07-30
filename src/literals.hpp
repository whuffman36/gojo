#ifndef GOJO_SRC_LITERALS_HPP
#define GOJO_SRC_LITERALS_HPP

#include <string_view>


namespace literals {

// {0} CLEAR
// {1} MAGENTA_BOLD
// {2} MAGENTA
// {3} YELLOW
// {4} CYAN

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


constexpr std::string_view INIT_HELP {
R"({1}Command:{0}
  gojo {2}init{0} {3}<name>{0} {4}[options]{0}

{1}Summary:{0}
  Create a new gojo project in the current directory. A name arguemnt
  is required. If no options are supplied, a default C++ project is
  created. The settings for a default C++ project can be found in the
  'gojo info' command.

{1}Arguments:{0}
  {3}name{0}            Name of the gojo project to be created.

{1}Options:{0}
  {4}--file | -f{0}       Create a new gojo project using a .gojo config
                    file located in the current directory. This option
                    syngergizes well with 'gojo template'. All other
                    arguments and options are ignored when using --file,
                    including <name>. This command will fail if no .gojo
                    config file is found in the current directory.

  {4}-i{0}                Create a new gojo project by answering several
                    interactive prompts. All other arguments and options
                    are ignored when using -i, including <name>.

  {4}--lib{0}             Set the project to be a library. By default, a new
                    gojo project is a binary executable.

  {4}--lang=<c|c++>{0}    Set the language of the project. The language options
                    are 'c' or 'c++'. By default, a new gojo project is C++.

  {4}--profile=<name>{0}  Create a new gojo project from an existing profile.
                    The profile must exist. When combining this option with
                    options, this one must come first, ie
                    gojo init --profile=default --lib
)"
};


constexpr std::string_view BUILD_HELP {
R"({1}Command:{0}
  gojo {2}build{0} {4}[options]{0}

{1}Summary:{0}
  Compile the project. By default, all targets are built in debug mode using
  the compiler flags set in the .gojo config file located at the root of the
  project.

{1}Options:{0}
  {4}--release{0}         Compile the project with all compiler optimizations
                    enabled, namely -03. If the project is currently set
                    to debug mode, it will be reconfigured before
                    compilation. If the project is already in release mode,
                    this option has no effect. No debug symbol information
                    is kept when compiled in release mode, unless
                    overidden with specific compiler flags set in the
                    .gojo config file of the project.

  {4}--debug{0}           Compile the project with debug symbols and zero
                    compiler optimizations enabled. If the project is
                    currently set to release mode, it will be reconfigured
                    before compilation. If the project is already in debug
                    mode, this option has no effect.

  {4}--tests{0}           Enable testing in the project and compile unit tests
                    along with the source code. If the project currently
                    does not have testing enabled, it will be reconfigured
                    before compilation. If the project already has testing
                    enabled, this option has no effect.

  {4}--no-tests{0}        Disable testing in the project and omit unit tests from
                    the compilation process. If the project currently has
                    testing enabled, it will be reconfigured before
                    compilation. If the project currently does not have
                    testing enabled, this option has no effect.

  {4}--clean{0}           Clean the build artifacts and caches before compilation.

  {4}--target=<target>{0} Compile only a specific target instead of the whole
                    project.
)"
};


constexpr std::string_view RUN_HELP {
R"({1}Command:{0}
  gojo {2}run{0} {4}[options]{0}

{1}Summary:{0}
  Run the compiled executable produced by the project. A call to 'gojo build'
  must be successfully completed before running this command. This command
  has no effect if the project is a library. This command uses the 'executable
  name' field in the .gojo config file of the project in order to find the
  correct executable to run, and will fail if it cannot find an executable with
  that name.

{1}Options:{0}
  Any options passed to this command will be forwarded to the executable.             
)"
};


constexpr std::string_view TEST_HELP {
R"({1}Command:{0}
  gojo {2}test{0} {4}[options]{0}

{1}Summary:{0}
  Run unit tests in the project. Tests must be enabled in the .gojo config
  file and a call to 'gojo build' must be successfully completed before
  running this command.

{1}Options:{0}
  {4}--target=<target>{0} Test only a specific target intead of all unit tests.
)"
};


constexpr std::string_view CLEAN_HELP {
R"({1}Command:{0}
  gojo {2}clean{0} {4}[options]{0}

{1}Summary:{0}
  Remove build artifacts and/or caches.

{1}Options:{0}
  {4}--deep{0}          Remove all build artifacts and caches from the,
                  project, while keeping dependencies intact.

  {4}--all{0}           Remove everything from the build directory, including
                  dependencies. Use this option with caution as it
                  requires dependencies to be reinstalled and rebuilt,
                  and the project has to be re-configured.
)"
};


constexpr std::string_view FMT_HELP {
R"({1}Command:{0}
  gojo {2}fmt{0} {4}[options]{0}

{1}Summary:{0}
  Format the project using clang-format. By default, this command uses the
  .clang-format file found in the root of the project created by 'gojo init'.
  If no file is found and no options are supplied, this command has no effect.
  Any options supplied to this command are forwarded directly to clang-format
  and override any settings in the .clang-format file. By default, code is
  auto formatted and changed in-place according the rules laid out in the
  .clang-format file. More information on clang-format can be found here:
  {4}https://clang.llvm.org/docs/ClangFormat.html{0}

{1}Options:{0}
  Any options supplied to this command are forwarded to clang-format and
  override any rules set in the .clang-format file, if it exists.
)"
};


constexpr std::string_view CHECK_HELP {
R"({1}Command:{0}
  gojo {2}check{0} {4}[options]{0}

{1}Summary:{0}
  Run static analysis checks on the code without building the project. There
  are two programs used to run static analysis checks, clang-tidy and
  cppcheck. Both can be enabled or disabled in the .gojo config file located
  at the root of the project. If neither program is enabled, this command has
  no effect. clang-tidy can be configured with the .clang-tidy file located
  at the root of the project, created by 'gojo init' if enabled. Additionally,
  command line arguments can be supplied in the .gojo config file at the root
  of the project using the 'clang-tidy arguments' field, which will take
  precedence over the .clang-tidy file. More information on clang-tidy can be
  found here: {4}https://clang.llvm.org/extra/clang-tidy/{0}
  cppcheck can be configured in the .gojo config file located in the root of
  the project using the 'cppcheck checks enabled' and 'cppcheck arguments'
  fields. The former decides which checks will be run and the latter is used
  to pass any command line options to cppcheck. More information on cppcheck
  can be found here: {4}https://cppcheck.sourceforge.io/manual.html{0}

{1}Options:{0}
  {4}--verbose | -v{0}  Print all output from the static analyzers enabled
                  in the project. By default, only errors are printed.
)"
};


constexpr std::string_view TEMPLATE_HELP {
R"({1}Command:{0}
  gojo {2}template{0}

{1}Summary:{0}
  Create an empty template for a .gojo config file in the current directory.
  This command synergizes well with 'gojo init --file' to create a new project
  using the settings laid out in a .gojo config file. See 'gojo info' for more
  information on the .gojo config file.
)"
};


constexpr std::string_view REFRESH_HELP {
R"({1}Command:{0}
  gojo {2}refresh{0} {4}[options]{0}

{1}Summary:{0}
  Read the .gojo config file located at the root of the project and reconfigure
  the project. This command is necessary when changing the following fields in
  the .gojo config file:

    - 'language standard'
    - 'gnu extensions'
    - 'compiler'
    - 'compiler flags'
    - 'generator'
    - 'build type'
    - 'testing enabled'

{1}Options:{0}
  {4}--profile=<name>{0}   Configure the project with an existing gojo profile.
                     The profile must exist.
)"
};


constexpr std::string_view INSTALL_HELP {
R"({1}Command:{0}
  gojo {2}install{0}

{1}Summary:{0}
  Install and build dependencies in both debug and release mode. They are
  built in both modes to allow seamless switching between build types in
  the project without having to rebuild upon a switch. This command is
  necessary if dependencies have been added or removed from the dependency
  list in either the conanfile.py or vcpkg files. This command has no effect
  if the package manager is set to 'none' in the project.
)"
};


constexpr std::string_view PROFILE_HELP {
R"({1}Command:{0}
  gojo {2}profile{0} {4}[options]{0}

{1}Summary:{0}
  Manage global gojo profiles. Some options require arguemnts to work. Profiles
  are stored in ~/.gojocli/profiles as .gojo config files.

{1}Options{0}
  {4}create{0} {3}<name>{0}    Create a new gojo profile with the specified
                   name. The name arguemnt is required.

  {4}list{0}             List all of the current gojo profiles found in
                   the system.

  {4}show{0} {3}<name>{0}      Show the gojo profile with the specified name.

  {4}delete{0} {3}<name>{0}    Delete a gojo profile with the specified name.
                   specify 'all' to delete all gojo profiles.
)"
};


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
file, you can use {1}gojo{0} {4}template{0}.

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
    {1}gojo{0} {4}refresh{0}).

  - {2}project root directory{0}
    This is the path of the project root directory. This value should only be
    modified if the rest of the project structure remains intact, as gojo uses
    this value to find all relevant project files. If this value is changed,
    you will need to reconfigure your build system with
    {1}gojo{0} {4}refresh{0}.

  - {2}executable name{0}
    This is the name of the executable that will be run when calling
    {1}gojo{0} {4}run{0}. This field is only relevant for
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
    {4}https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html{0}
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
    build_types=<> option when running {1}gojo{0} {4}install{0} to install
    your dependencies with every build type that you want to use so that you
    install them up front instead of when switching to a new build type for
    the first time.

  - {2}package manager{0}
    This field determines if your project is using a package manager. If no
    package manager is used, this value is set to '{3}none{0}'. The command
    {1}gojo{0} {4}install{0} has no effect if no package manager is used.

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
    arguments are passed to {1}gojo{0} {4}fmt{0}.

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
    {1}gojo{0} {4}check{0} command. If clang-tidy and cppcheck are both
    disabled, that command has no effect.

  - {2}clang-tidy arguments{0}
    This field is forwarded directly to clang-tidy when running the
    {1}gojo{0} {4}check{0} command. You can see a list of valid
    clang-tidy arguments here: {4}https://clang.llvm.org/extra/clang-tidy/{0}

  - {2}cppcheck enabled{0}
    This field tells gojo whether or not your project is using cppcheck
    to statically analyze your code. If it is enabled, cppcheck will run
    using the values defined in {2}cppcheck enabled checks{0} and
    {2}cppcheck arguments{0} to run static analysis checks on your project in
    the {1}gojo{0} {4}check{0} command. If cppcheck and clang-tidy are both
    disabled, the check command has no effect.

  - {2}cppcheck enabled checks{0}
    This field tells cppcheck which static analysis checks to run when using
    the {1}gojo{0} {4}check{0} command. You can find a list of available checks
    here: {4}https://linux.die.net/man/1/cppcheck{0}

  - {2}cppcheck arguments{0}
    This field is forwarded directly to cppcheck when running the
    {1}gojo{0} {4}check{0} command. You can see a list of available cppcheck
    arguments here: {4}https://linux.die.net/man/1/cppcheck{0}
    Note that the '--enable=' flag corresponds to {2}cppcheck enabled checks{0} in
    the .gojo config file and should not be duplicated here.

  - {2}testing enabled{0}
    This field tells gojo whether to build your project's unit tests. If
    testing is not enabled, unit tests are not built and
    {1}gojo{0} {4}test{0} has no effect. The project must be reconfigured
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
project with the {1}gojo{0} {4}init --profile=<name>{0} command, create a profile
from your current project settings with the {1}gojo{0} {4}profile create{0}
command, or see your current profiles with the {1}gojo{0} {4}profile list{0}
command. Using gojo profiles should make project setup even faster, assuming
you want to use many of the same settings as a previous project.

{1}Questions, Concerns, Bugs, and Contributions{0}
You can submit any bugs, questions, or other issues at the gojo github
repository, as well as become a contributor:
{4}https://github.com/gojocli/gojo{0}
)"
};


constexpr std::string_view WIN {
R"({2}
⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⣾⡳⣼⣆⠀⠀⢹⡄⠹⣷⣄⢠⠇⠻⣷⣶⢀⣸⣿⡾⡏⠀⠰⣿⣰⠏⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⣀⣀⣀⡹⣟⡪⢟⣷⠦⠬⣿⣦⣌⡙⠿⡆⠻⡌⠿⣦⣿⣿⣿⣿⣦⣿⡿⠟⠚⠉⠀⠉⠳⣄⡀⠀⠀⠁⠀
⠀⠀⠀⠀⠀⠀⠀⡀⢀⣼⣟⠛⠛⠙⠛⠉⠻⢶⣮⢿⣯⡙⢶⡌⠲⢤⡑⠀⠈⠛⠟⢿⣿⠛⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣆⠀⠀⠀
⠀⠀⠀⠀⠀⡸⠯⣙⠛⢉⣉⣙⣿⣿⡳⢶⣦⣝⢿⣆⠉⠻⣄⠈⢆⢵⡈⠀⠀⢰⡆⠀⣼⠓⠀⠀⠀ Nah,    ⠀⠈⣷⠀⠀
⠀⠀⠀⠖⠉⠻⣟⡿⣿⣭⢽⣽⣶⣈⢛⣾⣿⣧⠀⠙⠓⠀⠑⢦⡀⠹⣧⢂⠀⣿⡇⢀⣿⠺⠇⠀  I'd        ⣿⠀⠀
⠀⠀⠀⠀⠐⠈⠉⢛⣿⣿⣶⣤⣈⠉⣰⣗⡈⢛⣇⠀⣵⡀⠀⠘⣿⡄⢻⣤⠀⢻⡇⣼⣧⣿⡄⠀⠀ Win      ⠀⠀⡿⠀⠀
⠀⠀⠀⠀⠀⣠⣾⣿⢍⡉⠛⠻⣷⡆⠨⣿⣭⣤⣍⠀⢹⣷⡀⠀⠹⣿⡄⠈⠀⢿⠁⣿⣿⠏⠀⠀⠀            ⣇⠀⠀
⠀⣿⣇⣠⣾⣿⣛⣲⣿⠛⠀⠀⢀⣸⣿⣿⣟⣮⡻⣷⣤⡙⢟⡀⠀⠙⢧⠀⠀⠎⠀⠉⠁⠰⣿⠀⠀          ⢀⡿⠀⠀
⠀⠈⢻⣿⣿⣽⣿⣿⣿⣴⡏⠚⢛⣈⣍⠛⠛⠿⢦⣌⢙⠻⡆⠁⠀⠀⠀⣴⣦⠀⠀⠀⠐⢳⢻⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠮⠀⠀⠀
⠀⠀⠈⠙⣿⣧⣶⣿⠿⣧⣴⣿⢻⡉⠀⢀⣠⣴⣾⡟⠿⠃⠁⣠⣤⡶⣾⡟⠅⠀⣀⡄⠀⣾⢸⣿⣏⢻⢶⣦⣤⣤⣄⢶⣾⣿⣡⣤⡄⠀
⠀⠀⣠⣞⣋⣿⣿⣾⣿⡿⡛⣹⡟⣤⢰⡿⠟⠉⣀⣀⣤⣤⡠⠙⢁⣾⡿⠂⠀⣿⠟⣁⠀⣹⠀⣹⣿⡟⣼⣿⣿⣌⣿⣞⣿⣿⠁⠀⠀⠀
⠀⢠⡿⢛⢟⣿⣿⣿⣿⣿⣿⡟⣼⣿⣟⢓⠛⣿⣏⣿⣵⣗⣵⣴⣿⢟⡵⣣⣼⣿⢟⣵⣶⢻⣶⣿⠀⠀⣈⢻⣿⣿⣿⢿⣾⢿⣧⠀⠀⠀
⠀⠘⠃⢸⣿⡾⣿⣿⣿⣿⣯⣿⣿⣿⣶⣿⣿⣟⣾⡿⣫⣿⣿⣿⣽⣿⣿⣿⣿⢫⣾⣿⣿⣿⣿⣿⣴⡆⣻⣿⡏⣿⢻⣧⣿⡿⣿⡆⠀⠀
⠀⠀⠀⠜⣿⣾⢿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣭⣿⣖⣿⢿⣿⡿⣿⣿⣿⡿⢡⢯⣿⣿⣿⣿⣿⣿⣿⣧⡿⣾⣷⣿⣿⢿⣿⡇⠉⠁⠀⠀
⠀⠀⠀⠀⣿⣥⣾⣿⣿⣿⣿⣿⣿⣿⡇⣭⣿⣿⣿⣿⠃⠞⠟⣸⣿⠏⣸⣧⣀⠿⢿⣿⣿⣟⣿⣿⣿⣿⣽⣿⢿⣿⣿⣿⣿⠁⠀⠀⠀⠀
⠀⠀⠀⠈⠛⣹⣿⣿⣿⣿⢿⣿⣿⣿⣿⣿⣟⣿⣿⡿⢶⣦⣄⣿⠏⠀⣿⣟⣿⣶⠾⣿⣟⣋⣛⣿⣿⣿⣿⡇⣻⣿⣿⣿⡏⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠟⠛⠫⣿⣿⣿⣿⣿⡿⣧⠛⣿⠛⣿⣿⣿⣷⡌⠹⡟⠀⠀⠉⡟⠋⢠⣾⣿⣿⣿⡟⣿⣿⣿⣿⢀⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠘⠋⣾⣷⣿⣿⣧⠙⠀⠙⢣⠝⠛⠋⣽⣷⢦⠇⠀⠀⠘⠁⣤⣾⣿⠝⠛⠉⠘⢻⣿⣿⢿⣼⣷⡟⢻⣷⠉⠀⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠐⠟⢻⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠀⠈⠛⠀⠀⠀⠀⠀⣾⠟⠀⢸⣷⣿⡇⠀⠛⠀⠀⠁⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠁⠀⢹⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⡧⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠆⠀⠀⠀⠀⠀⠀⠈⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⢻⡿⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣇⠀⠀⠀⠀⠀⠀⠀⠀⠲⣄⠀⡄⠆⠀⠀⠀⠀⠀⠀⠀⠀⣼⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⣀⠀⠀⣠⣾⣿⠁⠀⠀⠀⠀⠀⣀⡄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⢻⣆⠀⠛⠁⠶⣶⣶⣶⣶⣶⣶⡶⠆⠘⠋⣠⡾⢫⣾⡟⠀⠀⠀⠀⠀⠐⠉⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠛⠀⠙⣷⡀⠀⠀⠙⠛⠛⠛⠛⠋⠁⠀⢀⣴⠋⠀⣾⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣿⣰⣦⡀⠸⣿⣦⡀⠀⠀⠀⠀⠀⠀⢀⣴⡟⠁⠀⠐⢻⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⡄⢺⣿⡄⠹⣿⠻⢦⣤⣤⣤⣤⣶⣿⡟⢀⣀⠀⠀⢸⣿⣦⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣮⣿⣿⡀⠹⡷⣦⣀⡀⡀⢸⣿⠏⢠⣾⣿⠀⠀⣾⣿⣿⣿⣿⣶⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀
⣀⣤⣴⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠘⣷⣻⡟⠀⡼⠁⣴⣿⣿⣯⣥⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣀⠀⠀⠀⠀
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣯⣿⣤⣤⣤⣬⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣄
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿{0}

{1}"Throughout Heaven and Earth, I alone am the honored one."{0}
)"
};


constexpr std::string_view GITIGNORE {
R"(CMakeLists.txt.user
CMakeCache.txt
CMakeFiles
CMakeScripts
Testing
Makefile
cmake_install.cmake
install_manifest.txt
compile_commands.json
CTestTestfile.cmake
_deps
CMakeUserPresets.json
build)"
};

}  // namespace literals

#endif
