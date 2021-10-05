On Ubuntu 20.04:
```shell
sudo apt-get update
sudo apt-get install git cmake clang-10 llvm-10-tools lld-10 zlib1g-dev flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-dev libjemalloc-dev curl maven pkg-config
git clone https://github.com/kframework/llvm-backend --recursive
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
brew install boost cmake coreutils flex git gmp jemalloc libyaml llvm@12 maven mpfr pkg-config z3

git clone https://github.com/kframework/llvm-backend --recursive
cd llvm-backend

mkdir build
cd build

export PATH=$($(brew --prefix llvm)/bin/llvm-config --bindir):$PATH

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=install \
  -DFLEX_EXECUTABLE=$(brew --prefix flex)/bin/flex \
  -DLLVM_DIR=$($(brew --prefix llvm@12)/bin/llvm-config --cmakedir)

make -j16
make install
```

Then add `llvm-backend/build/install/bin` to your $PATH.

You can run the test suite with `./ciscript Debug`. You can also run it with a different CMake profile by replacing `Debug` with `RelWithDebInfo`, `Release`, `FastBuild`, or `GcStats`.
