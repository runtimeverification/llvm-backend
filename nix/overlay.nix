final: prev:
let
  mkLlvmPackages = (packages:
    packages.override {
      bootBintoolsNoLibc = null;
      bootBintools = null;
    });

  llvmPackages =
    mkLlvmPackages prev."llvmPackages_${toString prev.llvm-version}";

  clang = llvmPackages.clangNoLibcxx;

  llvm-backend = prev.callPackage ./llvm-backend.nix {
    inherit (llvmPackages) llvm libllvm libcxxabi;
    stdenv = llvmPackages.stdenv;
    cmakeBuildType = prev.llvm-backend-build-type;
    src = prev.llvm-backend-src;
    host.clang = clang;
  };

  llvm-backend-matching = import ./llvm-backend-matching.nix {
    src = prev.llvm-backend-matching-src;
    mvnHash = "sha256-HF6BXeCnV5I7+oRVGK8DGHjaAtHWLfEaCwtkVcQHoGU";
    inherit (final) maven;
  };

  # This code is a bit of a hack to get Nix to accept the binding library being
  # defined in terms of the output of the LLVM backend. Using the backend's
  # lib/python directory directly causes a provenance error when reading the
  # pyproject file.
  kllvm = prev.poetry2nix.mkPoetryApplication {
    python = prev.python310;
    projectDir = ../bindings/python/package;
    postInstall = ''
      cp ${llvm-backend}/lib/python/kllvm/* $out/lib/python3.10/site-packages/kllvm/
    '';
  };

  llvm-kompile-testing = let
    inherit (prev.pkgs) runCommandNoCC jre;
    java = "${jre}/bin/java";
    inherit (llvm-backend-matching) jar;
  in runCommandNoCC "llvm-kompile-testing" { } ''
    mkdir -p "$out/bin"
    cp ${llvm-backend.src}/bin/llvm-kompile-testing "$out/bin"
    sed -i "$out/bin/llvm-kompile-testing" \
        -e '/@PROJECT_SOURCE_DIR@/ c ${java} -jar ${jar} $definition qbaL $dt_dir 1'
    substituteInPlace $out/bin/llvm-kompile-testing \
      --replace 'llvm-kompile' '${llvm-backend}/bin/llvm-kompile'
    chmod +x "$out/bin/llvm-kompile-testing"
    patchShebangs "$out/bin/llvm-kompile-testing"
  '';

  integration-tests = prev.stdenv.mkDerivation {
    name = "llvm-backend-integration-tests";
    src = llvm-backend.src;
    preferLocalBuild = true;
    buildInputs = [
      prev.diffutils # for golden testing
      prev.lit
      prev.clang
      llvm-kompile-testing # for constructing test input without the frontend
      llvm-backend # the system under test
    ];
    configurePhase = "true";
    buildPhase = ''
      runHook preBuild

      PYTHON_INTERPRETER=${llvm-backend.python-interpreter} \
      BINDINGS_INSTALL_PATH=${llvm-backend}/lib/kllvm/python \
      INCLUDE_INSTALL_PATH=${llvm-backend}/include \
        LIT_USE_NIX=1 lit -v test

      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall

      mkdir -p "$out"
      cp -a -t "$out" .

      runHook postInstall
    '';
  };
in {
  inherit kllvm llvm-backend llvm-backend-matching llvm-kompile-testing
    integration-tests;
  inherit (prev) clang; # for compatibility
}
