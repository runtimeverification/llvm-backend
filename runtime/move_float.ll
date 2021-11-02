target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 }
%floating = type { i64, { i64, i32, i64, i64 * } } ; exp, mpfr_t

; helper function for float hooks
define %floating* @move_float(%floating* %val) {
  %loaded = load %floating, %floating* %val
  %malloccall = tail call i8* @koreAllocFloating(i64 0)
  %ptr = bitcast i8* %malloccall to %floating*
  store %floating %loaded, %floating* %ptr
  ret %floating* %ptr

}

declare noalias i8* @koreAllocFloating(i64)

