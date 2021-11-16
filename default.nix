let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = [ ]; };
in

{ pkgs ? pinned

# Override `src` when this project is imported as a Git submodule:
#
# > ttuegel.cleanGitSubtree {
# >   name = "llvm-backend";
# >   src = ./parent/repo;
# >   subDir = "path/to/submodule";
# > };
#
# Use `cleanGitSubtree` whenever possible to preserve the same source code
# layout as the kframework/llvm-backend repository (to enable cache re-use).
#
, src ? null

# Build an optimized release package.
# Currently requires dependents to use LTO. Use sparingly.
, release ? false
}:

let
  localOverlay = import ./nix/overlay.nix;
  localPkgs = import sources."nixpkgs" {
    config = {};
    overlays = [
      (final: prev: {
        inherit release src;
        mavenix = import sources."mavenix" { inherit (prev) pkgs; };
      })
      localOverlay
    ];
  };
in {
  inherit (localPkgs)
    llvm-backend llvm-backend-matching llvm-kompile-testing;
  inherit (localPkgs) clang; # for compatibility
  inherit (localPkgs) mavenix devShell; # for CI
}

