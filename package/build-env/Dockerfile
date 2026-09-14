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
          debhelper                       \
          llvm-${LLVM_VERSION}-tools      \
          lld-${LLVM_VERSION}             \
          zlib1g-dev                      \
          flex                            \
          locales                         \
          libboost-dev                    \
          libboost-test-dev               \
          libfmt-dev                      \
          libgmp-dev                      \
          libmpfr-dev                     \
          libjemalloc-dev                 \
          libunwind-dev                   \
          libyaml-dev                     \
          openjdk-${JDK_VERSION}-jdk      \
          curl                            \
          maven                           \
          pkg-config                      \
          pipx                            \
          python3                         \
          python3-dev                     \
          python3-pybind11                \
          xxd

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# The build runs as an account whose uid/gid match the CALLER's, so that files
# written to the bind-mounted checkout are owned by whoever invoked the build.
# Only the ids matter for that; the names are ours to pick, and picking them
# ourselves is what keeps this working on an arbitrary machine.
#
# Passing the caller's names through, which is what this used to do, collides
# with whatever the base image already has: ubuntu:noble ships an `ubuntu`
# account at uid/gid 1000, and `users` (gid 100) is the primary group of many
# developer accounts. Either makes groupadd/useradd fail, several minutes into
# the build, with a message that does not mention the build args.
#
# -o permits the ids to be shared with an existing account rather than trying
# to delete one -- deleting a system group that something else already uses
# fails just as hard.
ARG USER=builder
ARG GROUP=$USER
ARG USER_ID=1000
ARG GROUP_ID=$USER_ID
RUN groupadd -o -g $GROUP_ID $GROUP && useradd -o -m -u $USER_ID -s /bin/sh -g $GROUP $USER

# dpkg-buildpackage writes its output to the PARENT of the directory it builds
# in, so the build has to run somewhere with a writable parent. Create that
# parent here, owned by the build user; package/build-env/build-deb then mounts the
# checkout at /opt/workspace/source rather than at /opt/workspace itself.
#
# Mounting the checkout directly at /opt/workspace makes the parent `/`, which
# a non-root user cannot write, and the failure surfaces from inside
# dpkg-buildpackage rather than from the mount that caused it.
RUN mkdir -p /opt/workspace && chown $USER_ID:$GROUP_ID /opt/workspace

USER $USER:$GROUP

RUN pipx install lit

ADD matching/pom.xml /home/user/.tmp-maven/
RUN    cd /home/user/.tmp-maven \
    && mvn -B dependency:go-offline

# Set $PATH to refer to LLVM's lit correctly
ENV PATH "$PATH:/home/$USER/.local/bin"
