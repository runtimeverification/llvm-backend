target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 }
%floating = type { i64, { i64, i32, i64, ptr } } ; exp, mpfr_t

; helper function for float hooks
define ptr @move_float(ptr %val) {
  %loaded = load %floating, ptr %val
  %malloccall = tail call ptr @kore_alloc_floating(i64 0)
  store %floating %loaded, ptr %malloccall
  ret ptr %malloccall

}

declare noalias ptr @kore_alloc_floating(i64)

