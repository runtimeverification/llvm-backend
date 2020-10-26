let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) callPackage;

  llvm-backend = callPackage ./nix/llvm-backend.nix {
    llvmPackages = pkgs.llvmPackages_10;
  };

  self = {
    inherit llvm-backend;
  };

in self

