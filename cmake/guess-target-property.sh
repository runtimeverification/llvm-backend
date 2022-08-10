#!/bin/bash

echo | \
  "$1" -S -emit-llvm -xc -o- - | \
  grep "target $2" | \
  cut -d' ' -f4 | \
  tr -d \"
