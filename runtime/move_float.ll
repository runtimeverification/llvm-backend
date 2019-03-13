target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 }
%floating = type { %blockheader, i64, { i64, i32, i64, i64 * } } ; 16-bit layout, 48-bit length, exp, mpfr_t

; helper function for float hooks
define %floating* @move_float(%floating* %val) {
  %p1 = getelementptr inbounds %floating, %floating* %val, i64 0, i32 1
  %loadedExp = load i64, i64* %p1
  %p2 = getelementptr inbounds %floating, %floating* %val, i64 0, i32 2
  %loadedMpfr = load { i64, i32, i64, i64 * }, { i64, i32, i64, i64 * }* %p2
  %malloccall = tail call i8* @koreAllocFloating(i64 0)
  %ptr = bitcast i8* %malloccall to %floating*
  %expPtr = getelementptr inbounds %floating, %floating* %ptr, i64 0, i32 1
  store i64 %loadedExp, i64* %expPtr
  %mpfrPtr = getelementptr inbounds %floating, %floating* %ptr, i64 0, i32 2
  store { i64, i32, i64, i64 * } %loadedMpfr, { i64, i32, i64, i64 * }* %mpfrPtr
  ret %floating* %ptr
}

declare noalias i8* @koreAllocFloating(i64)

