let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) callPackage nix-gitignore;

  llvm-backend = callPackage ./nix/llvm-backend.nix {
    inherit (nix-gitignore) gitignoreSource;
    llvmPackages = pkgs.llvmPackages_10;
  };

  mavenix = import sources."mavenix" { inherit pkgs; };

  llvm-backend-matching = import ./nix/llvm-backend-matching.nix {
    inherit mavenix;
  };

  self = {
    inherit llvm-backend llvm-backend-matching;
    inherit mavenix;
  };

in self

