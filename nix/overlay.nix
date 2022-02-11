final: prev:
let
  ttuegel =
    let
      src = builtins.fetchGit {
        url = "https://github.com/ttuegel/nix-lib";
        rev = "66bb0ab890ff4d828a2dcfc7d5968465d0c7084f";
        ref = "main";
      };
    in import src { inherit (prev) pkgs; };

  cleanedSrc = ttuegel.orElse prev.src (ttuegel.cleanGitSubtree { name = "llvm-backend"; src = ../.; });

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
    inherit (ttuegel) cleanSourceWith;
    inherit (prev) release;
    host.clang = clang;
    src = cleanedSrc;
  };

  llvm-backend-matching = import ./llvm-backend-matching.nix {
    inherit (prev) mavenix;
    src = cleanedSrc;
    inherit (ttuegel) cleanSourceWith;
  };

  llvm-kompile-testing =
    let inherit (prev.pkgs) runCommandNoCC jre;
        java = "${jre}/bin/java";
        inherit (llvm-backend-matching) jar;
    in runCommandNoCC "llvm-kompile-testing" { } ''
      mkdir -p "$out/bin"
      cp ${llvm-backend.src}/bin/llvm-kompile-testing "$out/bin"
      sed -i "$out/bin/llvm-kompile-testing" \
          -e '/@PROJECT_SOURCE_DIR@/ c ${java} -jar ${jar} $definition qbaL $dt_dir 9/10'
      chmod +x "$out/bin/llvm-kompile-testing"
      patchShebangs "$out/bin/llvm-kompile-testing"
    '';

  devShell = final.callPackage ./devShell.nix { };
in {
    inherit llvm-backend llvm-backend-matching llvm-kompile-testing;
    inherit clang; # for compatibility
    inherit devShell; # for CI
}
