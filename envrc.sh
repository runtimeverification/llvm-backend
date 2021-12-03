# Template .envrc for direnv (https://github.com/direnv/direnv)
#
# Make K Framework tools and built products available in shells and editors.
#
# Usage:
#   1. Install direnv (refer to direnv's documentation).
#   2. Copy this file to .envrc: cp envrc.sh .envrc
#   3. Edit .envrc if required for your environment.
#   4. Load with direnv: direnv allow

# If you're using a local build of K, then the K_ROOT environment variable
# should point to the root directory of the K checkout.
if [[ -v K_ROOT ]]; then
  PATH_add "$K_ROOT/k-distribution/bin"
fi

# This is the correct path for LLVM 12 installed on Ubuntu 21.04; it might not
# be elsewhere. The directory should contain a valid llvm-config executable.
PATH_add "/usr/lib/llvm-12/bin"

PATH_add "$(pwd)/build/bin"
PATH_add "$(pwd)/build/install/bin"
