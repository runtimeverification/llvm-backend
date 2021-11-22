{
  description = "LLVM Backend";

  inputs = {
    utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, utils }:
    let
      # put devShell and any other required packages into local overlay
      localOverlay = import ./nix/overlay.nix;

      pkgsForSystem = system: import nixpkgs {
        # if you have additional overlays, you may add them here
        overlays = [
          localOverlay # this should expose devShell
        ];
        inherit system;
      };
    # https://github.com/numtide/flake-utils#usage for more examples
    in utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" ] (system: rec {
      legacyPackages = pkgsForSystem system;
      packages = utils.lib.flattenTree {
        inherit (legacyPackages) devShell;
      };
      # TODO
      # defaultPackage = packages.myPkg;
      # apps.<mypkg> = utils.lib.mkApp { drv = packages.myPkg; };  # use as `nix run <mypkg>`
      # hydraJobs = { inherit (legacyPackages) myPkg; };
      # checks = { inherit (legacyPackages) myPkg; };              # items to be ran as part of `nix flake check`
  }) // {
    # non-system suffixed items should go here
    overlay = localOverlay;
    overlays = []; # list or attr set of overlays
    nixosModule = { config }: { options = {}; config = {};}; # export single module
    nixosModules = {}; # attr set or list
    nixosConfigurations.hostname = { config, pkgs }: {};
  };
}
