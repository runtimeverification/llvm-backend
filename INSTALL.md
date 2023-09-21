# Dependencies

## Ubuntu 20.04:

```shell
sudo apt update
sudo apt install      \
  clang-12            \
  cmake               \
  curl                \
  flex                \
  git                 \
  libboost-dev        \
  libboost-test-dev   \
  libfmt-dev          \
  libgmp-dev          \
  libjemalloc-dev     \
  libmpfr-dev         \
  libyaml-dev         \
  lld-12              \
  llvm-12-tools       \
  maven               \
  pkg-config          \
  python3             \
  python3-pip         \
  xxd                 \
  zlib1g-dev
python3 -m pip install pybind11 lit
```

## macOS / Homebrew

```shell
brew update
brew install  \
  boost       \
  cmake       \
  flex        \
  fmt         \
  git         \
  gmp         \
  jemalloc    \
  libyaml     \
  llvm@15     \
  maven       \
  mpfr        \
  pkg-config  \
  python3     \
  z3
python3 -m pip install pybind11 lit
```

# Building

Once the system dependencies have been installed, the backend can be built
locally with:
```shell
git clone https://github.com/runtimeverification/llvm-backend --recursive
cd llvm-backend
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=FastBuild \
  -DCMAKE_INSTALL_PREFIX=install
make -j$(nproc) install
```

If you're building on macOS, add the following option to your CMake invocation
so that the Homebrew installation of LLVM gets picked up correctly.
```shell
-DLLVM_DIR=$($(brew --prefix llvm@15)/bin/llvm-config --cmakedir)
```

Additionally, to build the pattern-matching compiler, run:
```shell
cd matching
mvn package
```

# Running Tests

The backend test suite uses [`lit`](lit) to run a set of integration tests. To
ensure your `PATH` variable is set correctly, run:
```shell
export PATH=$(pwd)/build/install/bin:$(pwd)/build/bin:$PATH
```
from the root source directory.

To run the integration tests, run:
```shell
lit test
```
from the root source directory.

There is also a unit test suite for backend internals;
Add the following option to your CMake invocation to enable it:
```
-DBUILD_TESTS=On
```
run it with:
```shell
make test
```
from the `build` directory to run them.

# Code Checks

If you are developing the backend, it is useful to also install
[`clang-format`](clang-format), [`include-what-you-use`](iwyu)
[`shellcheck`](shellcheck) to ensure that your changes match the project style
and conform to best practices.

```shell
# Ubuntu
apt install shellcheck clang-format-12 iwyu

# macOS
brew install shellcheck clang-format iwyu
```

[clang-format]: https://clang.llvm.org/docs/ClangFormat.html
[iwyu]: https://include-what-you-use.org/
[lit]: https://llvm.org/docs/CommandGuide/lit.html
[shellcheck]: https://www.shellcheck.net/
