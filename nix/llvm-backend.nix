{ lib, src, cmake, flex, pkgconfig, llvm, libllvm, libcxxabi, stdenv, boost, gmp
, jemalloc, libffi, libiconv, libyaml, mpfr, ncurses, python3,
# Runtime dependencies:
host,
# Options:
release ? false # optimized release build, currently: LTO
}:
stdenv.mkDerivation {
  pname = "llvm-backend";
  version = "0";
  inherit src;

  nativeBuildInputs = [ cmake flex llvm pkgconfig python3 ];
  buildInputs = [ boost libyaml ];
  propagatedBuildInputs = [ gmp jemalloc libffi mpfr ncurses ]
    ++ lib.optional stdenv.isDarwin libiconv;

  postPatch = ''
    sed -i bin/llvm-kompile \
      -e '2a export PATH="${lib.getBin host.clang}/bin:''${PATH}"'

    substituteInPlace bin/llvm-kompile-clang \
      --replace '"-lgmp"' '"-L${gmp}/lib" "-lgmp"' \
      --replace '"-lmpfr"' '"-L${mpfr}/lib" "-lmpfr"' \
      --replace '"-lffi"' '"-L${libffi}/lib" "-lffi"' \
      --replace '"-ljemalloc"' '"-L${jemalloc}/lib" "-ljemalloc"' \
      --replace '"-liconv"' '"-L${libiconv}/lib" "-liconv"' \
      --replace '"-lncurses"' '"-L${ncurses}/lib" "-lncurses"' \
      --replace '"-ltinfo"' '"-L${ncurses}/lib" "-ltinfo"' \
      --replace '"-L@BREW_PREFIX@/opt/libffi/lib"' ' ' \
      --replace '-L@BREW_PREFIX@/lib' '-L${libcxxabi}/lib'
  '';

  cmakeFlags =
    let isArmMac = stdenv.isDarwin && stdenv.hostPlatform.isAarch64;
    in [
    "-DCMAKE_C_COMPILER=${lib.getBin stdenv.cc}/bin/cc"
    "-DCMAKE_CXX_COMPILER=${lib.getBin stdenv.cc}/bin/c++"
    "-DLLVM_CLANG_PATH=${lib.getBin host.clang}/bin/clang"
    "-DLLVM_CONFIG_PATH=${lib.getBin libllvm.dev}/bin/llvm-config"
    "-DUSE_NIX=TRUE"
    "-DCMAKE_SKIP_BUILD_RPATH=FALSE"
    "-DBUILD_TESTS=True"
    "-DK_LLVM_BACKEND_LTO=${if isArmMac then "OFF" else "ON"}"
  ];

  cmakeBuildType = if release then "Release" else "FastBuild";

  NIX_CFLAGS_COMPILE = [ "-Wno-error" ];

  doCheck = true;
  checkPhase = ''
    runHook preCheck

    (
      # Allow linking to paths outside the Nix store.
      # Primarily, this allows linking to paths in the build tree.
      # The setting is only applied to the unit tests, which are not installed.
      export NIX_ENFORCE_PURITY=0
      make test
    )

    runHook postCheck
  '';

  passthru = { inherit (host) clang; };
}
