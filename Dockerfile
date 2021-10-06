FROM ubuntu:focal

ENV TZ America/Chicago
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y git cmake clang-10 llvm-10-tools lld-10 zlib1g-dev flex libboost-test-dev libgmp-dev libmpfr-dev libyaml-dev libjemalloc-dev curl maven pkg-config

ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g $GROUP_ID user && \
    useradd -m -u $USER_ID -s /bin/sh -g user user

USER $USER_ID:$GROUP_ID

ADD matching/pom.xml /home/user/.tmp-maven/
RUN    cd /home/user/.tmp-maven \
    && mvn dependency:go-offline
