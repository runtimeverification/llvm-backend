let
  sources = import ./nix/sources.nix;
  pinned = import sources."nixpkgs" {
    config = { };
    overlays = [ ];
  };

in { pkgs ? pinned }:

let default = import ./. { inherit pkgs; };
in import ./nix/devShell.nix {
  inherit (pkgs) mkShell;
  inherit (default) llvm-backend llvm-backend-matching mavenix-cli;
}
