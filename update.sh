#!bin/bash

git pull
cargo build --release
strip target/release/gojo
sudo rm -rf /usr/local/bin/gojo
sudo cp target/release/gojo /usr/local/bin