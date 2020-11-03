#!/usr/bin/env nix-shell
#! nix-shell --pure test.nix -i bash

set -e -x -o pipefail

src=$(pwd)

pushd matching
mvn verify -U
popd

mkdir -p build; cd build

cp ../test/Makefile .

sed ../bin/llvm-kompile-testing \
  -e "s,/bin/bash,$SHELL," \
  -e "s,@PROJECT_SOURCE_DIR@,$src," \
  > llvm-kompile-testing
chmod +x llvm-kompile-testing

make KOMPILE=$src/build/llvm-kompile-testing clean
make KOMPILE=$src/build/llvm-kompile-testing -j$(nproc)
