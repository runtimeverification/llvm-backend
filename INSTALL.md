On Ubuntu 18.04:

```
sudo apt-get update
sudo apt-get install git cmake clang-8 llvm-8-tools lld-8 zlib1g-dev flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-dev libjemalloc-dev curl maven pkg-config
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
