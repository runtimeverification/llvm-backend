ARG BASE_OS
ARG BASE_DISTRO

FROM ${BASE_OS}:${BASE_DISTRO}

ENV TZ America/Chicago
ENV DEBIAN_FRONTEND=noninteractive

ARG LLVM_VERSION
ARG JDK_VERSION

RUN     apt-get update                    \
    &&  apt-get upgrade -y                \
    &&  apt-get install -y                \
          git                             \
          cmake                           \
          clang-${LLVM_VERSION}           \
          clang-tidy-${LLVM_VERSION}      \
          llvm-${LLVM_VERSION}-tools      \
          lld-${LLVM_VERSION}             \
          zlib1g-dev                      \
          flex                            \
          locales                         \
          libboost-test-dev               \
          libfmt-dev                      \
          libgmp-dev                      \
          libmpfr-dev                     \
          libyaml-dev                     \
          libjemalloc-dev                 \
          openjdk-${JDK_VERSION}-jdk      \
          curl                            \
          maven                           \
          pkg-config                      \
          python3                         \
          python3-pip

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# The image is built specifically for an environment with this user/group
ARG USER=github-user
ARG GROUP=$USER
ARG USER_ID=1000
ARG GROUP_ID=$USER_ID
RUN groupadd -g $GROUP_ID $GROUP && useradd -m -u $USER_ID -s /bin/sh -g $GROUP $USER

USER $USER:$GROUP

RUN    pip3 install --user --upgrade pip  \
    && ~/.local/bin/pip3 install --user lit

ADD matching/pom.xml /home/user/.tmp-maven/
RUN    cd /home/user/.tmp-maven \
    && mvn -B dependency:go-offline

# Set $PATH to refer to LLVM's lit correctly
ENV PATH "$PATH:/home/$USER/.local/bin"
