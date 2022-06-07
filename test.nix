let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" {
    config = { };
    overlays = [ ];
  };

in { pkgs ? pinned }:

let
  inherit (pkgs) stdenv;
  inherit (pkgs) diffutils;

  default = import ./. { inherit pkgs; };

in default.integration-tests

