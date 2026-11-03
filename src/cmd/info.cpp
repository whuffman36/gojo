/// @author    William Huffman (whuffman36)
/// @date      2026
/// @copyright MIT License

#include <string_view>

#include "src/cmd/commands.hpp"
#include "src/utils.hpp"

namespace {

constexpr std::string_view INFO {
    R"(# Gojo CLI - See https://github.com/whuffman36/gojo for more
#
# Gojo is a cli tool that simply wraps around several common open source
# development tools. The key to gojo is the gojo.toml config file that is
# meant to sit at the root of your C or C++ project. This config file allows
# you to create declaritive, easy to reproduce project environments. The file
# is read by gojo to determine the project settings.
#
# Any field that is not marked with a '(REQUIRED)' in the description can be
# omitted from the config file and the default option(s) will be used. Some
# fields are not necessary and therefore do not even have a default value,
# other than 'undefined'. This is reflected with 'None' below.
#
# This section defines project wide settings.
[project]

# Name of the project. This name will be used to create a Cmake project and
# a CMake target. For binaries, this name will be the name of the executable
# that is produced from compilation. (REQUIRED)
name =

# Programming language of the project. (REQUIRED)
# Options are:
#   - "C++"
#   - "C"
language =

# Build target of the project. (REQUIRED)
# Options are:
#   - "binary"
#   - "library"
target =

# Toggle verbose mode project wide. Send subprocess output to standard output.
# Options are:
#   - true
#   - false
# Default option: false
verbose = 

# This section defines compile settings and options.
[build]

# Language standard used for compilation.
# Options for C++ are:
#   - 11
#   - 14
#   - 17
#   - 20
#   - 23
#   - 26
# Options for C are:
#   - 99
#   - 11
#   - 17
#   - 23
# Default options: (C++) 23, (C) 23
language_standard =

# Compiler used for compilation.
# Options for C++ are:
#   - "default"
#   - "clang++"
#   - "g++"
#   - "homebrew-clang++"
#   - "homebrew-g++"
#   - "apple-clang++"
# Options for C are:
#   - "default"
#   - "clang"
#   - "gcc"
#   - "homebrew-clang"
#   - "homebrew-gcc"
#   - "apple-clang"
# Default options: (C++) "default", (C) "default"
#
# Additionally, you can put the absolute path of the compiler you want to use,
# such as "/usr/bin/gcc".
#
# The value "default" depends on the operating system, with Linux resolving to
# "gcc"/"g++" and MacOS resolving to "apple-clang"/"apple-clang++"
#
# If you don't know what to use, it's best to leave this with "default" or just
# omit the field altogether.
compiler =

# Flags that are passed to the compiler during the build process. These flags
# are also used when compiling dependencies. The flags are represented as an
# array of strings, each entry containing a flag or argument to be passed.
compiler_flags = []

# Decides whether or not GNU extensions are enabled for the compilation. You
# can read more about GNU extensions here: https://gcc.gnu.org/onlinedocs/gcc/C-Extensions.html
# Options are:
#   - true
#   - false
# Default option: false
gnu_extensions_enabled =

# Build system used by gojo to configure and build the project.
# Options are:
#   - "CMake"
# Default option: "CMake"
build_system =

# Generator used to create build files from the build system build graph.
# Options are:
#   - "default"
#   - "Ninja"
#   - "Ninja-Multi Config"
#   - "Unix Makefiles"
#   - "XCode"
# Default option: "default"
#
# The "default" option depends on the operating system, with Linux and MacOS
# resolving to "Unix Makefiles"
#
# If you don't know what to choose, leaving this field as "default" or omitting
# it is the best option.
generator =

# Build type to build the project with.
# Options are:
#   - "Debug"
#   - "Release"
#   - "RelWithDebInfo"
#   - "MinSizeRel"
# Default option: "Debug"
build_type =

# This section defines settings and options for static analysis.
[checks]

# Declare the format style used when auto-formatting the project source code.
# Options are:
#   - "custom"
#   - "gojo"
#   - "google"
#   - "llvm"
#   - "chromium"
#   - "mozilla"
#   - "webkit"
#   - "microsoft"
#   - "gnu"
# Default option: None
# See more about clang-format: https://clang.llvm.org/docs/ClangFormat.html
clang_format_style =

# Language server used for the project.
# Represented as a string. For now, only the value "clangd" has any
# consequence, which is the creation of a .clangd file in the project root.
language_server =

# Enable the use of clang-tidy in this project.
# Options are:
#   - true
#   - false
# Default option: false
# See more about clang-tidy: https://clang.llvm.org/extra/clang-tidy/
clang_tidy_enabled =

# Forward command line arguments to clang-tidy. Each argument is represented
# as an entry in an array of strings.
clang_tidy_args = []

# Enable the use of cppcheck in the project.
# Options are:
#   - true
#   - false
# Default option: false
# See more about cppcheck: https://cppcheck.sourceforge.io/manual.pdf
cppcheck_enabled =

# Enable specific checks to be be run by cppcheck. Each check is represented
# as an entry in an array of strings.
cppcheck_checks = []

# Forward command line arguments to cppcheck. Each argument is represented as
# an entry in an array of strings.
cppcheck_args = []

# This section defines settings for testing in the project.
[tests]

# Enable CMake unit testing.
# Options are:
#   - true
#   - false
# Default option: false
testing_enabled =

# Third party unit test library to be used by the project.
# This field is really only used if testing_enabled is set to true.
# Options for C++ are:
#   - "GoogleTest"
# Options for C are:
#   - "Unity"
# Default options: None
# See more about GoogleTest: https://github.com/google/googletest
# See more about Unity: https://github.com/throwtheswitch/unity
test_framework =

# This section defines settings for managing third party dependencies.
[dependencies]

# Package manager to be used by the project.
# Options are:
#   - "conan"
#   - "vcpkg"
# Default option: None
# See more about conan: https://conan.io/
# See more about vcpkg: https://vcpkg.io/en/
package_manager =

# List of build types in which any dependencies should be built.
# The maximum size of this array is four, and each entry must be unique.
# The options for each entry are:
#   - "Debug"
#   - "Release"
#   - "RelWithDebInfo"
#   - "MinSizeRel"
# Default option: None
#
# If this field is left empty or omitted, then the build type defined in
# build.build_type will be used to compile any dependencies.
build_types = []
)"
};

}

namespace cmd {

void info() {
    utils::gojo_print(utils::LogLevel::Info, INFO);
}

}  // namespace cmd
