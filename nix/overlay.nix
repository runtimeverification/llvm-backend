final: prev:
let
  llvmPackages = prev.llvmPackages_13.override {
    bootBintoolsNoLibc = null;
    bootBintools = null;
  };

  clang = if !llvmPackages.stdenv.targetPlatform.isDarwin then
    llvmPackages.clangNoLibcxx.override (attrs: {
      extraBuildCommands = ''
        ${attrs.extraBuildCommands}
        sed -i $out/nix-support/cc-cflags -e '/^-nostdlib/ d'
      '';
    })
  else
    llvmPackages.libcxxClang.overrideAttrs (old: {
      # Hack from https://github.com/NixOS/nixpkgs/issues/166205 for macOS
      postFixup = old.postFixup + ''
        echo "-lc++abi" >> $out/nix-support/libcxx-ldflags
      '';
    });

  jemalloc = prev.jemalloc.overrideDerivation (oldAttrs: rec {
    # Some tests for jemalloc fail on the M1! Our tests seem to pass but this may be flaky
    doCheck = false;
    stdenv = prev.stdenv;
  });

  llvm-backend = prev.callPackage ./llvm-backend.nix {
    inherit (llvmPackages) llvm libllvm libcxxabi;
    stdenv = if !llvmPackages.stdenv.targetPlatform.isDarwin then
      llvmPackages.stdenv
    else
      prev.overrideCC llvmPackages.stdenv clang;
    release = prev.llvm-backend-release;
    src = prev.llvm-backend-src;
    inherit jemalloc;
    host.clang = clang;
  };

  llvm-backend-matching = import ./llvm-backend-matching.nix {
    inherit (prev) buildMaven;
    src = prev.llvm-backend-matching-src;
  };

  # This code is a bit of a hack to get Nix to accept the binding library being
  # defined in terms of the output of the LLVM backend. Using the backend's
  # lib/python directory directly causes a provenance error when reading the
  # pyproject file.
  kllvm = prev.poetry2nix.mkPoetryApplication {
    python = prev.python39;
    projectDir = ../bindings/python/package;
    postInstall = "
      cp ${llvm-backend}/lib/python/kllvm/* $out/lib/python3.9/site-packages/kllvm/
    ";
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
      llvm-kompile-testing # for constructing test input without the frontend
      llvm-backend # the system under test
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
  };
  devShell = prev.callPackage ./devShell.nix { };
in {
  inherit kllvm llvm-backend llvm-backend-matching integration-tests;
  inherit (prev) clang; # for compatibility
  inherit devShell; # for CI
}
