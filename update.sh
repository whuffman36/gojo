#!bin/bash

git pull
cargo build --release
sudo rm -rf /usr/local/bin/gojo
sudo cp target/release/gojo /usr/local/bin