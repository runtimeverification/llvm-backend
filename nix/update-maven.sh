#!/usr/bin/env nix-shell
#!nix-shell ../shell.nix -i bash

# Run this script (from the top level of the repository) when the maven
# project's dependencies change.

mvnix-update -E '(import ./default.nix {}).llvm-backend-matching'
