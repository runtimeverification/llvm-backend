#!/bin/bash

echo | \
  "$1" -S -emit-llvm -xc -o- - | \
  grep datalayout | \
  cut -d' ' -f4 | \
  tr -d \"
