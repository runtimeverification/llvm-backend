let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) callPackage nix-gitignore;

  llvmPackages = pkgs.llvmPackages_10;

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
      llvmPackages.lldClangNoLibcxx.override override;

  llvm-backend = callPackage ./nix/llvm-backend.nix {
    inherit llvmPackages;
  };

  mavenix = import sources."mavenix" { inherit pkgs; };

  llvm-backend-matching = import ./nix/llvm-backend-matching.nix {
    inherit mavenix;
  };

  llvm-kompile-testing =
    let inherit (pkgs) runCommandNoCC jre;
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

  self = {
    inherit clang llvm-backend llvm-backend-matching llvm-kompile-testing;
    inherit mavenix;
  };

in self

