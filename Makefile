SHELL := /bin/zsh

.PHONY: build
.PHONY: test

config:
	@cmake -S . -B build/Debug -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wconversion -Wshadow -fno-exceptions $(CPPFLAGS)" -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" -DCMAKE_TOOLCHAIN_FILE=deps/build/Debug/generators/conan_toolchain.cmake -DBUILD_TESTING=ON

config-rel:
	@cmake -S . -B build/Release -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wconversion -Wshadow -fno-exceptions $(CPPFLAGS)" -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" -DCMAKE_TOOLCHAIN_FILE=deps/build/Release/generators/conan_toolchain.cmake -DBUILD_TESTING=ON

build:
	@cmake --build build/Debug --parallel

build-rel:
	@cmake --build build/Release --parallel

test:
	@cd build/Debug/test && ctest --output-on-failure
	@cd ../../../

fmt:
	@find src/ -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format -n

check:
	@run-clang-tidy -p build/Debug

install:
	@conan install . --deployer=full_deploy --output-folder=./deps --build=missing --profile=gojo -s build_type=Release -c tools.cmake.cmaketoolchain:user_presets=""
	@conan install . --deployer=full_deploy --output-folder=./deps --build=missing --profile=gojo -s build_type=Debug -c tools.cmake.cmaketoolchain:user_presets=""

branch:
	@git branch $(BRANCH)
	@git checkout $(BRANCH)
	@git branch --set-upstream-to=origin/main $(BRANCH)
	@git pull

clean:
	@rm -rf build
	@mkdir -p build
