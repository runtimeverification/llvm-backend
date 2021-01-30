{
  lib, cleanSourceWith, src,
  cmake, flex, pkgconfig,
  llvmPackages,
  boost, gmp, jemalloc, libffi, libyaml, mpfr,
}:

let inherit (llvmPackages) stdenv llvm; in

let
  pname = "llvm-backend";
  version = "0";
in

stdenv.mkDerivation {
  inherit pname version;
  src =
    # Avoid spurious rebuilds by ignoring files that don't affect the build.
    cleanSourceWith {
      name = "llvm-backend-src";
      inherit src;
      ignore = 
        [
          "/nix" "*.nix" "*.nix.sh"
          "/.github"
          "/matching"
        ];
    };

  nativeBuildInputs = [ cmake flex llvm pkgconfig ];
  buildInputs = [ boost gmp libffi libyaml jemalloc mpfr ];

  cmakeFlags = [
    ''-DCMAKE_C_COMPILER=${lib.getBin stdenv.cc}/bin/cc''
    ''-DCMAKE_CXX_COMPILER=${lib.getBin stdenv.cc}/bin/c++''
    ''-DUSE_NIX=TRUE''
  ];

  cmakeBuildType = "FastBuild";

  NIX_CFLAGS_COMPILE = [ "-Wno-error" ];

  doCheck = true;
  checkPhase = ''
    runHook preCheck

    # Find local test libraries. Nixpkgs' linker script filters paths outside
    # the Nix store, so the build-local libraries are not linked.
    (
        export LD_LIBRARY_PATH="''${LD_LIBRARY_PATH:+:}"'$ORIGIN/lib'
        make run-unittests
    )

    runHook postCheck
  '';
}
