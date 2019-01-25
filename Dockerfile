FROM ubuntu:bionic

RUN apt-get update
RUN apt-get install -y git cmake clang-6.0 zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y
ENV PATH "${HOME}/.cargo/bin:${PATH}"
RUN rustup toolchain install 1.28.0
RUN rustup default 1.28.0
RUN curl -sSL https://get.haskellstack.org/ | sh      
RUN mkdir -p ~/.stack
RUN echo 'allow-different-user: true' > ~/.stack/config.yaml
 
