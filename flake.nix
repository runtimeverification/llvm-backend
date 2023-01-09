{
  description = "K Kore Language LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs";
    fmt-src.url =
      "github:fmtlib/fmt/9.1.0";
    fmt-src.flake = false;
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

  outputs = { self, nixpkgs, utils, fmt-src, immer-src, rapidjson-src, pybind11-src, mavenix }:
    let
      inherit (nixpkgs) lib;

      # put devShell and any other required packages into local overlay
      # if you have additional overlays, you may add them here
      localOverlay = import ./nix/overlay.nix; # this should expose devShell
      depsOverlay = (final: prev: {
        inherit fmt-src immer-src rapidjson-src pybind11-src;

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
            mkdir -p $out/deps/fmt
            mkdir -p $out/deps/immer
            mkdir -p $out/deps/rapidjson
            mkdir -p $out/deps/pybind11
            cp -rv ${final.fmt-src}/* $out/deps/fmt
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

      pkgsForSystem = system: llvm-version: llvm-backend-build-type:
        import nixpkgs {
          overlays = [
            (_: _: {
              inherit llvm-version;
              inherit llvm-backend-build-type;
            })
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
        listToChecks = checks:
          builtins.listToAttrs (lib.imap0 (i: v: { name = "check_${toString i}"; value = v; }) checks);

        matrix = builtins.listToAttrs (lib.forEach (lib.cartesianProductOfSets {
          llvm-version = [12 13 14];
          build-type = ["Debug" "Release" "RelWithDebInfo" "FastBuild" "GcStats"];
        }) (
          args:
            let pkgs = pkgsForSystem system args.llvm-version args.build-type; in
            {
              name = "llvm-backend-${toString args.llvm-version}-${args.build-type}";
              value = {
                inherit (pkgs) llvm-backend llvm-backend-matching integration-tests devShell;
              };
            }
        ));
      in with matrix; {
        packages = utils.lib.flattenTree {
          inherit (llvm-backend-14-FastBuild) llvm-backend llvm-backend-matching;
          default = llvm-backend-14-FastBuild.llvm-backend;
          llvm-backend-release = llvm-backend-14-Release.llvm-backend;
        };
        checks = listToChecks [
          # Check that the backend compiles on each supported version of LLVM,
          # but don't run the test suite on all 15 configurations.

          # Disable the full set temporarily while the checks run on a hosted
          # runner.
          # llvm-backend-12-Debug.llvm-backend
          llvm-backend-12-FastBuild.llvm-backend

          # llvm-backend-13-Debug.llvm-backend
          llvm-backend-13-FastBuild.llvm-backend

          # llvm-backend-14-Debug.integration-tests
          # llvm-backend-14-Release.integration-tests

          # llvm-backend-14-RelWithDebInfo.integration-tests
          # llvm-backend-14-FastBuild.integration-tests
          # llvm-backend-14-GcStats.integration-tests

          llvm-backend-14-FastBuild.integration-tests
        ];
        devShells.default = llvm-backend-14-FastBuild.devShell;
      }) // {
        # non-system suffixed items should go here
        overlays.default = llvm-backend-overlay;
      };
}
