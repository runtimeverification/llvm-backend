{
  description = "K Kore Language LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    immer-src.url =
      "github:runtimeverification/immer/198c2ae260d49ef1800a2fe4433e07d7dec20059";
    immer-src.flake = false;
    rapidjson-src.url =
      "github:Tencent/rapidjson/f54b0e47a08782a6131cc3d60f94d038fa6e0a51";
    rapidjson-src.flake = false;
    mavenix.url = "github:nix-community/mavenix";
  };

  outputs = { self, nixpkgs, utils, immer-src, rapidjson-src, mavenix }:
    let
      # put devShell and any other required packages into local overlay
      # if you have additional overlays, you may add them here
      localOverlay = import ./nix/overlay.nix; # this should expose devShell
      depsOverlay = (final: prev: {
        inherit immer-src rapidjson-src;

        llvm-backend-src = prev.stdenv.mkDerivation {
          name = "llvm-backend-src";
          src = prev.lib.cleanSource
            (prev.nix-gitignore.gitignoreSourcePure [ ] ./.);
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

        llvm-backend-matching-src = prev.lib.cleanSource
          (prev.nix-gitignore.gitignoreSourcePure [ ] ./matching);
      });

      overlay = nixpkgs.lib.composeManyExtensions [ depsOverlay localOverlay ];

      pkgsForSystem = system: release:
        import nixpkgs {
          overlays = [ (_: _: { inherit release; }) mavenix.overlay overlay ];
          inherit system;
        };
    in utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" ]
    (system:
      let legacyPackagesRelease = pkgsForSystem system true;
      in rec {
        legacyPackages = pkgsForSystem system false;

        packages = utils.lib.flattenTree {
          inherit (legacyPackages) llvm-backend llvm-backend-matching;
          llvm-backend-release = legacyPackagesRelease.llvm-backend;
        };
        checks = { inherit (legacyPackages) llvm-backend-test; };
        inherit (legacyPackages) devShell;
        defaultPackage = packages.llvm-backend;
        # TODO
        # apps.<mypkg> = utils.lib.mkApp { drv = packages.myPkg; };  # use as `nix run <mypkg>`
        # hydraJobs = { inherit (legacyPackages) myPkg; };
        # checks = { inherit (legacyPackages) myPkg; };              # items to be ran as part of `nix flake check`
      }) // {
        # non-system suffixed items should go here
        inherit overlay;
      };
}
