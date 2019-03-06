On Ubuntu 18.04:

```
sudo apt-get update
sudo apt-get install git cmake clang-6.0 llvm-6.0-tools zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl libtinfo-dev libssl-dev pkg-config
sudo ./install-rust
curl -sSL https://get.haskellstack.org/ | sh
git clone https://github.com/kframework/llvm-backend
cd llvm-backend
git submodule update --init
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # or Debug
make -j16
make install
```

Then add `llvm-backend/build/install/bin` to your $PATH.
