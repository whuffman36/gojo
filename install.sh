#!bin/bash

cargo build --release
sudo cp target/release/gojo /usr/local/bin