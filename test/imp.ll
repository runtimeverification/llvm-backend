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

define %block* @apply_rule_0(%block* %K2) {
entry:
  ret %block* %K2
}

; Function Attrs: noreturn
declare void @abort() #0

define %block* @apply_rule_1(%block* %K1, %block* %K2, %block* %K3) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612686 }, %blockheader* %kseq
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %K1, %block** %1
  %2 = alloca %block*
  %3 = call i1 @"eval_append{SortK{},SortK{}}"(%block** %2, %block* %K2, %block* %K3)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = load %block*, %block** %2
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %6

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare noalias i8* @malloc(i64)

declare i1 @"eval_append{SortK{},SortK{}}"(%block**, %block*, %block*)

define i1 @apply_rule_2() {
entry:
  ret i1 true
}

define i1 @apply_rule_3(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = alloca i1
  %1 = call i1 @hook_STRING_lt(i1* %0, %block* %VarS2, %block* %VarS1)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_lt(i1*, %block*, %block*)

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
  %0 = alloca i1
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %1, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %1, i64 0, i32 2
  store i1 %VarK1, i1* %2
  %3 = bitcast { %blockheader, [0 x i64], i1 }* %1 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 4503608217305214 }, %blockheader* %inj2
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %VarK2, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = call i1 @hook_KEQUAL_eq(i1* %0, %block* %3, %block* %6)
  br i1 %7, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %8 = load i1, i1* %0
  ret i1 %8

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_KEQUAL_eq(i1*, %block*, %block*)

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
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %2 = call i1 @hook_INT_shr(%mpz* %1, %mpz* %VarI, %mpz* %VarIDX)
  br i1 %2, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to %mpz*
  %4 = call i1 @hook_INT_shl(%mpz* %3, %mpz* @int_1, %mpz* %VarLEN)
  br i1 %4, label %notstuck3, label %stuck

notstuck3:                                        ; preds = %notstuck
  %5 = call i1 @hook_INT_emod(%mpz* %0, %mpz* %1, %mpz* %3)
  br i1 %5, label %notstuck4, label %stuck

notstuck4:                                        ; preds = %notstuck3
  ret %mpz* %0

stuck:                                            ; preds = %notstuck3, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_shr(%mpz*, %mpz*, %mpz*)

declare i1 @hook_INT_shl(%mpz*, %mpz*, %mpz*)

declare i1 @hook_INT_emod(%mpz*, %mpz*, %mpz*)

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
  %2 = alloca %block*
  %3 = call i1 @"eval_append{SortK{},SortK{}}"(%block** %2, %block* %VarI, %block* %VarDotVar1)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = load %block*, %block** %2
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %6, %block** %7
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %8, i64 0, i32 0
  store %blockheader { i64 844442110001154 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %9 = alloca %map
  %10 = alloca %map
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %VarX, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = call i1 @hook_MAP_element(%map* %10, %block* %13, %block* %VarI)
  br i1 %14, label %notstuck4, label %stuck

notstuck4:                                        ; preds = %notstuck
  %15 = load %map, %map* %10
  %16 = alloca %map
  store %map %15, %map* %16
  %17 = alloca %map
  store %map %VarDotVar2, %map* %17
  %18 = call i1 @hook_MAP_concat(%map* %9, %map* %16, %map* %17)
  br i1 %18, label %notstuck5, label %stuck

notstuck5:                                        ; preds = %notstuck4
  %19 = load %map, %map* %9
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %8, i64 0, i32 2
  store %map %19, %map* %20
  %21 = bitcast { %blockheader, [0 x i64], %map }* %8 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %21, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23

stuck:                                            ; preds = %notstuck4, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_MAP_element(%map*, %block*, %block*)

declare i1 @hook_MAP_concat(%map*, %map*, %map*)

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
  %4 = alloca i1
  %5 = call i1 @hook_INT_le(i1* %4, %mpz* %VarI1, %mpz* %VarI2)
  br i1 %5, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %6 = load i1, i1* %4
  %7 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %6, i1* %7
  %8 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
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

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_le(i1*, %mpz*, %mpz*)

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
  %14 = alloca %map
  store %map %VarRho, %map* %14
  %15 = alloca %map
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block* }*
  %inj6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj6
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarX, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %19 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %mpz* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %19, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj8
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %19, i64 0, i32 2
  store %mpz* @int_0, %mpz** %20
  %21 = bitcast { %blockheader, [0 x i64], %mpz* }* %19 to %block*
  %22 = call i1 @hook_MAP_element(%map* %15, %block* %18, %block* %21)
  br i1 %22, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %23 = load %map, %map* %15
  %24 = alloca %map
  store %map %23, %map* %24
  %25 = call i1 @hook_MAP_concat(%map* %13, %map* %14, %map* %24)
  br i1 %25, label %notstuck9, label %stuck

