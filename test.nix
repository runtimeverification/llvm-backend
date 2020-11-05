let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) mkShell;

  inherit (pkgs) coreutils diffutils git ncurses gmp mpfr libffi jemalloc;

  default = import ./. { inherit pkgs; };
  inherit (default) clang llvm-backend llvm-backend-matching;

in mkShell {
  inputsFrom = [
    llvm-backend-matching
  ];

  buildInputs = [
    coreutils diffutils git ncurses gmp mpfr libffi jemalloc
    clang
    llvm-backend
  ];
}
