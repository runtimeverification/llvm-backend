FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y git cmake clang-6.0 llvm-6.0-tools zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl
RUN curl -sSL https://get.haskellstack.org/ | sh      

ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g $GROUP_ID user && \
    useradd -m -u $USER_ID -s /bin/sh -g user user

USER $USER_ID:$GROUP_ID

ADD install-rust rust-checksum /home/user/
RUN cd /home/user/ && ./install-rust

ENV LC_ALL=C.UTF-8
ADD --chown=user:user matching/stack.yaml /home/user/.tmp-haskell2/
ADD --chown=user:user matching/package.yaml /home/user/.tmp-haskell2/
ADD --chown=user:user matching/submodules/kore/stack.yaml /home/user/.tmp-haskell2/submodules/kore/
ADD --chown=user:user matching/submodules/kore/src/main/haskell/kore/package.yaml /home/user/.tmp-haskell2/submodules/kore/src/main/haskell/kore/
RUN    cd /home/user/.tmp-haskell2 \
    && stack build --only-snapshot --test
