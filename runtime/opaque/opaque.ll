target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

define ptr addrspace(0) @addrspace_0_to_0(ptr %in) #0 {
  ; %out = addrspacecast ptr %in to ptr addrspace(0)
  ; ret ptr addrspace(0) %out
  ret ptr addrspace(0) %in
}

attributes #0 = { alwaysinline }
