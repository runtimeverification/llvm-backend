let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) callPackage nix-gitignore;

  llvm-backend = callPackage ./nix/llvm-backend.nix {
    inherit (nix-gitignore) gitignoreSource;
    llvmPackages = pkgs.llvmPackages_10;
  };

  mavenix = import sources."mavenix" { inherit pkgs; };

  llvm-backend-matching = import ./nix/llvm-backend-matching.nix {
    inherit mavenix;
  };

  # The backend requires clang/lld/libstdc++ at runtime.
  # The closest configuration in Nixpkgs is clang/lld without any C++ standard
  # library. We override that configuration to inherit libstdc++ from stdenv.
  clang =
    let
      llvmPackages = pkgs.llvmPackages_10;
      override = attrs: {
        extraBuildCommands = ''
          ${attrs.extraBuildCommands}
          sed -i $out/nix-support/cc-cflags -e '/^-nostdlib/ d'
        '';
      };
    in
      llvmPackages.lldClangNoLibcxx.override override;

  self = {
    inherit clang llvm-backend llvm-backend-matching;
    inherit mavenix;
  };

in self

