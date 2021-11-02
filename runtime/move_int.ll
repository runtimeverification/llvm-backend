target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%mpz = type { i32, i32, i64 * } ; mpz_t

; helper function for int hooks
define %mpz* @move_int(%mpz* %val) {
  %loaded = load %mpz, %mpz* %val
  %malloccall = tail call i8* @koreAllocInteger(i64 0)
  %ptr = bitcast i8* %malloccall to %mpz*
  store %mpz %loaded, %mpz* %ptr
  ret %mpz* %ptr
}

declare noalias i8* @koreAllocInteger(i64)

