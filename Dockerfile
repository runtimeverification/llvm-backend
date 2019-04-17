FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y git cmake clang-6.0 llvm-6.0-tools lld-6.0 zlib1g-dev bison flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-cpp-dev libjemalloc-dev curl
RUN curl -sSL https://get.haskellstack.org/ | sh      

ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g $GROUP_ID user && \
    useradd -m -u $USER_ID -s /bin/sh -g user user

USER $USER_ID:$GROUP_ID

ADD install-rust rust-checksum /home/user/
RUN cd /home/user/ && ./install-rust

ADD matching-scala/pom.xml /home/user/.tmp-maven/
RUN    cd /home/user/.tmp-maven \
    && mvn dependency:go-offline
