{
  description = "K Kore Language LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    rv-utils.url = "github:runtimeverification/rv-nix-tools";
    nixpkgs.follows = "rv-utils/nixpkgs";

    fmt-src.url =
      "github:fmtlib/fmt/9.1.0";
    fmt-src.flake = false;
    immer-src.url =
      "github:runtimeverification/immer/4b0914f0b2acb33befe0ba4cd3a7954f2687e9bb";
    immer-src.flake = false;
    rapidjson-src.url =
      "github:Tencent/rapidjson/f54b0e47a08782a6131cc3d60f94d038fa6e0a51";
    rapidjson-src.flake = false;
    pybind11-src.url =
      "github:pybind/pybind11/0ba639d6177659c5dc2955ac06ad7b5b0d22e05c";
    pybind11-src.flake = false;
    # needed by nix/flake-compat-k-unwrapped.nix
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, utils, fmt-src, immer-src, rapidjson-src, pybind11-src, ... }:
    let
      inherit (nixpkgs) lib;

      # put required packages into local overlay
      # if you have additional overlays, you may add them here
      localOverlay = import ./nix/overlay.nix;
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

      maven-overlay = (final: prev: {
        maven = prev.callPackage ./nix/maven.nix { };
      });

      llvm-backend-overlay =
        nixpkgs.lib.composeManyExtensions [ depsOverlay localOverlay ];

      pkgsForSystem = system: llvm-version: llvm-backend-build-type:
        import nixpkgs {
          overlays = [
            (final: prev: {
              inherit llvm-version;
              inherit llvm-backend-build-type;
              maven = prev.maven // { inherit (prev) jdk; };
            })
            maven-overlay
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
          llvm-version = [15 16 17 18];
          build-type = ["Debug" "Release" "RelWithDebInfo" "FastBuild" "GcStats"];
        }) (
          args:
            let pkgs = pkgsForSystem system args.llvm-version args.build-type; in
            {
              name = "llvm-backend-${toString args.llvm-version}-${args.build-type}";
              value = {
                inherit (pkgs) llvm-backend llvm-backend-matching llvm-kompile-testing integration-tests integration-test-shell;
              };
            }
        ));
      in with matrix; {
        packages = utils.lib.flattenTree {
          inherit (llvm-backend-18-FastBuild) llvm-backend llvm-backend-matching llvm-kompile-testing;
          default = llvm-backend-18-FastBuild.llvm-backend;
          llvm-backend-release = llvm-backend-18-Release.llvm-backend;

          integration-test-shell-15 = llvm-backend-15-Debug.integration-test-shell;
          integration-test-shell-16 = llvm-backend-16-Debug.integration-test-shell;
          integration-test-shell-17 = llvm-backend-17-Debug.integration-test-shell;
          integration-test-shell-18 = llvm-backend-18-Debug.integration-test-shell;
        };

        checks = listToChecks [
          llvm-backend-18-Debug.llvm-backend
          llvm-backend-18-Release.llvm-backend
          llvm-backend-18-RelWithDebInfo.llvm-backend
          llvm-backend-18-GcStats.llvm-backend

          llvm-backend-15-FastBuild.integration-tests
          llvm-backend-16-FastBuild.integration-tests
          llvm-backend-17-FastBuild.integration-tests
          llvm-backend-18-FastBuild.integration-tests
        ];
      }) // {
        # non-system suffixed items should go here
        overlays.default = llvm-backend-overlay;
        overlays.maven = maven-overlay;
      };
}
