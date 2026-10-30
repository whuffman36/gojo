.PHONY: config config-rel build build-rel test test-rel fmt check install-dbg install-res install branch clean 

config:
	@cmake -S . -B build/Debug -G Ninja -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wconversion -Wshadow $(CPPFLAGS)" -DCMAKE_TOOLCHAIN_FILE=deps/build/Debug/generators/conan_toolchain.cmake -DBUILD_TESTING=ON -DEXE_NAME=gojo

config-rel:
	@cmake -S . -B build/Release -G Ninja -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wconversion -Wshadow $(CPPFLAGS)" -DCMAKE_TOOLCHAIN_FILE=deps/build/Release/generators/conan_toolchain.cmake -DBUILD_TESTING=ON -DEXE_NAME=gojo

build:
	@cmake --build build/Debug --parallel

build-rel:
	@cmake --build build/Release --parallel

test:
	@cd build/Debug/test && ctest --output-on-failure
	@cd ../../../

test-rel:
	@cd build/Release/test && ctest --output-on-failure
	@cd ../../../

fmt:
	@find src/ -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format -i -style=file
	@find test/ -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format -i -style=file

check:
	@run-clang-tidy -p build/Debug

check-rel:
	@run-clang-tidy -p build/Release

install-dbg:
	@conan install . --deployer=full_deploy --output-folder=./deps --build=missing --profile=gojo -s build_type=Release -c tools.cmake.cmaketoolchain:user_presets=""
	@conan install . --deployer=full_deploy --output-folder=./deps --build=missing --profile=gojo -s build_type=Debug -c tools.cmake.cmaketoolchain:user_presets=""

install-rel:
	@conan install . --deployer=full_deploy --output-folder=./deps --build=missing --profile=gojo -s build_type=Release -c tools.cmake.cmaketoolchain:user_presets=""

branch:
	@git branch $(BRANCH)
	@git checkout $(BRANCH)
	@git branch --set-upstream-to=origin/main $(BRANCH)
	@git pull

renew:
	@git checkout main
	@git pull
	@git branch -D $(BRANCH)
	@git branch

prepare: fmt config-rel build-rel test-rel check-rel

clean:
	@rm -rf build
	@mkdir -p build

search:
	grep -rnw '.' -e '$(PATTERN)'

# Fuck it just put it in both bash and zsh rc files
install: install-rel
	@cmake -S . -B build/Release -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wconversion -Wshadow $(CPPFLAGS)" -DCMAKE_TOOLCHAIN_FILE=deps/build/Release/generators/conan_toolchain.cmake -DBUILD_TESTING=OFF -DEXE_NAME=gojo
	@cmake --build build/Release --parallel
	@mkdir -p $(HOME)/.gojo/bin
	@mkdir -p $(HOME)/.gojo/profiles
	@mkdir -p $(HOME)/.gojo/formats
	@cp build/Release/src/gojo $(HOME)/.gojo/bin/gojo
	@cp install/gojo-c-format $(HOME)/.gojo/formats/gojo-c
	@cp install/gojo-cpp-format $(HOME)/.gojo/formats/gojo-c++
	@cp install/gojo-upgrade $(HOME)/.gojo/bin/gojo-upgrade
	@echo "export PATH=\"\$$PATH:$(HOME)/.gojo/bin\"" >> $(HOME)/.zshrc
	@echo "export PATH=\"\$$PATH:$(HOME)/.gojo/bin\"" >> $(HOME)/.bashrc
	@echo ""
	@echo "Gojo has successfully been installed!"
	@echo "Run 'source ~/.bashrc' or 'source ~/.zshrc' or start a new terminal session to use it"
	@echo ""
