let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) stdenv;
  inherit (pkgs) coreutils diffutils git ncurses gmp mpfr libffi jemalloc;

  default = import ./. { inherit pkgs; };
  inherit (default) clang llvm-backend llvm-backend-matching llvm-kompile-testing;

in

stdenv.mkDerivation {
  name = "llvm-backend-test";
  src = llvm-backend.src;
  preferLocalBuild = true;
  buildInputs = [
    diffutils git ncurses gmp mpfr libffi jemalloc
    clang llvm-backend llvm-kompile-testing
  ];
  configurePhase = "true";
  buildPhase = ''
    runHook preBuild

    mkdir -p build; cd build
    cp ../test/Makefile .
    make KOMPILE=llvm-kompile-testing clean
    make KOMPILE=llvm-kompile-testing -O -j$NIX_MAX_JOBS

    runHook postBuild
  '';
  installPhase = ''
    runHook preInstall

    mkdir -p "$out"
    cp -a -t "$out" .

    runHook postInstall
  '';
}

