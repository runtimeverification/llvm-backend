let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" {
    config = { };
    overlays = [ ];
  };

in { pkgs ? pinned

  # Override `src` when this project is imported as a Git submodule:
  #
  # > ttuegel.cleanGitSubtree {
  # >   name = "llvm-backend";
  # >   src = ./parent/repo;
  # >   subDir = "path/to/submodule";
  # > };
  #
  # Use `cleanGitSubtree` whenever possible to preserve the same source code
  # layout as the runtimeverification/llvm-backend repository (to enable cache re-use).
  #
, src ? null

  # Build an optimized release package.
  # Currently requires dependents to use LTO. Use sparingly.
, release ? false }:

let
  localOverlay = import ./nix/overlay.nix;

  localPkgs = import sources."nixpkgs" {
    config = { };
    overlays = [
      (final: prev:
        let
          ttuegel = import sources."ttuegel" { inherit (prev) pkgs; };
          # Avoid spurious rebuilds by ignoring files that don't affect the build.
          mavenix = import sources."mavenix" { inherit (prev) pkgs; };
        in {
          llvm-backend-release = release;
          inherit (mavenix) buildMaven;
          mavenix-cli = mavenix.cli;

          llvm-backend-src = ttuegel.orElse src (ttuegel.cleanSourceWith {
            name = "llvm-backend-src";
            src = ./.;
            ignore = [ "/nix" "*.nix" "*.nix.sh" "/.github" ];
          });

          llvm-backend-matching-src = ttuegel.cleanSourceWith {
            name = "llvm-backend-matching-src";
            src = ttuegel.orElse src (ttuegel.cleanGitSubtree {
              name = "llvm-backend-src";
              src = ./.;
            });
            subDir = "matching";
          };

        })
      localOverlay
    ];
  };
in {
  inherit (localPkgs) llvm-backend llvm-backend-matching integration-tests;
  inherit (localPkgs) clang; # for compatibility
  inherit (localPkgs) mavenix-cli; # for CI
}

