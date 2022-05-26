{
  description = "K Kore Language LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    immer-src.url = "github:runtimeverification/immer/198c2ae260d49ef1800a2fe4433e07d7dec20059";
    immer-src.flake = false;
    rapidjson-src.url = "github:Tencent/rapidjson/f54b0e47a08782a6131cc3d60f94d038fa6e0a51";
    rapidjson-src.flake = false;
  };

  outputs = { self, nixpkgs, utils, immer-src, rapidjson-src }:
    let
      # put devShell and any other required packages into local overlay
      # if you have additional overlays, you may add them here
      localOverlay = import ./nix/overlay.nix; # this should expose devShell
      depsOverlay = 
        (final: prev: {
          inherit immer-src rapidjson-src;
        });

      overlay = nixpkgs.lib.composeManyExtensions [ depsOverlay localOverlay ];

      pkgsForSystem = system: import nixpkgs {
        overlays = [ (_:_: {release = false;}) overlay ];
        inherit system;
      };
    # https://github.com/numtide/flake-utils#usage for more examples
    in utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ] (system: rec {
      legacyPackages = pkgsForSystem system;
      packages = utils.lib.flattenTree {
        inherit (legacyPackages) llvm-backend devShell;
      };
      # TODO
      # defaultPackage = packages.myPkg;
      # apps.<mypkg> = utils.lib.mkApp { drv = packages.myPkg; };  # use as `nix run <mypkg>`
      # hydraJobs = { inherit (legacyPackages) myPkg; };
      # checks = { inherit (legacyPackages) myPkg; };              # items to be ran as part of `nix flake check`
  }) // {
    # non-system suffixed items should go here
    inherit overlay;
    overlays = []; # list or attr set of overlays
    nixosModule = { config }: { options = {}; config = {};}; # export single module
    nixosModules = {}; # attr set or list
    nixosConfigurations.hostname = { config, pkgs }: {};
  };
}
