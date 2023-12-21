#!/usr/bin/env bash

set -e

LLVM_VERSION=15
BUILD_DIR=build

if [[ "$OSTYPE" == "darwin"* ]]; then
  clang_tidy="$(brew --prefix "llvm@${LLVM_VERSION}")/bin/clang-tidy"
  driver="$(brew --prefix "llvm@${LLVM_VERSION}")/bin/run-clang-tidy"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
  clang_tidy="clang-tidy-$LLVM_VERSION"
  driver="run-clang-tidy-$LLVM_VERSION"
fi

source_dirs=(
  bindings
  include
  lib
  runtime
  tools
)

mapfile -t inputs < <(find "${source_dirs[@]}" -name '*.cpp' -or -name '*.h')

"${driver}"                           \
  "${inputs[@]}"                      \
  -clang-tidy-binary "${clang_tidy}"  \
  -j "$(nproc)"                       \
  -p "${BUILD_DIR}" "$@"
