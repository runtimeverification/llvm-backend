#!/usr/bin/env bash
set -euxo pipefail

llvm_version="$1"
shift

mkdir build

pushd build
cmake ..                                                  \
  -DCMAKE_C_COMPILER="$(which clang-${llvm_version})"     \
  -DCMAKE_CXX_COMPILER="$(which clang++-${llvm_version})" \
  -DCMAKE_BUILD_TYPE=Debug                                \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=On                      \
  -DBUILD_TESTS=On                                        \
  -DCMAKE_INSTALL_PREFIX=install
make -j$(nproc) install
make test
popd

pushd matching
mvn -B package
popd

export PATH="$(realpath ./build/install/bin):$(realpath ./build/bin):/usr/lib/llvm-${llvm_version}/bin:$PATH"
lit -v test
