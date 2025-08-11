#!/bin/bash

# install deps
apt update && apt upgrade -y
apt install -y git g++ cmake curl
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | bash -s -- -y

# make structure
mkdir -p ~/.gojo
mkdir -p ~/.gojo/repos
mkdir -p ~/.gojo/include
mkdir -p ~/.gojo/lib

# clone repo
cd ~/.gojo/repos
git clone https://github.com/google/googletest.git
cd googletest

# build from source
mkdir -p build
cd build
cmake ..
cmake --build .

# copy files into gojo lib and include dirs
cd ~/.gojo
mkdir -p lib/gtest
mkdir -p lib/gmock
mkdir -p include/gtest
mkdir -p include/gmock
cp repos/googletest/build/lib/*test*.a lib/gtest
cp repos/googletest/build/lib/*mock*.a lib/gmock
cp -r repos/googletest/googletest/include/gtest/* include/gtest
cp -r repos/googletest/googlemock/include/gmock/* include/gmock

echo "gtest downloaded. run 'source ~/.bashrc' to make changes visible"
