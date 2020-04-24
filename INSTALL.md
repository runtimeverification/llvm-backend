On Ubuntu 20.04:

```
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

Then add `llvm-backend/build/install/bin` to your $PATH.
