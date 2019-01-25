FROM ubuntu:bionic

RUN apt-get update
RUN apt-get install -y git cmake clang-6.0 zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl
RUN curl -sSL https://get.haskellstack.org/ | sh      
