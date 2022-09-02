{
  description = "K Kore Language LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    # locked due to https://github.com/NixOS/nixpkgs/pull/172397/files
    nixpkgs.url =
      "github:nixos/nixpkgs/a923e194a3f21ed8a31367c96530a06756ed993e";
    immer-src.url =
      "github:runtimeverification/immer/198c2ae260d49ef1800a2fe4433e07d7dec20059";
    immer-src.flake = false;
    rapidjson-src.url =
      "github:Tencent/rapidjson/f54b0e47a08782a6131cc3d60f94d038fa6e0a51";
    rapidjson-src.flake = false;
    pybind11-src.url =
      "github:pybind/pybind11/0ba639d6177659c5dc2955ac06ad7b5b0d22e05c";
    pybind11-src.flake = false;
    mavenix.url = "github:nix-community/mavenix";
  };

  outputs = { self, nixpkgs, utils, immer-src, rapidjson-src, pybind11-src, mavenix }:
    let
      # put devShell and any other required packages into local overlay
      # if you have additional overlays, you may add them here
      localOverlay = import ./nix/overlay.nix; # this should expose devShell
      depsOverlay = (final: prev: {
        inherit immer-src rapidjson-src pybind11-src;

        llvm-backend-src = prev.stdenv.mkDerivation {
          name = "llvm-backend-src";
          src = prev.lib.cleanSource (prev.nix-gitignore.gitignoreSourcePure [
            "/nix"
            "*.nix"
            "*.nix.sh"
            "/.github"
            "flake.lock"
            ./.gitignore
          ] ./.);
          dontBuild = true;
          installPhase = ''
            mkdir $out
            cp -rv $src/* $out
            chmod -R u+w $out
            mkdir -p $out/deps/immer
            mkdir -p $out/deps/rapidjson
            mkdir -p $out/deps/pybind11
            cp -rv ${final.immer-src}/* $out/deps/immer
            cp -rv ${final.rapidjson-src}/* $out/deps/rapidjson
            cp -rv ${final.pybind11-src}/* $out/deps/pybind11
          '';
        };

        llvm-backend-matching-src = prev.lib.cleanSource
          (prev.nix-gitignore.gitignoreSourcePure [
            "/nix"
            "*.nix"
            "*.nix.sh"
            "/.github"
            "flake.lock"
            ./.gitignore
          ] ./matching);
      });

      llvm-backend-overlay =
        nixpkgs.lib.composeManyExtensions [ depsOverlay localOverlay ];

      pkgsForSystem = system: llvm-backend-release:
        import nixpkgs {
          overlays = [
            (_: _: { inherit llvm-backend-release; })
            mavenix.overlay
            llvm-backend-overlay
          ];
          inherit system;
        };
    in utils.lib.eachSystem [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ] (system:
      let
        packagesRelease = pkgsForSystem system true;
        packages = pkgsForSystem system false;
      in {
        packages = utils.lib.flattenTree {
          inherit (packages) llvm-backend llvm-backend-matching;
          llvm-backend-release = packagesRelease.llvm-backend;
          default = packages.llvm-backend;
        };
        checks = { inherit (packages) integration-tests; };
        devShells.default = packages.devShell;
      }) // {
        # non-system suffixed items should go here
        overlays.default = llvm-backend-overlay;
      };
}
