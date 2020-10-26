let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned }:

let
  inherit (pkgs) stdenv mkShell wrapCCWith wrapBintoolsWith;
  llvmPackages = pkgs.llvmPackages_10;
  clang =
    let
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

  inherit (pkgs) ncurses gmp mpfr libffi jemalloc;

  default = import ./. { inherit pkgs; };
  inherit (default) llvm-backend llvm-backend-matching;

in mkShell {
  inputsFrom = [
    llvm-backend-matching
  ];

  buildInputs = [
    llvm-backend
    clang
    ncurses gmp mpfr libffi jemalloc
  ];
}
