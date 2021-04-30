let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" { config = {}; overlays = []; };
in

{ pkgs ? pinned

# Override `src` when this project is imported as a Git submodule:
#
# > ttuegel.cleanGitSubtree {
# >   name = "llvm-backend";
# >   src = ./parent/repo;
# >   subDir = "path/to/submodule";
# > };
#
# Use `cleanGitSubtree` whenever possible to preserve the same source code
# layout as the kframework/llvm-backend repository (to enable cache re-use).
#
, src ? null

# Build an optimized release package.
# Currently requires dependents to use LTO. Use sparingly.
, release ? false
}:

let
  ttuegel =
    let
      src = builtins.fetchGit {
        url = "https://github.com/ttuegel/nix-lib";
        rev = "66bb0ab890ff4d828a2dcfc7d5968465d0c7084f";
        ref = "main";
      };
    in import src { inherit pkgs; };
in

let _src = src; in
let src = ttuegel.orElse _src (ttuegel.cleanGitSubtree { name = "llvm-backend"; src = ./.; }); in

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
    inherit llvmPackages src;
    inherit (ttuegel) cleanSourceWith;
    inherit release;
    host.clang = clang;
  };

  mavenix = import sources."mavenix" { inherit pkgs; };

  llvm-backend-matching = import ./nix/llvm-backend-matching.nix {
    inherit mavenix src;
    inherit (ttuegel) cleanSourceWith;
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
    inherit llvm-backend llvm-backend-matching llvm-kompile-testing;
    inherit clang; # for compatibility
    inherit mavenix; # for CI
  };

in self
