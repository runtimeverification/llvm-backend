# Dependencies

## Ubuntu 22.04:

```shell
sudo apt update
sudo apt install      \
  clang-15            \
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
  libunwind-dev       \
  libyaml-dev         \
  lld-15              \
  llvm-15-tools       \
  maven               \
  openjdk-17-jdk      \
  pkg-config          \
  python3             \
  python3-pip         \
  xxd
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
  grep        \
  jemalloc    \
  libyaml     \
  llvm@15     \
  maven       \
  mpfr        \
  pkg-config  \
  python3     \
  z3
```

To ensure that the backend can use pybind11 correctly, we must create an virtual
environment and install the `pybind11` package:
```shell
python3 -m venv venv
source venv/bin/activate
python3 -m pip install pybind11 lit
```

Some tests rely on GNU Grep options, which are not available on macOS by
default. To ensure that the tests run correctly, you add the path of
GNU Grep to your `PATH` in your shell profile:
```shell
export PATH=/opt/homebrew/Cellar/grep/3.11/libexec/gnubin/:$PATH
```

# Building

## Environment Variables

If you're building on macOS, type the following command or epermanently
add it your `env` (`.zshrc`, `.bashrc`, etc.), so that the Homebrew
installation of LLVM gets picked up correctly. We recommend adding it to
your shell profile.
```shell
export LLVM_DIR=$($(brew --prefix llvm@15)/bin/llvm-config --cmakedir)
```

If you don't usually use the `clang` from your Homebrew installation as
your default compiler, you can set the following CMake flg to use these
`clang` and `clang++`:
```shell
-DCMAKE_C_COMPILER="$(brew --prefix llvm@15)/bin/clang" \
-DCMAKE_CXX_COMPILER="$(brew --prefix llvm@15)/bin/clang++"
```
Once again, we recommend adding them and other llvm binaries to your
`PATH` in your shell profile:
```shell
export PATH="$(brew --prefix llvm@15)/bin:$PATH"
```

Some tests rely on GNU Grep options, which are not available on macOS by
default. To ensure that the tests run correctly, you can create an alias
for GNU Grep:
```shell
alias grep=ggrep
```

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
from the root source directory. You can use `-v` which test is being executed
and the output of failling tests.

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
apt install shellcheck clang-format-15 iwyu

# macOS
brew install shellcheck clang-format iwyu
```

[clang-format]: https://clang.llvm.org/docs/ClangFormat.html
[iwyu]: https://include-what-you-use.org/
[lit]: https://llvm.org/docs/CommandGuide/lit.html
[shellcheck]: https://www.shellcheck.net/
