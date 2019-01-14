System Dependencies
===================

Ubuntu 18.04
------------

```sh
sudo apt-get install git cmake clang-6.0 clang++-6.0 llvm-6.0 zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl
curl https://sh.rustup.rs -sSf | sh
source $HOME/.cargo/env
rustup toolchain install 1.28.0
rustup default 1.28.0
curl -sSL https://get.haskellstack.org/ | sh
```

and setup your environment with:

```sh
export CC=clang-6.0
export CXX=clang++-6.0
```

Arch
----

```sh
sudo pacman -Syu
sudo pacman -S cmake git clang llvm llvm-libs bison flex boost gmp mpfr libyaml yaml-cpp jemalloc curl zlib
rustup install stable
rustup default stable
curl -sSL https://get.haskellstack.org/ | sh
```

and setup your environment with:

```sh
export CC=clang
export CXX=clang++
```

Building
========

Make sure the environment variables `CC` and `CCX` are set as described above, then build with:

```sh
git submodule update --init
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # or Debug
make -j16
make install
```

Finally, setup your `PATH` environment variable:

```sh
export PATH=$PATH:/path/to/llvm-backend/build/install/bin
```
