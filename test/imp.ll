target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64* }
%blockheader = type { i64 }
%block = type { %blockheader, [0 x i64*] }
%map = type { i64, i8*, i8* }

@int_-1 = global %mpz { i32 1, i32 -1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_-1_limbs, i32 0, i32 0) }
@int_-1_limbs = global [1 x i64] [i64 1]
@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer
@int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_1_limbs, i32 0, i32 0) }
@int_1_limbs = global [1 x i64] [i64 1]
@"token_\22_\22" = global { %blockheader, [3 x i8] } { %blockheader { i64 3 }, [3 x i8] c"\22_\22" }
@"token_$PGM" = global { %blockheader, [4 x i8] } { %blockheader { i64 4 }, [4 x i8] c"$PGM" }

define %mpz* @move_int(%mpz %val) {
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %ptr = bitcast i8* %malloccall to %mpz*
  store %mpz %val, %mpz* %ptr
  ret %mpz* %ptr
}

declare noalias i8* @malloc(i64)

define %block* @apply_rule_0(%block* %K2) {
entry:
  ret %block* %K2
}

define %block* @apply_rule_1(%block* %K1, %block* %K2, %block* %K3) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %K1, %block** %1
  %2 = call %block* @"eval_append{SortK{},SortK{}}"(%block* %K2, %block* %K3)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %4
}

declare %block* @"eval_append{SortK{},SortK{}}"(%block*, %block*)

define i1 @apply_rule_2() {
entry:
  ret i1 true
}

