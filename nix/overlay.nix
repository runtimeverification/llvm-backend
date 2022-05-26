final: prev:
let
  cleanedSrc = prev.stdenv.mkDerivation {
    name = "llvm-source";
    src = prev.lib.cleanSource (prev.nix-gitignore.gitignoreSourcePure [] ../.);
    dontBuild = true;
    installPhase = ''
      mkdir $out
      cp -rv $src/* $out
      chmod -R u+w $out
      mkdir -p $out/deps/immer
      mkdir -p $out/deps/rapidjson
      cp -rv ${final.immer-src}/* $out/deps/immer
      cp -rv ${final.rapidjson-src}/* $out/deps/rapidjson
    '';
  };

  llvmPackages = prev.llvmPackages_12.override {
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
    host.clang = clang;
    src = cleanedSrc;
  };

  llvm-backend-matching = import ./llvm-backend-matching.nix {
    inherit (prev) mavenix;
    src = cleanedSrc;
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

  devShell = final.callPackage ./devShell.nix { inherit (prev) mavenix; };
in {
    inherit llvm-backend llvm-backend-matching llvm-kompile-testing;
    inherit clang; # for compatibility
    inherit devShell; # for CI
}
