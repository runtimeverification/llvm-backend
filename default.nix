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

  clang =
    let
      inherit (pkgs) stdenv mkShell wrapCCWith wrapBintoolsWith;
      llvmPackages = pkgs.llvmPackages_10;
      inherit (llvmPackages) tools libraries;
      mkExtraBuildCommands = cc: ''
        rsrc="$out/resource-root"
        mkdir "$rsrc"
        ln -s "${cc}/lib/clang/10.0.0/include" "$rsrc"
        ln -s "${libraries.compiler-rt.out}/lib" "$rsrc/lib"
        echo "-resource-dir=$rsrc" >> $out/nix-support/cc-cflags
      '' + stdenv.lib.optionalString (stdenv.targetPlatform.isLinux && !(stdenv.targetPlatform.useLLVM or false)) ''
        echo "--gcc-toolchain=${tools.clang-unwrapped.gcc}" >> $out/nix-support/cc-cflags
      '';
    in
    wrapCCWith rec {
      cc = tools.clang-unwrapped;
      libcxx = null;
      bintools = wrapBintoolsWith {
        inherit (tools) bintools;
      };
      extraPackages = [
        libraries.compiler-rt
      ];
      extraBuildCommands = ''
        echo "-rtlib=compiler-rt -Wno-unused-command-line-argument" >> $out/nix-support/cc-cflags
        echo "-B${libraries.compiler-rt}/lib" >> $out/nix-support/cc-cflags
      '' + mkExtraBuildCommands cc;
    };

  self = {
    inherit clang llvm-backend llvm-backend-matching;
    inherit mavenix;
  };

in self

