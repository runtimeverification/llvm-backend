target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define ptr addrspace(0) @addrspace_0_to_0(ptr %in) #0 {
  %out = addrspacecast ptr %in to ptr addrspace(0)
  ret ptr addrspace(0) %out
}

attributes #0 = { alwaysinline }
