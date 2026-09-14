# Shared by build-deb and test-deb. Source it; it is not executable.

# The one table. The LLVM version is a property of the Ubuntu release, not a
# free choice: each release carries one clang/lld/llvm-tools set that the
# package is built and tested against. Adding a release means editing here and
# in the CI matrix, and nowhere else.
llvm_version_for() {
    case "${1}" in
        jammy) echo 15 ;;
        noble) echo 17 ;;
        *)     return 1 ;;
    esac
}

SUPPORTED_DISTROS='jammy noble'

# Both scripts take a distro as their first argument and must agree on which
# ones exist -- a list kept in two places is a list that drifts.
require_distro() {
    if [ -z "${1}" ]; then
        echo "${0##*/}: usage: ${0##*/} <${SUPPORTED_DISTROS// /|}> [package.deb]" >&2
        exit 2
    fi
    if ! llvm_version_for "${1}" > /dev/null; then
        echo "${0##*/}: unknown distro '${1}' (expected: ${SUPPORTED_DISTROS})" >&2
        exit 2
    fi
}

default_pkg_name() {
    echo "k-llvm-backend_amd64_ubuntu_${1}.deb"
}

# podman is what RV uses; docker is accepted so this still runs on a machine
# that only has that. CONTAINER_ENGINE overrides both.
container_engine() {
    if [ -n "${CONTAINER_ENGINE:-}" ]; then
        echo "${CONTAINER_ENGINE}"
    elif command -v podman > /dev/null 2>&1; then
        echo podman
    elif command -v docker > /dev/null 2>&1; then
        echo docker
    else
        echo "${0##*/}: no container engine found (looked for podman, docker)" >&2
        exit 1
    fi
}
