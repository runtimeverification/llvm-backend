#!/usr/bin/env bash

set -euxo pipefail

BUILD_DIR=build

# Try to detect available LLVM version
if [[ "$OSTYPE" == "darwin"* ]]; then
  # On macOS, try LLVM 17 first, then fall back to 15
  for version in 17 15; do
    if brew list "llvm@${version}" >/dev/null 2>&1; then
      LLVM_VERSION=$version
      clang_tidy="$(brew --prefix "llvm@${version}")/bin/clang-tidy"
      driver="$(brew --prefix "llvm@${version}")/bin/run-clang-tidy"
      break
    fi
  done
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # On Linux, try different versions based on what's available
  for version in 17 16 15 14; do
    if command -v "clang-tidy-${version}" >/dev/null 2>&1 && command -v "run-clang-tidy-${version}" >/dev/null 2>&1; then
      LLVM_VERSION=$version
      clang_tidy="clang-tidy-${version}"
      driver="run-clang-tidy-${version}"
      break
    fi
  done
  
  # Fallback to default clang-tidy if no versioned one is found
  if [[ -z "${LLVM_VERSION:-}" ]]; then
    if command -v clang-tidy >/dev/null 2>&1 && command -v run-clang-tidy >/dev/null 2>&1; then
      LLVM_VERSION="default"
      clang_tidy="clang-tidy"
      driver="run-clang-tidy"
    else
      echo "Error: No clang-tidy installation found"
      exit 1
    fi
  fi
fi

if [[ -z "${LLVM_VERSION:-}" ]]; then
  echo "Error: No suitable LLVM/clang-tidy installation found"
  exit 1
fi

echo "Using LLVM version: ${LLVM_VERSION}"

source_dirs=(
  bindings
  include
  lib
  runtime
  tools
)

mapfile -t inputs < <(find "${source_dirs[@]}" -name '*.cpp')

"${driver}"                           \
  "${inputs[@]}"                      \
  -header-filter '(include/kllvm/)|(include/runtime/*.h)'              \
  -clang-tidy-binary "${clang_tidy}"  \
  -j "$(nproc)"                       \
  -p "${BUILD_DIR}" "$@"              \
  2>&1 | awk '!/(^Suppressed|^Use -header|^[0-9]+ warnings)/'
