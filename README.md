# Gojo: A CLI build system for C++

## Installation

First, clone the repository from github. In this example, the repo is cloned into a new directory called `~/devtools`, but feel free to copy the source code to your directory of choice. Gojo comes with a simple install script that installs the binary on your local filesystem.

```bash
mkdir ~/devtools
cd ~/devtools
git clone https://github.com/whuffman36/gojo.git
cd gojo
sh install.sh
```

## Update

To update gojo, return to where you first cloned the repo. The `update.sh` script will take care of removing the old version and installing the new version for you.

```bash
cd ~/devtools/gojo
sh update.sh
```

## Go Build With Gojo!

Now, let's make sure gojo is working correctly. Try running `gojo` or `gojo help` in a new terminal instance.

```bash
$ gojo
"Throughout Heaven and Earth, I alone am the honored one"

gojo: a modern build system for C++

Usage: gojo <COMMAND> [OPTIONS]

Commands:
    init <NAME> [OPTIONS]       initializes new gojo project in current directory
    build [--release]           build project with CMake
    run [<PATH>]                run compiled executable
    test                        build and run unit tests
    clean                       remove build files and CMake cache
    fmt [--style <STYLE>]       automatically formats your code according to the style provided
    check                       run static analyzers found in .clang-tidy
    help <COMMAND>              print help

See 'gojo help <COMMAND>' for more information on a specific command

```

Consult `gojo help` to learn how to use the cli, though my hope is that it is straightforward!
