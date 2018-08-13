
%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }

define %block* @apply_rule_0() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645270 }, %blockheader* %inj
  %2 = inttoptr i64 8589934593 to %block*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %6
}

; Function Attrs: noreturn
declare void @abort() #0

declare noalias i8* @malloc(i64)

attributes #0 = { noreturn }
