#!/usr/bin/env bash
set -euxo pipefail

llvm_version="$1"
shift

mkdir build

pushd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DBUILD_TESTS=On -DCMAKE_INSTALL_PREFIX=install ..
make -j$(nproc) install
popd

pushd matching
mvn package
popd

export PATH="$(realpath ./build/install/bin):$(realpath ./build/bin):/usr/lib/llvm-${llvm_version}/bin:$PATH"
lit -v test