notstuck9:                                        ; preds = %notstuck
  %26 = load %map, %map* %13
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %12, i64 0, i32 2
  store %map %26, %map* %27
  %28 = bitcast { %blockheader, [0 x i64], %map }* %12 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %28, %block** %29
  %30 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %30

stuck:                                            ; preds = %notstuck, %entry
  call void @abort()
  unreachable
}

define %block* @apply_rule_47(%block* %VarB1) {
entry:
  ret %block* %VarB1
}

define i1 @apply_rule_48(%block* %VarS1, %block* %VarS2) {
entry:
  %0 = alloca i1
  %1 = call i1 @hook_STRING_lt(i1* %0, %block* %VarS2, %block* %VarS1)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  ret i1 %2

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
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
  %0 = alloca i1
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to %mpz*
  %2 = call i1 @hook_INT_tmod(%mpz* %1, %mpz* %VarI2, %mpz* %VarI1)
  br i1 %2, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %3 = call i1 @hook_INT_eq(i1* %0, %mpz* %1, %mpz* @int_0)
  br i1 %3, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %4 = load i1, i1* %0
  ret i1 %4

stuck:                                            ; preds = %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_tmod(%mpz*, %mpz*, %mpz*)

declare i1 @hook_INT_eq(i1*, %mpz*, %mpz*)

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
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to %mpz*
  %5 = call i1 @hook_INT_sub(%mpz* %4, %mpz* @int_0, %mpz* %VarI1)
  br i1 %5, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %6
  %7 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %15

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_sub(%mpz*, %mpz*, %mpz*)

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
  %0 = alloca i1
  %1 = call i1 @hook_STRING_eq(i1* %0, %block* %VarS1, %block* %VarS2)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_eq(i1*, %block*, %block*)

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
  %0 = alloca %block*
  %1 = alloca %block*
  %2 = alloca %block*
  %3 = call i1 @hook_STRING_int2string(%block** %2, %mpz* %VarI)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = load %block*, %block** %2
  %5 = call i1 @hook_STRING_concat(%block** %1, %block* bitcast ({ %blockheader, [3 x i8] }* @"token_\22_\22" to %block*), %block* %4)
  br i1 %5, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %6 = load %block*, %block** %1
  %7 = call i1 @hook_STRING_string2token(%block** %0, %block* %6)
  br i1 %7, label %notstuck2, label %stuck

notstuck2:                                        ; preds = %notstuck1
  %8 = load %block*, %block** %0
  ret %block* %8

stuck:                                            ; preds = %notstuck1, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_int2string(%block**, %mpz*)

declare i1 @hook_STRING_concat(%block**, %block*, %block*)

declare i1 @hook_STRING_string2token(%block**, %block*)

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
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to %mpz*
  %5 = call i1 @hook_INT_add(%mpz* %4, %mpz* %VarI1, %mpz* %VarI2)
  br i1 %5, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %6
  %7 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %15

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_add(%mpz*, %mpz*, %mpz*)

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
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to %mpz*
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to %mpz*
  %4 = call i1 @hook_INT_abs(%mpz* %3, %mpz* %VarI2)
  br i1 %4, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %5 = call i1 @hook_INT_tmod(%mpz* %2, %mpz* %VarI1, %mpz* %3)
  br i1 %5, label %notstuck4, label %stuck

notstuck4:                                        ; preds = %notstuck
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall5 to %mpz*
  %7 = call i1 @hook_INT_abs(%mpz* %6, %mpz* %VarI2)
  br i1 %7, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck4
  %8 = call i1 @hook_INT_add(%mpz* %1, %mpz* %2, %mpz* %6)
  br i1 %8, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck6
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall8 to %mpz*
  %10 = call i1 @hook_INT_abs(%mpz* %9, %mpz* %VarI2)
  br i1 %10, label %notstuck9, label %stuck

notstuck9:                                        ; preds = %notstuck7
  %11 = call i1 @hook_INT_tmod(%mpz* %0, %mpz* %1, %mpz* %9)
  br i1 %11, label %notstuck10, label %stuck

notstuck10:                                       ; preds = %notstuck9
  ret %mpz* %0

stuck:                                            ; preds = %notstuck9, %notstuck7, %notstuck6, %notstuck4, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_abs(%mpz*, %mpz*)

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
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall4 to %mpz*
  %5 = call i1 @hook_INT_tdiv(%mpz* %4, %mpz* %VarI1, %mpz* %VarI2)
  br i1 %5, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %4, %mpz** %6
  %7 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %15

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_tdiv(%mpz*, %mpz*, %mpz*)

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
  %1 = alloca %map
  %2 = call i1 @hook_MAP_unit(%map* %1)
  br i1 %2, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %3 = load %map, %map* %1
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 2
  store %map %3, %map* %4
  %5 = bitcast { %blockheader, [0 x i64], %map }* %0 to %block*
  ret %block* %5

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_MAP_unit(%map*)

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
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %2 = alloca %block*
  %3 = call i1 @hook_STRING_substr(%block** %2, %block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = load %block*, %block** %2
  %5 = call i1 @hook_STRING_rfind(%mpz* %1, %block* %VarS1, %block* %4, %mpz* %VarI)
  br i1 %5, label %notstuck2, label %stuck

notstuck2:                                        ; preds = %notstuck
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to %mpz*
  %7 = alloca %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall4 to %mpz*
  %9 = call i1 @hook_STRING_length(%mpz* %8, %block* %VarS2)
  br i1 %9, label %notstuck5, label %stuck

notstuck5:                                        ; preds = %notstuck2
  %10 = call i1 @hook_STRING_substr(%block** %7, %block* %VarS2, %mpz* @int_1, %mpz* %8)
  br i1 %10, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck5
  %11 = load %block*, %block** %7
  %12 = call i1 @hook_STRING_rfindChar(%mpz* %6, %block* %VarS1, %block* %11, %mpz* %VarI)
  br i1 %12, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck6
  %13 = call i1 @hook_INT_max(%mpz* %0, %mpz* %1, %mpz* %6)
  br i1 %13, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck7
  ret %mpz* %0

stuck:                                            ; preds = %notstuck7, %notstuck6, %notstuck5, %notstuck2, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_substr(%block**, %block*, %mpz*, %mpz*)

declare i1 @hook_STRING_rfind(%mpz*, %block*, %block*, %mpz*)

declare i1 @hook_STRING_length(%mpz*, %block*)

declare i1 @hook_STRING_rfindChar(%mpz*, %block*, %block*, %mpz*)

declare i1 @hook_INT_max(%mpz*, %mpz*, %mpz*)

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
  %0 = alloca i1
  %1 = call i1 @hook_KEQUAL_eq(i1* %0, %block* %VarK1, %block* %VarK2)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
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
  %0 = alloca i1
  %1 = call i1 @hook_STRING_lt(i1* %0, %block* %VarS1, %block* %VarS2)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
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
  %0 = alloca i1
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543356041 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarS1, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356041 }, %blockheader* %inj2
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarS2, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %7 = call i1 @hook_KEQUAL_eq(i1* %0, %block* %3, %block* %6)
  br i1 %7, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %8 = load i1, i1* %0
  ret i1 %8

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
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
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %1 = alloca i1
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to %mpz*
  %3 = alloca %block*
  %4 = call i1 @hook_STRING_substr(%block** %3, %block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  br i1 %4, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %5 = load %block*, %block** %3
  %6 = call i1 @hook_STRING_find(%mpz* %2, %block* %VarS1, %block* %5, %mpz* %VarI)
  br i1 %6, label %notstuck2, label %stuck

notstuck2:                                        ; preds = %notstuck
  %7 = call i1 @hook_INT_eq(i1* %1, %mpz* %2, %mpz* @int_-1)
  br i1 %7, label %notstuck3, label %stuck

notstuck3:                                        ; preds = %notstuck2
  %8 = load i1, i1* %1
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to %mpz*
  %10 = alloca %block*
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall5 to %mpz*
  %12 = call i1 @hook_STRING_length(%mpz* %11, %block* %VarS2)
  br i1 %12, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck3
  %13 = call i1 @hook_STRING_substr(%block** %10, %block* %VarS2, %mpz* @int_1, %mpz* %11)
  br i1 %13, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck6
  %14 = load %block*, %block** %10
  %15 = call i1 @hook_STRING_findChar(%mpz* %9, %block* %VarS1, %block* %14, %mpz* %VarI)
  br i1 %15, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck7
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall9 to %mpz*
  %17 = alloca i1
  %malloccall10 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %18 = bitcast i8* %malloccall10 to %mpz*
  %19 = alloca %block*
  %malloccall11 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall11 to %mpz*
  %21 = call i1 @hook_STRING_length(%mpz* %20, %block* %VarS2)
  br i1 %21, label %notstuck12, label %stuck

notstuck12:                                       ; preds = %notstuck8
  %22 = call i1 @hook_STRING_substr(%block** %19, %block* %VarS2, %mpz* @int_1, %mpz* %20)
  br i1 %22, label %notstuck13, label %stuck

notstuck13:                                       ; preds = %notstuck12
  %23 = load %block*, %block** %19
  %24 = call i1 @hook_STRING_findChar(%mpz* %18, %block* %VarS1, %block* %23, %mpz* %VarI)
  br i1 %24, label %notstuck14, label %stuck

notstuck14:                                       ; preds = %notstuck13
  %25 = call i1 @hook_INT_eq(i1* %17, %mpz* %18, %mpz* @int_-1)
  br i1 %25, label %notstuck15, label %stuck

notstuck15:                                       ; preds = %notstuck14
  %26 = load i1, i1* %17
  %malloccall16 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %27 = bitcast i8* %malloccall16 to %mpz*
  %28 = alloca %block*
  %29 = call i1 @hook_STRING_substr(%block** %28, %block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  br i1 %29, label %notstuck17, label %stuck

notstuck17:                                       ; preds = %notstuck15
  %30 = load %block*, %block** %28
  %31 = call i1 @hook_STRING_find(%mpz* %27, %block* %VarS1, %block* %30, %mpz* %VarI)
  br i1 %31, label %notstuck18, label %stuck

notstuck18:                                       ; preds = %notstuck17
  %malloccall19 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %32 = bitcast i8* %malloccall19 to %mpz*
  %malloccall20 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %33 = bitcast i8* %malloccall20 to %mpz*
  %34 = alloca %block*
  %35 = call i1 @hook_STRING_substr(%block** %34, %block* %VarS2, %mpz* @int_0, %mpz* @int_1)
  br i1 %35, label %notstuck21, label %stuck

notstuck21:                                       ; preds = %notstuck18
  %36 = load %block*, %block** %34
  %37 = call i1 @hook_STRING_find(%mpz* %33, %block* %VarS1, %block* %36, %mpz* %VarI)
  br i1 %37, label %notstuck22, label %stuck

notstuck22:                                       ; preds = %notstuck21
  %malloccall23 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %38 = bitcast i8* %malloccall23 to %mpz*
  %39 = alloca %block*
  %malloccall24 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %40 = bitcast i8* %malloccall24 to %mpz*
  %41 = call i1 @hook_STRING_length(%mpz* %40, %block* %VarS2)
  br i1 %41, label %notstuck25, label %stuck

notstuck25:                                       ; preds = %notstuck22
  %42 = call i1 @hook_STRING_substr(%block** %39, %block* %VarS2, %mpz* @int_1, %mpz* %40)
  br i1 %42, label %notstuck26, label %stuck

notstuck26:                                       ; preds = %notstuck25
  %43 = load %block*, %block** %39
  %44 = call i1 @hook_STRING_findChar(%mpz* %38, %block* %VarS1, %block* %43, %mpz* %VarI)
  br i1 %44, label %notstuck27, label %stuck

notstuck27:                                       ; preds = %notstuck26
  %45 = call i1 @hook_INT_min(%mpz* %32, %mpz* %33, %mpz* %38)
  br i1 %45, label %notstuck28, label %stuck

notstuck28:                                       ; preds = %notstuck27
  %46 = call i1 @hook_KEQUAL_ite(%mpz* %16, i1 %26, %mpz* %27, %mpz* %32)
  br i1 %46, label %notstuck29, label %stuck

notstuck29:                                       ; preds = %notstuck28
  %47 = call i1 @hook_KEQUAL_ite(%mpz* %0, i1 %8, %mpz* %9, %mpz* %16)
  br i1 %47, label %notstuck30, label %stuck

notstuck30:                                       ; preds = %notstuck29
  ret %mpz* %0

stuck:                                            ; preds = %notstuck29, %notstuck28, %notstuck27, %notstuck26, %notstuck25, %notstuck22, %notstuck21, %notstuck18, %notstuck17, %notstuck15, %notstuck14, %notstuck13, %notstuck12, %notstuck8, %notstuck7, %notstuck6, %notstuck3, %notstuck2, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_find(%mpz*, %block*, %block*, %mpz*)

declare i1 @hook_STRING_findChar(%mpz*, %block*, %block*, %mpz*)

declare i1 @hook_INT_min(%mpz*, %mpz*, %mpz*)

declare i1 @hook_KEQUAL_ite(%mpz*, i1, %mpz*, %mpz*)

define %mpz* @apply_rule_128(%block* %VarToCount, %block* %VarSource) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %2 = alloca %block*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to %mpz*
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to %mpz*
  %5 = call i1 @hook_STRING_find(%mpz* %4, %block* %VarSource, %block* %VarToCount, %mpz* @int_0)
  br i1 %5, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall4 to %mpz*
  %7 = call i1 @hook_STRING_length(%mpz* %6, %block* %VarToCount)
  br i1 %7, label %notstuck5, label %stuck

notstuck5:                                        ; preds = %notstuck
  %8 = call i1 @hook_INT_add(%mpz* %3, %mpz* %4, %mpz* %6)
  br i1 %8, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck5
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall7 to %mpz*
  %10 = call i1 @hook_STRING_length(%mpz* %9, %block* %VarSource)
  br i1 %10, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck6
  %11 = call i1 @hook_STRING_substr(%block** %2, %block* %VarSource, %mpz* %3, %mpz* %9)
  br i1 %11, label %notstuck9, label %stuck

notstuck9:                                        ; preds = %notstuck8
  %12 = load %block*, %block** %2
  %13 = call i1 @hook_STRING_countAllOccurrences(%mpz* %1, %block* %12, %block* %VarToCount)
  br i1 %13, label %notstuck10, label %stuck

notstuck10:                                       ; preds = %notstuck9
  %14 = call i1 @hook_INT_add(%mpz* %0, %mpz* @int_1, %mpz* %1)
  br i1 %14, label %notstuck11, label %stuck

notstuck11:                                       ; preds = %notstuck10
  ret %mpz* %0

stuck:                                            ; preds = %notstuck10, %notstuck9, %notstuck8, %notstuck6, %notstuck5, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_countAllOccurrences(%mpz*, %block*, %block*)

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
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to %mpz*
  %3 = call i1 @hook_INT_emod(%mpz* %2, %mpz* %VarI1, %mpz* %VarI2)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = call i1 @hook_INT_sub(%mpz* %1, %mpz* %VarI1, %mpz* %2)
  br i1 %4, label %notstuck3, label %stuck

notstuck3:                                        ; preds = %notstuck
  %5 = call i1 @hook_INT_tdiv(%mpz* %0, %mpz* %1, %mpz* %VarI2)
  br i1 %5, label %notstuck4, label %stuck

notstuck4:                                        ; preds = %notstuck3
  ret %mpz* %0

stuck:                                            ; preds = %notstuck3, %notstuck, %entry
  call void @abort()
  unreachable
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
  %1 = alloca %block*
  %2 = alloca %map
  store %map %VarInit, %map* %2
  %3 = call i1 @"eval_LblinitKCell{SortMap{}}"(%block** %1, %map* %2)
  br i1 %3, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %4 = load %block*, %block** %1
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = alloca %block*
  %7 = call i1 @"eval_LblinitStateCell{}"(%block** %6)
  br i1 %7, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %8 = load %block*, %block** %6
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %10

stuck:                                            ; preds = %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @"eval_LblinitKCell{SortMap{}}"(%block**, %map*)

declare i1 @"eval_LblinitStateCell{}"(%block**)

define %mpz* @apply_rule_143(%mpz* %VarI, %mpz* %VarLEN, %mpz* %VarIDX) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to %mpz*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %mpz*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to %mpz*
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to %mpz*
  %4 = call i1 @hook_INT_bitRange(%mpz* %3, %mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN)
  br i1 %4, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall4 to %mpz*
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall5 to %mpz*
  %7 = call i1 @hook_INT_sub(%mpz* %6, %mpz* %VarLEN, %mpz* @int_1)
  br i1 %7, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck
  %8 = call i1 @hook_INT_shl(%mpz* %5, %mpz* @int_1, %mpz* %6)
  br i1 %8, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck6
  %9 = call i1 @hook_INT_add(%mpz* %2, %mpz* %3, %mpz* %5)
  br i1 %9, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck7
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall9 to %mpz*
  %11 = call i1 @hook_INT_shl(%mpz* %10, %mpz* @int_1, %mpz* %VarLEN)
  br i1 %11, label %notstuck10, label %stuck

notstuck10:                                       ; preds = %notstuck8
  %12 = call i1 @hook_INT_emod(%mpz* %1, %mpz* %2, %mpz* %10)
  br i1 %12, label %notstuck11, label %stuck

notstuck11:                                       ; preds = %notstuck10
  %malloccall12 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %13 = bitcast i8* %malloccall12 to %mpz*
  %malloccall13 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall13 to %mpz*
  %15 = call i1 @hook_INT_sub(%mpz* %14, %mpz* %VarLEN, %mpz* @int_1)
  br i1 %15, label %notstuck14, label %stuck

notstuck14:                                       ; preds = %notstuck11
  %16 = call i1 @hook_INT_shl(%mpz* %13, %mpz* @int_1, %mpz* %14)
  br i1 %16, label %notstuck15, label %stuck

notstuck15:                                       ; preds = %notstuck14
  %17 = call i1 @hook_INT_sub(%mpz* %0, %mpz* %1, %mpz* %13)
  br i1 %17, label %notstuck16, label %stuck

notstuck16:                                       ; preds = %notstuck15
  ret %mpz* %0

stuck:                                            ; preds = %notstuck15, %notstuck14, %notstuck11, %notstuck10, %notstuck8, %notstuck7, %notstuck6, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_INT_bitRange(%mpz*, %mpz*, %mpz*, %mpz*)

define %block* @apply_rule_144(%mpz* %VarCount, %block* %VarToReplace, %block* %VarReplacement, %block* %VarSource) {
entry:
  %0 = alloca %block*
  %1 = alloca %block*
  %2 = alloca %block*
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to %mpz*
  %4 = call i1 @hook_STRING_find(%mpz* %3, %block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  br i1 %4, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %5 = call i1 @hook_STRING_substr(%block** %2, %block* %VarSource, %mpz* @int_0, %mpz* %3)
  br i1 %5, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %6 = load %block*, %block** %2
  %7 = call i1 @hook_STRING_concat(%block** %1, %block* %6, %block* %VarReplacement)
  br i1 %7, label %notstuck2, label %stuck

notstuck2:                                        ; preds = %notstuck1
  %8 = load %block*, %block** %1
  %9 = alloca %block*
  %10 = alloca %block*
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall3 to %mpz*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall4 to %mpz*
  %13 = call i1 @hook_STRING_find(%mpz* %12, %block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  br i1 %13, label %notstuck5, label %stuck

notstuck5:                                        ; preds = %notstuck2
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall6 to %mpz*
  %15 = call i1 @hook_STRING_length(%mpz* %14, %block* %VarToReplace)
  br i1 %15, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck5
  %16 = call i1 @hook_INT_add(%mpz* %11, %mpz* %12, %mpz* %14)
  br i1 %16, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck7
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %17 = bitcast i8* %malloccall9 to %mpz*
  %18 = call i1 @hook_STRING_length(%mpz* %17, %block* %VarSource)
  br i1 %18, label %notstuck10, label %stuck

notstuck10:                                       ; preds = %notstuck8
  %19 = call i1 @hook_STRING_substr(%block** %10, %block* %VarSource, %mpz* %11, %mpz* %17)
  br i1 %19, label %notstuck11, label %stuck

notstuck11:                                       ; preds = %notstuck10
  %20 = load %block*, %block** %10
  %malloccall12 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %21 = bitcast i8* %malloccall12 to %mpz*
  %22 = call i1 @hook_INT_sub(%mpz* %21, %mpz* %VarCount, %mpz* @int_1)
  br i1 %22, label %notstuck13, label %stuck

notstuck13:                                       ; preds = %notstuck11
  %23 = call i1 @hook_STRING_replace(%block** %9, %block* %20, %block* %VarToReplace, %block* %VarReplacement, %mpz* %21)
  br i1 %23, label %notstuck14, label %stuck

notstuck14:                                       ; preds = %notstuck13
  %24 = load %block*, %block** %9
  %25 = call i1 @hook_STRING_concat(%block** %0, %block* %8, %block* %24)
  br i1 %25, label %notstuck15, label %stuck

notstuck15:                                       ; preds = %notstuck14
  %26 = load %block*, %block** %0
  ret %block* %26

stuck:                                            ; preds = %notstuck14, %notstuck13, %notstuck11, %notstuck10, %notstuck8, %notstuck7, %notstuck5, %notstuck2, %notstuck1, %notstuck, %entry
  call void @abort()
  unreachable
}

declare i1 @hook_STRING_replace(%block**, %block*, %block*, %block*, %mpz*)

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
  %0 = alloca i1
  %1 = call i1 @hook_INT_eq(i1* %0, %mpz* %VarI1, %mpz* %VarI2)
  br i1 %1, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %2 = load i1, i1* %0
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

define %block* @apply_rule_151(%block* %VarToReplace, %block* %VarReplacement, %block* %VarSource) {
entry:
  %0 = alloca %block*
  %1 = alloca %block*
  %2 = alloca %block*
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to %mpz*
  %4 = call i1 @hook_STRING_find(%mpz* %3, %block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  br i1 %4, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %5 = call i1 @hook_STRING_substr(%block** %2, %block* %VarSource, %mpz* @int_0, %mpz* %3)
  br i1 %5, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %6 = load %block*, %block** %2
  %7 = call i1 @hook_STRING_concat(%block** %1, %block* %6, %block* %VarReplacement)
  br i1 %7, label %notstuck2, label %stuck

notstuck2:                                        ; preds = %notstuck1
  %8 = load %block*, %block** %1
  %9 = alloca %block*
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to %mpz*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to %mpz*
  %12 = call i1 @hook_STRING_find(%mpz* %11, %block* %VarSource, %block* %VarToReplace, %mpz* @int_0)
  br i1 %12, label %notstuck5, label %stuck

notstuck5:                                        ; preds = %notstuck2
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %13 = bitcast i8* %malloccall6 to %mpz*
  %14 = call i1 @hook_STRING_length(%mpz* %13, %block* %VarToReplace)
  br i1 %14, label %notstuck7, label %stuck

notstuck7:                                        ; preds = %notstuck5
  %15 = call i1 @hook_INT_add(%mpz* %10, %mpz* %11, %mpz* %13)
  br i1 %15, label %notstuck8, label %stuck

notstuck8:                                        ; preds = %notstuck7
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall9 to %mpz*
  %17 = call i1 @hook_STRING_length(%mpz* %16, %block* %VarSource)
  br i1 %17, label %notstuck10, label %stuck

notstuck10:                                       ; preds = %notstuck8
  %18 = call i1 @hook_STRING_substr(%block** %9, %block* %VarSource, %mpz* %10, %mpz* %16)
  br i1 %18, label %notstuck11, label %stuck

notstuck11:                                       ; preds = %notstuck10
  %19 = load %block*, %block** %9
  %20 = call i1 @hook_STRING_concat(%block** %0, %block* %8, %block* %19)
  br i1 %20, label %notstuck12, label %stuck

notstuck12:                                       ; preds = %notstuck11
  %21 = load %block*, %block** %0
  ret %block* %21

stuck:                                            ; preds = %notstuck11, %notstuck10, %notstuck8, %notstuck7, %notstuck5, %notstuck2, %notstuck1, %notstuck, %entry
  call void @abort()
  unreachable
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
  %1 = alloca %block*
  %2 = alloca %map
  store %map %VarInit, %map* %2
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356034 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @"token_$PGM" to %block*), %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = call i1 @hook_MAP_lookup(%block** %1, %map* %2, %block* %5)
  br i1 %6, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %7 = load %block*, %block** %1
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %9

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare i1 @hook_MAP_lookup(%block**, %map*, %block*)

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
  %6 = alloca %map
  %7 = alloca %map
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356036 }, %blockheader* %inj
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %VarX, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %mpz* }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %11, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj5
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %11, i64 0, i32 2
  store %mpz* %VarI, %mpz** %12
  %13 = bitcast { %blockheader, [0 x i64], %mpz* }* %11 to %block*
  %14 = call i1 @hook_MAP_element(%map* %7, %block* %10, %block* %13)
  br i1 %14, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %15 = load %map, %map* %7
  %16 = alloca %map
  store %map %15, %map* %16
  %17 = alloca %map
  store %map %VarDotVar2, %map* %17
  %18 = call i1 @hook_MAP_concat(%map* %6, %map* %16, %map* %17)
  br i1 %18, label %notstuck6, label %stuck

notstuck6:                                        ; preds = %notstuck
  %19 = load %map, %map* %6
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %5, i64 0, i32 2
  store %map %19, %map* %20
  %21 = bitcast { %blockheader, [0 x i64], %map }* %5 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %21, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %23

stuck:                                            ; preds = %notstuck, %entry
  call void @abort()
  unreachable
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
  %0 = alloca %block*
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to %mpz*
  %2 = call i1 @hook_STRING_countAllOccurrences(%mpz* %1, %block* %VarSource, %block* %VarToReplace)
  br i1 %2, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %3 = call i1 @hook_STRING_replace(%block** %0, %block* %VarSource, %block* %VarToReplace, %block* %VarReplacement, %mpz* %1)
  br i1 %3, label %notstuck1, label %stuck

notstuck1:                                        ; preds = %notstuck
  %4 = load %block*, %block** %0
  ret %block* %4

stuck:                                            ; preds = %notstuck, %entry
  call void @abort()
  unreachable
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
  %0 = alloca i1
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %1, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %1, i64 0, i32 2
  store %mpz* %VarI1, %mpz** %2
  %3 = bitcast { %blockheader, [0 x i64], %mpz* }* %1 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %4, i64 0, i32 0
  store %blockheader { i64 2814758357041277 }, %blockheader* %inj2
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %4, i64 0, i32 2
  store %mpz* %VarI2, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz* }* %4 to %block*
  %7 = call i1 @hook_KEQUAL_eq(i1* %0, %block* %3, %block* %6)
  br i1 %7, label %notstuck, label %stuck

notstuck:                                         ; preds = %entry
  %8 = load i1, i1* %0
  ret i1 %8

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

define i1 @apply_rule_180() {
entry:
  ret i1 true
}

define i1 @apply_rule_181() {
entry:
  ret i1 false
}

attributes #0 = { noreturn }
