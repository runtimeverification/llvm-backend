target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64* }
%blockheader = type { i64 }
%block = type { %blockheader, [0 x i64*] }
%map = type { i64, i8*, i8* }

@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer
@int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_1_limbs, i32 0, i32 0) }
@int_1_limbs = global [1 x i64] [i64 1]
@"token_$PGM" = global { %blockheader, [4 x i8] } { %blockheader { i64 4 }, [4 x i8] c"$PGM" }
@token_token = global { %blockheader, [5 x i8] } { %blockheader { i64 5 }, [5 x i8] c"token" }
@int_2 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_2_limbs, i32 0, i32 0) }
@int_2_limbs = global [1 x i64] [i64 2]

declare %block* @parseConfiguration(i8*)

define %block* @apply_rule_0(%block* %K2) {
entry:
  ret %block* %K2
}

define %block* @apply_rule_1(%block* %K1, %block* %K2, %block* %K3) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %K1, %block** %1
  %2 = call %block* @"eval_append{SortK{},SortK{}}"(%block* %K2, %block* %K3)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %4
}

declare noalias i8* @malloc(i64)

declare %block* @"eval_append{SortK{},SortK{}}"(%block*, %block*)

define %block* @apply_rule_2(%block* %VarB2) {
entry:
  ret %block* %VarB2
}

