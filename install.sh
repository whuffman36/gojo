#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "linux detected"
    # Install dependencies

    which -s git
    if [[ $? != 0 ]] ; then
      apt install -y git
    fi

    which -s g++
    if [[ $? != 0 ]] ; then
      apt install -y g++
    fi

    which -s curl
    if [[ $? != 0 ]] ; then
      apt install -y curl
    fi

    which -s cmake
    if [[ $? != 0 ]] ; then
      apt install -y cmake
    fi

    which -s cargo
    if [[ $? != 0 ]] ; then
      curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | bash -s -- -y
    fi

    source ~/.bashrc
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macos detected"
    # Install dependencies

    xcode-select -p 1>/dev/null
    if [[ $? != 2 ]] ; then
      xcode-select --install
    fi

    which -s brew
    if [[ $? != 0 ]] ; then
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        brew update
    fi

    echo "export PATH=\"/opt/homebrew/bin:\$PATH\"" >> ~/.zshenv
    source ~/.zshenv

    which -s git
    if [[ $? != 0 ]] ; then
      brew install -y git
    fi

    which -s cmake
    if [[ $? != 0 ]] ; then
      brew install -y cmake
    fi

    which -s cargo
    if [[ $? != 0 ]] ; then
      curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | bash -s -- -y
    fi

    source ~/.zshenv
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

# Create gojo directory.
mkdir -p ~/.gojo
mkdir -p ~/.gojo/repos/gojo
mkdir ~/.gojo/include
mkdir ~/.gojo/lib
mkdir ~/.gojo/bin

# Clone gojo repo and build from source.
# ONLY FOR DOCKER TEST
cp -r . ~/.gojo/repos/gojo
cd ~/.gojo/repos/gojo
# END ONLY FOR DOCKER TEST
#git clone <url of gojo repo>
#cd gojo
~/.cargo/bin/cargo build --release
strip target/release/gojo
mv target/release/gojo ~/.gojo/bin

# Clone gojo++ repo and build from source.
#cd ~/.gojo/repos
#git clone <url of gojo++ repo>
#cd gojo++
#cargo build --release
#strip target/release/gojo++
#mv target/release/gojo++ ~/.gojo/bin

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "export PATH=\"$HOME/.gojo/bin:\$PATH\"" >> ~/.bashrc
    source ~/.bashrc
    echo ""
    echo "gojo installed successfully"
fi
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "export PATH=\"$HOME/.gojo/bin:\$PATH\"" >> ~/.zshenv
    source ~/.zshenv
    echo ""
    echo "gojo installed successfully"
fi
