#!/usr/bin/env bash
set -euxo pipefail

mkdir build

pushd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DBUILD_TESTS=On -DCMAKE_INSTALL_PREFIX=install ..
make -j$(nproc) install
popd

pushd matching
mvn package
popd

export PATH="$(realpath ./build/install):$PATH"
lit -av test