define i1 @apply_rule_3(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = call i1 @hook_STRING_lt(%block* %VarS2, %block* %VarS1)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_STRING_lt(%block*, %block*)

define i1 @apply_rule_4() {
entry:
  ret i1 true
}

define i1 @apply_rule_5() {
entry:
  ret i1 true
}

define i1 @apply_rule_6(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_7() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_8() {
entry:
  ret %mpz* @int_-1
}

define %block* @apply_rule_9(%block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarDotVar1, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %6
}

define i1 @apply_rule_10() {
entry:
  ret i1 true
}

define i1 @apply_rule_11() {
entry:
  ret i1 true
}

define i1 @apply_rule_12() {
entry:
  ret i1 true
}

define i1 @apply_rule_13() {
entry:
  ret i1 false
}

define i1 @apply_rule_14() {
entry:
  ret i1 true
}

define i1 @apply_rule_15(i1 %VarK1, i1 %VarK2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %0, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %0, i64 0, i32 2
  store i1 %VarK1, i1* %1
  %2 = bitcast { %blockheader, [0 x i64], i1 }* %0 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj2
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %VarK2, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = call i1 @hook_KEQUAL_eq(%block* %2, %block* %5)
  ret i1 %6
}

declare i1 @hook_KEQUAL_eq(%block*, %block*)

define %block* @apply_rule_16(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612585 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarK1, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define i1 @apply_rule_17() {
entry:
  ret i1 true
}

define i1 @apply_rule_18() {
entry:
  ret i1 true
}

define i1 @apply_rule_19() {
entry:
  ret i1 true
}

define i1 @apply_rule_20() {
entry:
  ret i1 true
}

define i1 @apply_rule_21() {
entry:
  ret i1 false
}

define %block* @apply_rule_22(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355912 }, %blockheader* %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define %mpz* @apply_rule_23() {
entry:
  ret %mpz* @int_0
}

define i1 @apply_rule_24() {
entry:
  ret i1 true
}

define %block* @apply_rule_25(%block* %VarSource) {
entry:
  ret %block* %VarSource
}

define i1 @apply_rule_26() {
entry:
  ret i1 true
}

define i1 @apply_rule_27(i1 %VarB) {
entry:
  %hook_BOOL_not = xor i1 %VarB, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_28() {
entry:
  ret i1 true
}

define i1 @apply_rule_29() {
entry:
  ret i1 true
}

define i1 @apply_rule_30() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_31(%mpz* %VarI, %mpz* %VarLEN, %mpz* %VarIDX) {
entry:
  %0 = call %mpz* @hook_INT_shr(%mpz* %VarI, %mpz* %VarIDX)
  %1 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %VarLEN)
  %2 = call %mpz* @hook_INT_emod(%mpz* %0, %mpz* %1)
  ret %mpz* %2
}

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

define %block* @apply_rule_32(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612601 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarK1, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define %block* @apply_rule_33(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612603 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarK1, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define i1 @apply_rule_34() {
entry:
  ret i1 true
}

define %block* @apply_rule_35(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355911 }, %blockheader* %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define i1 @apply_rule_36() {
entry:
  ret i1 false
}

define %block* @apply_rule_37(%block* %VarX, %block* %VarI, %block* %VarDotVar1, %map %VarDotVar2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %2 = call %block* @"eval_append{SortK{},SortK{}}"(%block* %VarI, %block* %VarDotVar1)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %6, i64 0, i32 0
  store %blockheader { i64 844442110001154 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %VarX, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
  %10 = call %map @hook_MAP_element(%block* %9, %block* %VarI)
  %11 = alloca %map
  store %map %10, %map* %11
  %12 = alloca %map
  store %map %VarDotVar2, %map* %12
  %13 = call %map @hook_MAP_concat(%map* %11, %map* %12)
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %6, i64 0, i32 2
  store %map %13, %map* %14
  %15 = bitcast { %blockheader, [0 x i64], %map }* %6 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

declare %map @hook_MAP_element(%block*, %block*)

declare %map @hook_MAP_concat(%map*, %map*)

define i1 @apply_rule_38() {
entry:
  ret i1 true
}

define i1 @apply_rule_39() {
entry:
  ret i1 true
}

define i1 @apply_rule_40() {
entry:
  ret i1 true
}

define i1 @apply_rule_41(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_42(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_43() {
entry:
  ret i1 true
}

define %block* @apply_rule_44(%mpz* %VarI1, %mpz* %VarI2, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj
  %4 = call i1 @hook_INT_le(%mpz* %VarI1, %mpz* %VarI2)
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %4, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %14
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define %block* @apply_rule_45(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612586 }, %blockheader* %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarK1, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define %block* @apply_rule_46(%block* %VarX, %map %VarRho, %block* %VarXs, %block* %"Var'Unds'22") {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 562958543356040 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612621 }, %blockheader* %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarXs, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %"Var'Unds'22", %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %12, i64 0, i32 0
  store %blockheader { i64 844442110001154 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %13 = alloca %map
  store %map %VarRho, %map* %13
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block* }*
  %inj6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj6
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 2
  store %block* %VarX, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %14 to %block*
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %17 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %mpz* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %17, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj8
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %17, i64 0, i32 2
  store %mpz* @int_0, %mpz** %18
  %19 = bitcast { %blockheader, [0 x i64], %mpz* }* %17 to %block*
  %20 = call %map @hook_MAP_element(%block* %16, %block* %19)
  %21 = alloca %map
  store %map %20, %map* %21
  %22 = call %map @hook_MAP_concat(%map* %13, %map* %21)
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %12, i64 0, i32 2
  store %map %22, %map* %23
  %24 = bitcast { %blockheader, [0 x i64], %map }* %12 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %26
}

define %block* @apply_rule_47(%block* %VarB1) {
entry:
  ret %block* %VarB1
}

define i1 @apply_rule_48(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = call i1 @hook_STRING_lt(%block* %VarS2, %block* %VarS1)
  ret i1 %0
}

define i1 @apply_rule_49() {
entry:
  ret i1 false
}

define %block* @apply_rule_50(%block* %VarHOLE, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355907 }, %blockheader* %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %16
}

define i1 @apply_rule_51() {
entry:
  ret i1 true
}

define i1 @apply_rule_52() {
entry:
  ret i1 false
}

define %block* @apply_rule_53(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356026 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_54(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_tmod(%mpz* %VarI2, %mpz* %VarI1)
  %1 = call i1 @hook_INT_eq(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

declare %mpz* @hook_INT_tmod(%mpz*, %mpz*)

declare i1 @hook_INT_eq(%mpz*, %mpz*)

define i1 @apply_rule_55(i1 %VarK) {
entry:
  ret i1 %VarK
}

define %block* @apply_rule_56(%mpz* %VarI1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj
  %4 = call %mpz* @hook_INT_sub(%mpz* @int_0, %mpz* %VarI1)
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %14
}

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

define i1 @apply_rule_57() {
entry:
  ret i1 false
}

define i1 @apply_rule_58() {
entry:
  ret i1 true
}

define i1 @apply_rule_59() {
entry:
  ret i1 false
}

define %block* @apply_rule_60(%block* %VarSource) {
entry:
  ret %block* %VarSource
}

define i1 @apply_rule_61(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_62(%block* %VarB2) {
entry:
  ret %block* %VarB2
}

define i1 @apply_rule_63() {
entry:
  ret i1 false
}

define i1 @apply_rule_64(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_65() {
entry:
  ret i1 false
}

define i1 @apply_rule_66(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_67() {
entry:
  ret i1 true
}

define i1 @apply_rule_68(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = call i1 @hook_STRING_eq(%block* %VarS1, %block* %VarS2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_STRING_eq(%block*, %block*)

define i1 @apply_rule_69() {
entry:
  ret i1 true
}

define i1 @apply_rule_70() {
entry:
  ret i1 true
}

define %block* @apply_rule_71(%mpz* %VarI) {
entry:
  %0 = call %block* @hook_STRING_int2string(%mpz* %VarI)
  %1 = call %block* @hook_STRING_concat(%block* bitcast ({ %blockheader, [3 x i8] }* @"token_\22_\22" to %block*), %block* %0)
  %2 = call %block* @hook_STRING_string2token(%block* %1)
  ret %block* %2
}

declare %block* @hook_STRING_int2string(%mpz*)

declare %block* @hook_STRING_concat(%block*, %block*)

declare %block* @hook_STRING_string2token(%block*)

define i1 @apply_rule_72() {
entry:
  ret i1 true
}

define i1 @apply_rule_73() {
entry:
  ret i1 true
}

define %block* @apply_rule_74(%mpz* %VarI1, %mpz* %VarI2, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj
  %4 = call %mpz* @hook_INT_add(%mpz* %VarI1, %mpz* %VarI2)
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %14
}

declare %mpz* @hook_INT_add(%mpz*, %mpz*)

define %mpz* @apply_rule_75() {
entry:
  ret %mpz* @int_-1
}

define i1 @apply_rule_76() {
entry:
  ret i1 false
}

define i1 @apply_rule_77() {
entry:
  ret i1 false
}

define i1 @apply_rule_78() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_79(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_abs(%mpz* %VarI2)
  %1 = call %mpz* @hook_INT_tmod(%mpz* %VarI1, %mpz* %0)
  %2 = call %mpz* @hook_INT_abs(%mpz* %VarI2)
  %3 = call %mpz* @hook_INT_add(%mpz* %1, %mpz* %2)
  %4 = call %mpz* @hook_INT_abs(%mpz* %VarI2)
  %5 = call %mpz* @hook_INT_tmod(%mpz* %3, %mpz* %4)
  ret %mpz* %5
}

declare %mpz* @hook_INT_abs(%mpz*)

define %block* @apply_rule_80(%mpz* %VarI1, %mpz* %VarI2, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj
  %4 = call %mpz* @hook_INT_tdiv(%mpz* %VarI1, %mpz* %VarI2)
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %14
}

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

define i1 @apply_rule_81() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_82(%mpz* %VarI) {
entry:
  ret %mpz* %VarI
}

define i1 @apply_rule_83() {
entry:
  ret i1 true
}

define i1 @apply_rule_84() {
entry:
  ret i1 false
}

define %block* @apply_rule_85(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarB, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 4222141830529097 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarB, %block** %5
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543355921 }, %blockheader* %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt"
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612604 }, %blockheader* %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356026 }, %blockheader* %inj8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %VarS, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %10, %block** %11
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall9 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612659 }, %blockheader* %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block"
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %VarB, %block** %13
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %VarS, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 3
  store %block* %19, %block** %20
  %21 = inttoptr i64 68719476737 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 4
  store %block* %21, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %4 to %block*
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %23, %block** %24
  %25 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %25, %block** %26
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %31 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %30, %block** %31
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %33
}

define i1 @apply_rule_86() {
entry:
  ret i1 true
}

define i1 @apply_rule_87() {
entry:
  ret i1 true
}

define %block* @apply_rule_88(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612603 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarK0, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarHOLE, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define i1 @apply_rule_89() {
entry:
  ret i1 true
}

define %block* @apply_rule_90() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 0
  store %blockheader { i64 844442110001154 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %1 = call %map @hook_MAP_unit()
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 2
  store %map %1, %map* %2
  %3 = bitcast { %blockheader, [0 x i64], %map }* %0 to %block*
  ret %block* %3
}

declare %map @hook_MAP_unit()

define i1 @apply_rule_91() {
entry:
  ret i1 true
}

define i1 @apply_rule_92() {
entry:
  ret i1 true
}

define i1 @apply_rule_93() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_94(%mpz* %VarI, %block* %VarS1, %block* %VarS2) {
entry:
  %0 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  %1 = call %mpz* @hook_STRING_rfind(%block* %VarS1, %block* %0, %mpz* %VarI)
  %2 = call %mpz* @hook_STRING_length(%block* %VarS2)
  %3 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_1, %mpz* %2)
  %4 = call %mpz* @hook_STRING_rfindChar(%block* %VarS1, %block* %3, %mpz* %VarI)
  %5 = call %mpz* @hook_INT_max(%mpz* %1, %mpz* %4)
  ret %mpz* %5
}

declare %block* @hook_STRING_substr(%block*, %mpz*, %mpz*)

declare %mpz* @hook_STRING_rfind(%block*, %block*, %mpz*)

declare %mpz* @hook_STRING_length(%block*)

declare %mpz* @hook_STRING_rfindChar(%block*, %block*, %mpz*)

declare %mpz* @hook_INT_max(%mpz*, %mpz*)

define i1 @apply_rule_95(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_96() {
entry:
  ret i1 false
}

define i1 @apply_rule_97() {
entry:
  ret i1 false
}

define %block* @apply_rule_98(%block* %VarDotVar0, %block* %VarDotVar1, i1 %VarT) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj
  %hook_BOOL_not = xor i1 %VarT, true
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %hook_BOOL_not, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_99() {
entry:
  ret i1 true
}

define i1 @apply_rule_100() {
entry:
  ret i1 true
}

define i1 @apply_rule_101(%block* %VarK1, %block* %VarK2) {
entry:
  %0 = call i1 @hook_KEQUAL_eq(%block* %VarK1, %block* %VarK2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_102() {
entry:
  ret i1 true
}

define i1 @apply_rule_103() {
entry:
  ret i1 true
}

define i1 @apply_rule_104() {
entry:
  ret i1 true
}

define i1 @apply_rule_105() {
entry:
  ret i1 true
}

define i1 @apply_rule_106() {
entry:
  ret i1 true
}

define i1 @apply_rule_107(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = call i1 @hook_STRING_lt(%block* %VarS1, %block* %VarS2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_108() {
entry:
  ret i1 true
}

define i1 @apply_rule_109() {
entry:
  ret i1 true
}

define i1 @apply_rule_110() {
entry:
  ret i1 true
}

define i1 @apply_rule_111(%block* %VarS1, %block* %VarS2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 562958543356041 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %VarS1, %block** %1
  %2 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356041 }, %blockheader* %inj2
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS2, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = call i1 @hook_KEQUAL_eq(%block* %2, %block* %5)
  ret i1 %6
}

define i1 @apply_rule_112() {
entry:
  ret i1 false
}

define %block* @apply_rule_113(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612601 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarK0, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarHOLE, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define i1 @apply_rule_114(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_115() {
entry:
  ret i1 false
}

define %block* @apply_rule_116(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355914 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define i1 @apply_rule_117() {
entry:
  ret i1 false
}

define i1 @apply_rule_118(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_119() {
entry:
  ret i1 true
}

define %block* @apply_rule_120(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarB) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarB, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define %block* @apply_rule_121(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_122() {
entry:
  ret i1 false
}

define i1 @apply_rule_123() {
entry:
  ret i1 true
}

define i1 @apply_rule_124() {
entry:
  ret i1 true
}

define i1 @apply_rule_125() {
entry:
  ret i1 false
}

define %block* @apply_rule_126(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612585 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarK0, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarHOLE, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define %mpz* @apply_rule_127(%mpz* %VarI, %block* %VarS1, %block* %VarS2) {
entry:
  %0 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  %1 = call %mpz* @hook_STRING_find(%block* %VarS1, %block* %0, %mpz* %VarI)
  %2 = call i1 @hook_INT_eq(%mpz* %1, %mpz* @int_-1)
  %3 = call %mpz* @hook_STRING_length(%block* %VarS2)
  %4 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_1, %mpz* %3)
  %5 = call %mpz* @hook_STRING_findChar(%block* %VarS1, %block* %4, %mpz* %VarI)
  %6 = call %mpz* @hook_STRING_length(%block* %VarS2)
  %7 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_1, %mpz* %6)
  %8 = call %mpz* @hook_STRING_findChar(%block* %VarS1, %block* %7, %mpz* %VarI)
  %9 = call i1 @hook_INT_eq(%mpz* %8, %mpz* @int_-1)
  %10 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  %11 = call %mpz* @hook_STRING_find(%block* %VarS1, %block* %10, %mpz* %VarI)
  %12 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  %13 = call %mpz* @hook_STRING_find(%block* %VarS1, %block* %12, %mpz* %VarI)
  %14 = call %mpz* @hook_STRING_length(%block* %VarS2)
  %15 = call %block* @hook_STRING_substr(%block* %VarS2, %mpz* @int_1, %mpz* %14)
  %16 = call %mpz* @hook_STRING_findChar(%block* %VarS1, %block* %15, %mpz* %VarI)
  %17 = call %mpz* @hook_INT_min(%mpz* %13, %mpz* %16)
  %18 = call %mpz* @hook_KEQUAL_ite(i1 %9, %mpz* %11, %mpz* %17)
  %19 = call %mpz* @hook_KEQUAL_ite(i1 %2, %mpz* %5, %mpz* %18)
  ret %mpz* %19
}

declare %mpz* @hook_STRING_find(%block*, %block*, %mpz*)

declare %mpz* @hook_STRING_findChar(%block*, %block*, %mpz*)

declare %mpz* @hook_INT_min(%mpz*, %mpz*)

declare %mpz* @hook_KEQUAL_ite(i1, %mpz*, %mpz*)

define %mpz* @apply_rule_128(%block* %VarToCount, %block* %VarSource) {
entry:
  %0 = call %mpz* @hook_STRING_find(%block* %VarSource, %block* %VarToCount, %mpz* @int_0)
  %1 = call %mpz* @hook_STRING_length(%block* %VarToCount)
  %2 = call %mpz* @hook_INT_add(%mpz* %0, %mpz* %1)
  %3 = call %mpz* @hook_STRING_length(%block* %VarSource)
  %4 = call %block* @hook_STRING_substr(%block* %VarSource, %mpz* %2, %mpz* %3)
  %5 = call %mpz* @hook_STRING_countAllOccurrences(%block* %4, %block* %VarToCount)
  %6 = call %mpz* @hook_INT_add(%mpz* @int_1, %mpz* %5)
  ret %mpz* %6
}

declare %mpz* @hook_STRING_countAllOccurrences(%block*, %block*)

define i1 @apply_rule_129() {
entry:
  ret i1 true
}

define %block* @apply_rule_130(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612596 }, %blockheader* %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarK0, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarHOLE, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %17
}

define i1 @apply_rule_131() {
entry:
  ret i1 false
}

define i1 @apply_rule_132() {
entry:
  ret i1 false
}

define i1 @apply_rule_133(i1 %VarK) {
entry:
  ret i1 %VarK
}

define %block* @apply_rule_134(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355913 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define %mpz* @apply_rule_135(%mpz* %VarI2) {
entry:
  ret %mpz* %VarI2
}

define i1 @apply_rule_136(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define %block* @apply_rule_137(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355910 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define i1 @apply_rule_138() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_139(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_emod(%mpz* %VarI1, %mpz* %VarI2)
  %1 = call %mpz* @hook_INT_sub(%mpz* %VarI1, %mpz* %0)
  %2 = call %mpz* @hook_INT_tdiv(%mpz* %1, %mpz* %VarI2)
  ret %mpz* %2
}

define i1 @apply_rule_140() {
entry:
  ret i1 true
}

define %block* @apply_rule_141(%block* %VarHOLE, %block* %VarK1, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355916 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define %block* @apply_rule_142(%map %VarInit) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %1 = alloca %map
  store %map %VarInit, %map* %1
  %2 = call %block* @"eval_LblinitKCell{SortMap{}}"(%map* %1)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = call %block* @"eval_LblinitStateCell{}"()
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %6
}

declare %block* @"eval_LblinitKCell{SortMap{}}"(%map*)

declare %block* @"eval_LblinitStateCell{}"()

define %mpz* @apply_rule_143(%mpz* %VarI, %mpz* %VarLEN, %mpz* %VarIDX) {
entry:
  %0 = call %mpz* @hook_INT_bitRange(%mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN)
  %1 = call %mpz* @hook_INT_sub(%mpz* %VarLEN, %mpz* @int_1)
  %2 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %1)
  %3 = call %mpz* @hook_INT_add(%mpz* %0, %mpz* %2)
  %4 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %VarLEN)
  %5 = call %mpz* @hook_INT_emod(%mpz* %3, %mpz* %4)
  %6 = call %mpz* @hook_INT_sub(%mpz* %VarLEN, %mpz* @int_1)
  %7 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %6)
  %8 = call %mpz* @hook_INT_sub(%mpz* %5, %mpz* %7)
  ret %mpz* %8
}

declare %mpz* @hook_INT_bitRange(%mpz*, %mpz*, %mpz*)

define %block* @apply_rule_144(%mpz* %VarCount, %block* %VarToReplace, %block* %VarReplacement, %block* %VarSource) {
entry:
  %0 = call %mpz* @hook_STRING_find(%block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  %1 = call %block* @hook_STRING_substr(%block* %VarSource, %mpz* @int_0, %mpz* %0)
  %2 = call %block* @hook_STRING_concat(%block* %1, %block* %VarReplacement)
  %3 = call %mpz* @hook_STRING_find(%block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  %4 = call %mpz* @hook_STRING_length(%block* %VarToReplace)
  %5 = call %mpz* @hook_INT_add(%mpz* %3, %mpz* %4)
  %6 = call %mpz* @hook_STRING_length(%block* %VarSource)
  %7 = call %block* @hook_STRING_substr(%block* %VarSource, %mpz* %5, %mpz* %6)
  %8 = call %mpz* @hook_INT_sub(%mpz* %VarCount, %mpz* @int_1)
  %9 = call %block* @hook_STRING_replace(%block* %7, %block* %VarToReplace, %block* %VarReplacement, %mpz* %8)
  %10 = call %block* @hook_STRING_concat(%block* %2, %block* %9)
  ret %block* %10
}

declare %block* @hook_STRING_replace(%block*, %block*, %block*, %mpz*)

define i1 @apply_rule_145() {
entry:
  ret i1 true
}

define i1 @apply_rule_146() {
entry:
  ret i1 true
}

define i1 @apply_rule_147() {
entry:
  ret i1 true
}

define i1 @apply_rule_148() {
entry:
  ret i1 true
}

define %block* @apply_rule_149(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355915 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define i1 @apply_rule_150(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_eq(%mpz* %VarI1, %mpz* %VarI2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define %block* @apply_rule_151(%block* %VarToReplace, %block* %VarReplacement, %block* %VarSource) {
entry:
  %0 = call %mpz* @hook_STRING_find(%block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  %1 = call %block* @hook_STRING_substr(%block* %VarSource, %mpz* @int_0, %mpz* %0)
  %2 = call %block* @hook_STRING_concat(%block* %1, %block* %VarReplacement)
  %3 = call %mpz* @hook_STRING_find(%block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  %4 = call %mpz* @hook_STRING_length(%block* %VarToReplace)
  %5 = call %mpz* @hook_INT_add(%mpz* %3, %mpz* %4)
  %6 = call %mpz* @hook_STRING_length(%block* %VarSource)
  %7 = call %block* @hook_STRING_substr(%block* %VarSource, %mpz* %5, %mpz* %6)
  %8 = call %block* @hook_STRING_concat(%block* %2, %block* %7)
  ret %block* %8
}

define i1 @apply_rule_152() {
entry:
  ret i1 true
}

define i1 @apply_rule_153() {
entry:
  ret i1 true
}

define i1 @apply_rule_154() {
entry:
  ret i1 true
}

define i1 @apply_rule_155() {
entry:
  ret i1 false
}

define %block* @apply_rule_156(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356026 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define %block* @apply_rule_157(%map %VarInit) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %1 = alloca %map
  store %map %VarInit, %map* %1
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 562958543356034 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @"token_$PGM" to %block*), %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %5 = call %block* @hook_MAP_lookup(%map* %1, %block* %4)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %7
}

declare %block* @hook_MAP_lookup(%map*, %block*)

define %block* @apply_rule_158(%block* %VarX, %mpz* %VarI, %block* %VarDotVar1, %map %VarDotVar2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarDotVar1, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %5, i64 0, i32 0
  store %blockheader { i64 844442110001154 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarX, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %mpz* }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %9, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj5
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %9, i64 0, i32 2
  store %mpz* %VarI, %mpz** %10
  %11 = bitcast { %blockheader, [0 x i64], %mpz* }* %9 to %block*
  %12 = call %map @hook_MAP_element(%block* %8, %block* %11)
  %13 = alloca %map
  store %map %12, %map* %13
  %14 = alloca %map
  store %map %VarDotVar2, %map* %14
  %15 = call %map @hook_MAP_concat(%map* %13, %map* %14)
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %5, i64 0, i32 2
  store %map %15, %map* %16
  %17 = bitcast { %blockheader, [0 x i64], %map }* %5 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %19
}

define i1 @apply_rule_159() {
entry:
  ret i1 true
}

define i1 @apply_rule_160() {
entry:
  ret i1 true
}

define i1 @apply_rule_161(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_162() {
entry:
  ret i1 false
}

define i1 @apply_rule_163() {
entry:
  ret i1 true
}

define %block* @apply_rule_164(%block* %VarHOLE, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %8 = inttoptr i64 17179869185 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %18
}

define %block* @apply_rule_165(%block* %VarHOLE, %block* %VarK1, %block* %VarK2, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 4222141830529097 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 3
  store %block* %VarK1, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %4, i64 0, i32 4
  store %block* %VarK2, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %4 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %18
}

define i1 @apply_rule_166() {
entry:
  ret i1 false
}

define i1 @apply_rule_167() {
entry:
  ret i1 false
}

define %block* @apply_rule_168(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS1, %block* %VarS2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarS1, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %inj7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj7
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %VarS2, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %17, %block** %18
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %20
}

define %block* @apply_rule_169(%block* %VarHOLE, %block* %VarK1, %block* %VarK2, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356024 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 0
  store %blockheader { i64 281487861612557 }, %blockheader* %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356026 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %13 = bitcast i8* %malloccall9 to { %blockheader, [0 x i64], %block* }*
  %inj10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %13, i64 0, i32 0
  store %blockheader { i64 562958543356026 }, %blockheader* %inj10
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %13, i64 0, i32 2
  store %block* %VarK2, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %13 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %8 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %17, %block** %18
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %27
}

define i1 @apply_rule_170() {
entry:
  ret i1 false
}

define %block* @apply_rule_171(%block* %VarToReplace, %block* %VarReplacement, %block* %VarSource) {
entry:
  %0 = call %mpz* @hook_STRING_countAllOccurrences(%block* %VarSource, %block* %VarToReplace)
  %1 = call %block* @hook_STRING_replace(%block* %VarSource, %block* %VarToReplace, %block* %VarReplacement, %mpz* %0)
  ret %block* %1
}

define i1 @apply_rule_172(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_173() {
entry:
  ret i1 true
}

define %block* @apply_rule_174(%block* %VarHOLE, %block* %VarK0, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543355909 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj8
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %VarK0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %VarDotVar1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23
}

define %mpz* @apply_rule_175(%mpz* %VarI1) {
entry:
  ret %mpz* %VarI1
}

define i1 @apply_rule_176() {
entry:
  ret i1 true
}

define %block* @apply_rule_177(%block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355905 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 false, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_178() {
entry:
  ret i1 false
}

define i1 @apply_rule_179(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %0, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %0, i64 0, i32 2
  store %mpz* %VarI1, %mpz** %1
  %2 = bitcast { %blockheader, [0 x i64], %mpz* }* %0 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj2
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %VarI2, %mpz** %4
  %5 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %6 = call i1 @hook_KEQUAL_eq(%block* %2, %block* %5)
  ret i1 %6
}

define i1 @apply_rule_180() {
entry:
  ret i1 true
}

define i1 @apply_rule_181() {
entry:
  ret i1 false
}
