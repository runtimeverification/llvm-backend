target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%mpz = type { i32, i32, ptr } ; mpz_t

; helper function for int hooks
define ptr @move_int(ptr %val) {
  %loaded = load %mpz, ptr %val
  %malloccall = tail call ptr @kore_alloc_integer(i64 0)
  store %mpz %loaded, ptr %malloccall
  ret ptr %malloccall
}

declare noalias ptr @kore_alloc_integer(i64)

