FROM ubuntu:focal

ENV TZ America/Chicago
ENV DEBIAN_FRONTEND=noninteractive

RUN     apt-get update      \
    &&  apt-get upgrade -y  \
    &&  apt-get install -y  \
          git               \
          cmake             \
          clang-10          \
          llvm-10-tools     \
          lld-10            \
          zlib1g-dev        \
          flex              \
          libboost-test-dev \
          libgmp-dev        \
          libmpfr-dev       \
          libyaml-dev       \
          libjemalloc-dev   \
          libunwind-dev     \
          curl              \
          maven             \
          pkg-config        \
          python3           \
          python3-pip

ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g $GROUP_ID user && \
    useradd -m -u $USER_ID -s /bin/sh -g user user

USER $USER_ID:$GROUP_ID

RUN    pip3 install --user --upgrade pip  \
    && ~/.local/bin/pip3 install --user lit

ADD matching/pom.xml /home/user/.tmp-maven/
RUN    cd /home/user/.tmp-maven \
    && mvn dependency:go-offline

# Set $PATH to refer to LLVM's lit correctly
ENV PATH "$PATH:/home/user/.local/bin"
