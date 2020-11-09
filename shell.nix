let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) mkShell;

  default = import ./. { inherit pkgs; };
  inherit (default) llvm-backend llvm-backend-matching;
  inherit (default) mavenix;

in mkShell {
  inputsFrom = [
    llvm-backend
    llvm-backend-matching
  ];

  buildInputs = [
    mavenix.cli
  ];
}
