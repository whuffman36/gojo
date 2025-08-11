#!/bin/bash


cargo build --release
strip target/release/gojo
mkdir -p ~/.gojo/bin
cp target/release/gojo ~/.gojo/bin

echo "Detecting operating system..."
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "linux detected"
    echo "export PATH=\"$HOME/.gojo/bin:\$PATH\"" >> ~/.bashrc
    source ~/.bashrc
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macos detected"
    echo "export PATH=\"$HOME/.gojo/bin:\$PATH\"" >> ~/.zshrc
    source ~/.zshrc
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi
