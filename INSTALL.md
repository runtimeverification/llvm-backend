On Ubuntu 20.04:
```shell
sudo apt-get update
sudo apt-get install git cmake clang-12 llvm-12-tools lld-12 zlib1g-dev flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-dev libjemalloc-dev curl maven pkg-config
git clone https://github.com/runtimeverification/llvm-backend --recursive
cd llvm-backend
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install # or Debug
make -j16
make install
```

On macos 11 (Big Sur), using Homebrew:
```shell
brew update
brew install boost cmake coreutils flex git gmp jemalloc libyaml llvm@13 maven mpfr pkg-config z3

git clone https://github.com/runtimeverification/llvm-backend --recursive
cd llvm-backend

mkdir build
cd build

export PATH=$($(brew --prefix llvm)/bin/llvm-config --bindir):$PATH

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=install \
  -DFLEX_EXECUTABLE=$(brew --prefix flex)/bin/flex \
  -DLLVM_DIR=$($(brew --prefix llvm@13)/bin/llvm-config --cmakedir)

make -j16
make install
```

Then add `llvm-backend/build/install/bin` to your $PATH.

You can run the test suite with `./ciscript Debug`. You can also run it with a different CMake profile by replacing `Debug` with `RelWithDebInfo`, `Release`, `FastBuild`, or `GcStats`.

If you are developing the backend, it is useful to also install
[`clang-format`](clang-format) and [`shellcheck`](shellcheck) to ensure that
your changes match the project style and conform to best practices.

```shell
# Ubuntu
apt install shellcheck clang-format-12

# macOS
brew install shellcheck clang-format
```

[clang-format]: https://clang.llvm.org/docs/ClangFormat.html
[shellcheck]: https://www.shellcheck.net/
