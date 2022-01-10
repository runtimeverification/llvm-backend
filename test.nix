let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) stdenv;
  inherit (pkgs) diffutils;

  default = import ./. { inherit pkgs; };
  inherit (default) llvm-backend llvm-kompile-testing;

in

stdenv.mkDerivation {
  name = "llvm-backend-test";
  src = llvm-backend.src;
  preferLocalBuild = true;
  buildInputs = [
    diffutils  # for golden testing
    pkgs.lit
    llvm-kompile-testing  # for constructing test input without the frontend
    llvm-backend  # the system under test
  ];
  configurePhase = "true";
  buildPhase = ''
    runHook preBuild

    LIT_USE_NIX=1 lit -v test

    runHook postBuild
  '';
  installPhase = ''
    runHook preInstall

    mkdir -p "$out"
    cp -a -t "$out" .

    runHook postInstall
  '';
}

