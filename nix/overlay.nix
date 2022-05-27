final: prev:
let
  llvmPackages = prev.llvmPackages_10.override {
    bootBintoolsNoLibc = null;
    bootBintools = null;
  };

  # The backend requires clang/lld/libstdc++ at runtime.
  # The closest configuration in Nixpkgs is clang/lld without any C++ standard
  # library. We override that configuration to inherit libstdc++ from stdenv.
  clang =
    let
      override = attrs: {
        extraBuildCommands = ''
          ${attrs.extraBuildCommands}
          sed -i $out/nix-support/cc-cflags -e '/^-nostdlib/ d'
        '';
      };
    in
      llvmPackages.clangNoLibcxx.override override;

  llvm-backend = prev.callPackage ./llvm-backend.nix {
    inherit llvmPackages;
    inherit (prev) release;
    src = prev.llvm-backend-src;
    host.clang = clang;
  };

  llvm-backend-matching = import ./llvm-backend-matching.nix {
    inherit (prev) buildMaven;
    src = prev.llvm-backend-matching-src;
  };

  llvm-kompile-testing =
    let inherit (prev.pkgs) runCommandNoCC jre;
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

  llvm-backend-test = prev.stdenv.mkDerivation {
    name = "llvm-backend-test";
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

  devShell = prev.callPackage ./devShell.nix { inherit (prev) mavenix-cli; };
in {
    inherit llvm-backend llvm-backend-matching llvm-backend-test;
    inherit clang; # for compatibility
    inherit devShell; # for CI
}
