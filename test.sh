#!/bin/bash

rm -rf test
mkdir test
cargo build --release
cd test
../target/release/gojo init cumshot
cd cumshot
../../target/release/gojo build -t
#../../target/release/gojo fmt
#../../target/release/gojo check