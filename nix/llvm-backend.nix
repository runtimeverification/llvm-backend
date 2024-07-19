{ lib, src, cmake, flex, fmt, pkg-config, llvm, libllvm, libcxx, stdenv, boost, gmp
, jemalloc, libffi, libiconv, libunwind, libyaml, mpfr, ncurses, python310, unixtools,
# Runtime dependencies:
host,
# Options:
cmakeBuildType ? "FastBuild" # optimized release build, currently: LTO
}:
let python-env = (python310.withPackages (ps: with ps; [ pybind11 ])); in
stdenv.mkDerivation {
  pname = "llvm-backend";
  version = "${lib.removeSuffix "\n" (builtins.readFile ../package/version)}";

  inherit src cmakeBuildType;

  nativeBuildInputs = [ cmake flex llvm pkg-config ];
  buildInputs = [ libyaml ];
  propagatedBuildInputs = [
    boost fmt gmp libunwind jemalloc libffi mpfr ncurses python-env unixtools.xxd
  ] ++ lib.optional stdenv.isDarwin libiconv;

  dontStrip = true;

  postPatch = ''
    sed -i bin/llvm-kompile \
      -e '2a export PATH="${lib.getBin host.clang}/bin:''${PATH}"'

    substituteInPlace bin/utils.sh \
      --replace 'python3' '${python-env.interpreter}'

    substituteInPlace bin/llvm-kompile \
      --replace 'python_cmd=python3' 'python_cmd="${python-env.interpreter}"' \
      --replace 'extra_python_flags=()' \
                'extra_python_flags=($(${python-env}/bin/pybind11-config --includes))'

    substituteInPlace bin/llvm-kompile-clang \
      --replace '"-lgmp"' '"-I${gmp.dev}/include" "-L${gmp}/lib" "-lgmp"' \
      --replace '"-lmpfr"' '-I${mpfr.dev}/include "-L${mpfr}/lib" "-lmpfr"' \
      --replace '"-lffi"' '"-L${libffi}/lib" "-lffi"' \
      --replace '"-ljemalloc"' '"-L${jemalloc}/lib" "-ljemalloc"' \
      --replace '"-liconv"' '"-L${libiconv}/lib" "-liconv"' \
      --replace '"-lncurses"' '"-L${ncurses}/lib" "-lncurses"' \
      --replace '"-ltinfo"' '"-L${ncurses}/lib" "-ltinfo"' \
      --replace '"$libunwind"' '"-L${libunwind}/lib" "-lunwind"' \
      --replace '"-L@BREW_PREFIX@/opt/libffi/lib"' ' ' \
      --replace '"-L@LLVM_LIBRARY_DIR@"' ' ' \
      --replace '-L@BREW_PREFIX@/lib' '-L${libcxx}/lib' \
      --replace '-I "$(dirname "$0")"/../include/kllvm' \
                '-I "$(dirname "$0")"/../include/kllvm -I ${boost.dev}/include -I ${fmt.dev}/include'
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
    "-DK_LLVM_BACKEND_LTO=OFF"
  ];

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

  python-interpreter = python-env.interpreter;
}