define i1 @apply_rule_3() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_4(%mpz* %VarI1, %mpz* %VarI2) {
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

declare %mpz* @hook_INT_tmod(%mpz*, %mpz*)

declare %mpz* @hook_INT_add(%mpz*, %mpz*)

define i1 @apply_rule_5() {
entry:
  ret i1 false
}

define %block* @apply_rule_6(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 0
  store %blockheader { i64 2251808403619899 }, %blockheader* %inj
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  %3 = call %mpz* @hook_INT_add(%mpz* @int_0, %mpz* @int_1)
  br label %hook_KEQUAL_ite

else:                                             ; preds = %entry
  br label %hook_KEQUAL_ite

hook_KEQUAL_ite:                                  ; preds = %else, %then
  %phi = phi %mpz* [ %3, %then ], [ @int_0, %else ]
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 2
  store %mpz* %phi, %mpz** %4
  %5 = bitcast { %blockheader, [0 x i64], %mpz* }* %2 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %10
}

define i1 @apply_rule_7(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_8() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_9(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_emod(%mpz* %VarI1, %mpz* %VarI2)
  %1 = call %mpz* @hook_INT_sub(%mpz* %VarI1, %mpz* %0)
  %2 = call %mpz* @hook_INT_tdiv(%mpz* %1, %mpz* %VarI2)
  ret %mpz* %2
}

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

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
  ret i1 true
}

define i1 @apply_rule_14() {
entry:
  ret i1 true
}

define i1 @apply_rule_15(%block* %VarK1, %block* %VarK2) {
entry:
  %0 = call i1 @hook_KEQUAL_eq(%block* %VarK1, %block* %VarK2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_KEQUAL_eq(%block*, %block*)

define i1 @apply_rule_16() {
entry:
  ret i1 false
}

define i1 @apply_rule_17() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_18(%mpz* %VarI1) {
entry:
  ret %mpz* %VarI1
}

define %block* @apply_rule_19(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041184 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_eq = icmp eq i1 true, false
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %hook_BOOL_eq, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12
}

define %mpz* @apply_rule_20(%mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN) {
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

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

define i1 @apply_rule_21() {
entry:
  ret i1 true
}

define i1 @apply_rule_22(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_tmod(%mpz* %VarI2, %mpz* %VarI1)
  %1 = call i1 @hook_INT_eq(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

declare i1 @hook_INT_eq(%mpz*, %mpz*)

define i1 @apply_rule_23() {
entry:
  ret i1 false
}

define i1 @apply_rule_24() {
entry:
  ret i1 true
}

define %block* @apply_rule_25(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041202 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_not = xor i1 true, true
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %hook_BOOL_not, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12
}

define i1 @apply_rule_26(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %0, i64 0, i32 0
  store %blockheader { i64 2251808403619899 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %0, i64 0, i32 2
  store %mpz* %VarI1, %mpz** %1
  %2 = bitcast { %blockheader, [0 x i64], %mpz* }* %0 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 2251808403619899 }, %blockheader* %inj2
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %VarI2, %mpz** %4
  %5 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %6 = call i1 @hook_KEQUAL_eq(%block* %2, %block* %5)
  ret i1 %6
}

define %block* @apply_rule_27(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041184 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_ne = xor i1 false, false
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %hook_BOOL_ne, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12
}

define i1 @apply_rule_28(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define %mpz* @apply_rule_29(%mpz* %VarI) {
entry:
  ret %mpz* %VarI
}

define i1 @apply_rule_30(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_31() {
entry:
  ret i1 false
}

define i1 @apply_rule_32() {
entry:
  ret i1 false
}

define i1 @apply_rule_33() {
entry:
  ret i1 true
}

define i1 @apply_rule_34(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_35() {
entry:
  ret i1 true
}

define i1 @apply_rule_36() {
entry:
  ret i1 true
}

define i1 @apply_rule_37() {
entry:
  ret i1 false
}

define i1 @apply_rule_38(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_39() {
entry:
  ret i1 true
}

define i1 @apply_rule_40() {
entry:
  ret i1 false
}

define i1 @apply_rule_41(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_42(%map %VarM) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %1 = alloca %map
  store %map %VarM, %map* %1
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 0
  store %blockheader { i64 2251808403619899 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 2
  store %mpz* @int_0, %mpz** %3
  %4 = bitcast { %blockheader, [0 x i64], %mpz* }* %2 to %block*
  %5 = call %block* @hook_MAP_lookup(%map* %1, %block* %4)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %7
}

declare %block* @hook_MAP_lookup(%map*, %block*)

define i1 @apply_rule_43() {
entry:
  ret i1 true
}

define i1 @apply_rule_44() {
entry:
  ret i1 false
}

define i1 @apply_rule_45() {
entry:
  ret i1 true
}

define i1 @apply_rule_46() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_47(%mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN) {
entry:
  %0 = call %mpz* @hook_INT_shr(%mpz* %VarI, %mpz* %VarIDX)
  %1 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %VarLEN)
  %2 = call %mpz* @hook_INT_emod(%mpz* %0, %mpz* %1)
  ret %mpz* %2
}

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

define i1 @apply_rule_48() {
entry:
  ret i1 true
}

define i1 @apply_rule_49() {
entry:
  ret i1 true
}

define i1 @apply_rule_50(i1 %VarK) {
entry:
  ret i1 %VarK
}

define %block* @apply_rule_51(%map %VarInit) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %1 = alloca %map
  store %map %VarInit, %map* %1
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645313 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @"token_$PGM" to %block*), %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %5 = call %block* @hook_MAP_lookup(%map* %1, %block* %4)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %7
}

define i1 @apply_rule_52() {
entry:
  ret i1 true
}

define i1 @apply_rule_53() {
entry:
  ret i1 true
}

define i1 @apply_rule_54(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_55() {
entry:
  ret i1 false
}

define i1 @apply_rule_56() {
entry:
  ret i1 true
}

define i1 @apply_rule_57(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_eq(%mpz* %VarI1, %mpz* %VarI2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_58() {
entry:
  ret i1 true
}

define i1 @apply_rule_59(i1 %VarK1, i1 %VarK2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %0, i64 0, i32 0
  store %blockheader { i64 2814758357041215 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %0, i64 0, i32 2
  store i1 %VarK1, i1* %1
  %2 = bitcast { %blockheader, [0 x i64], i1 }* %0 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041215 }, %blockheader* %inj2
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %VarK2, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = call i1 @hook_KEQUAL_eq(%block* %2, %block* %5)
  ret i1 %6
}

define i1 @apply_rule_60(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_61(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645306 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [5 x i8] }* @token_token to %block*), %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %9
}

define i1 @apply_rule_62(i1 %VarB) {
entry:
  %hook_BOOL_not = xor i1 %VarB, true
  ret i1 %hook_BOOL_not
}

define %mpz* @apply_rule_63(%mpz* %VarI2) {
entry:
  ret %mpz* %VarI2
}

define i1 @apply_rule_64() {
entry:
  ret i1 false
}

define i1 @apply_rule_65() {
entry:
  ret i1 false
}

define i1 @apply_rule_66() {
entry:
  ret i1 true
}

define %block* @apply_rule_67(%block* %VarB1) {
entry:
  ret %block* %VarB1
}

define i1 @apply_rule_68() {
entry:
  ret i1 true
}

define i1 @apply_rule_69() {
entry:
  ret i1 true
}

define i1 @apply_rule_70() {
entry:
  ret i1 true
}

define %block* @apply_rule_71(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz*, %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int" = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1125912791744537 }, %blockheader* %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %3, i64 0, i32 2
  store %mpz* @int_1, %mpz** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %3, i64 0, i32 3
  store %mpz* @int_2, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz*, %mpz* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %13
}

define i1 @apply_rule_72() {
entry:
  ret i1 false
}

define i1 @apply_rule_73() {
entry:
  ret i1 true
}

define %block* @apply_rule_74() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %2 = inttoptr i64 124554051585 to %block*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %6
}

define i1 @apply_rule_75(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_76() {
entry:
  ret i1 true
}

define i1 @apply_rule_77() {
entry:
  ret i1 false
}

define %block* @apply_rule_78(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041182 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  br label %hook_BOOL_and

hook_BOOL_and:                                    ; preds = %then, %entry
  %phi = phi i1 [ true, %then ], [ true, %entry ]
  br i1 %phi, label %then4, label %hook_BOOL_and5

then4:                                            ; preds = %hook_BOOL_and
  br i1 false, label %then6, label %hook_BOOL_and7

hook_BOOL_and5:                                   ; preds = %hook_BOOL_and7, %hook_BOOL_and
  %phi9 = phi i1 [ %phi8, %hook_BOOL_and7 ], [ %phi, %hook_BOOL_and ]
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %phi9, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12

then6:                                            ; preds = %then4
  br label %hook_BOOL_and7

hook_BOOL_and7:                                   ; preds = %then6, %then4
  %phi8 = phi i1 [ false, %then6 ], [ false, %then4 ]
  br label %hook_BOOL_and5
}

define i1 @apply_rule_79() {
entry:
  ret i1 false
}

define i1 @apply_rule_80() {
entry:
  ret i1 true
}

define i1 @apply_rule_81() {
entry:
  ret i1 false
}

define i1 @apply_rule_82(i1 %VarK) {
entry:
  ret i1 %VarK
}

define %block* @apply_rule_83(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041202 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
  br i1 true, label %then, label %hook_BOOL_implies

then:                                             ; preds = %entry
  br label %hook_BOOL_implies

hook_BOOL_implies:                                ; preds = %then, %entry
  %phi = phi i1 [ false, %then ], [ true, %entry ]
  br i1 %phi, label %then4, label %hook_BOOL_implies5

then4:                                            ; preds = %hook_BOOL_implies
  br label %hook_BOOL_implies5

hook_BOOL_implies5:                               ; preds = %then4, %hook_BOOL_implies
  %phi6 = phi i1 [ false, %then4 ], [ true, %hook_BOOL_implies ]
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %phi6, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12
}

define i1 @apply_rule_84() {
entry:
  ret i1 true
}

define i1 @apply_rule_85(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_86(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 1688862745165890 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645310 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 2814758357041182 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
  br i1 false, label %hook_BOOL_or, label %else

else:                                             ; preds = %entry
  br label %hook_BOOL_or

hook_BOOL_or:                                     ; preds = %else, %entry
  %phi = phi i1 [ false, %else ], [ false, %entry ]
  br i1 %phi, label %hook_BOOL_or5, label %else4

else4:                                            ; preds = %hook_BOOL_or
  br i1 true, label %hook_BOOL_or7, label %else6

hook_BOOL_or5:                                    ; preds = %hook_BOOL_or7, %hook_BOOL_or
  %phi9 = phi i1 [ %phi8, %hook_BOOL_or7 ], [ %phi, %hook_BOOL_or ]
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %phi9, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %VarDotVar0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %12

else6:                                            ; preds = %else4
  br label %hook_BOOL_or7

hook_BOOL_or7:                                    ; preds = %else6, %else4
  %phi8 = phi i1 [ true, %else6 ], [ true, %else4 ]
  br label %hook_BOOL_or5
}

define i1 @apply_rule_87(i1 %VarB) {
entry:
  ret i1 %VarB
}
