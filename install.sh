#!bin/bash

cargo build --release
strip target/release/gojo
sudo cp target/release/gojo /usr/local/bin