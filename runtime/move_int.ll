target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64 * } ; mpz_t

; helper function for int hooks
define %mpz* @move_int(%mpz* %val) {
  %loaded = load %mpz, %mpz* %val
  %malloccall = tail call i8* @koreAllocOld(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %ptr = bitcast i8* %malloccall to %mpz*
  store %mpz %loaded, %mpz* %ptr
  ret %mpz* %ptr
}

declare noalias i8* @koreAllocOld(i64)

