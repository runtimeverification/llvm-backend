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
@token_2450474d = global { %blockheader, [4 x i8] } { %blockheader { i64 4 }, [4 x i8] c"$PGM" }
@token_746f6b656e = global { %blockheader, [5 x i8] } { %blockheader { i64 5 }, [5 x i8] c"token" }
@int_2 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_2_limbs, i32 0, i32 0) }
@int_2_limbs = global [1 x i64] [i64 2]

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define fastcc %block* @apply_rule_0(%block*) {
entry:
  ret %block* %0
}

define fastcc %block* @apply_rule_1(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %0, %block** %4
  %5 = call fastcc %block* @"eval_append{SortK{}, SortK{}}"(%block* %1, %block* %2)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %7
}

declare noalias i8* @malloc(i64)

declare %block* @"eval_append{SortK{}, SortK{}}"(%block*, %block*)

define fastcc %block* @apply_rule_145(%block*) {
entry:
  ret %block* %0
}

define fastcc i1 @apply_rule_146() {
entry:
  ret i1 true
}

define fastcc %mpz* @apply_rule_147(%mpz*, %mpz*) {
entry:
  %2 = call %mpz* @hook_INT_abs(%mpz* %1)
  %3 = call %mpz* @hook_INT_tmod(%mpz* %0, %mpz* %2)
  %4 = call %mpz* @hook_INT_abs(%mpz* %1)
  %5 = call %mpz* @hook_INT_add(%mpz* %3, %mpz* %4)
  %6 = call %mpz* @hook_INT_abs(%mpz* %1)
  %7 = call %mpz* @hook_INT_tmod(%mpz* %5, %mpz* %6)
  ret %mpz* %7
}

declare %mpz* @hook_INT_abs(%mpz*)

declare %mpz* @hook_INT_tmod(%mpz*, %mpz*)

declare %mpz* @hook_INT_add(%mpz*, %mpz*)

define fastcc i1 @apply_rule_148() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_149(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1407383473487972 }, %blockheader* %inj
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  %4 = call %mpz* @hook_INT_add(%mpz* @int_0, %mpz* @int_1)
  br label %hook_KEQUAL_ite

else:                                             ; preds = %entry
  br label %hook_KEQUAL_ite

hook_KEQUAL_ite:                                  ; preds = %else, %then
  %phi = phi %mpz* [ %4, %then ], [ @int_0, %else ]
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %phi, %mpz** %5
  %6 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %11
}

define fastcc i1 @apply_rule_150(i1, i1) {
entry:
  %hook_BOOL_eq = icmp eq i1 %0, %1
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define fastcc i1 @apply_rule_151() {
entry:
  ret i1 true
}

define fastcc %mpz* @apply_rule_152(%mpz*, %mpz*) {
entry:
  %2 = call %mpz* @hook_INT_emod(%mpz* %0, %mpz* %1)
  %3 = call %mpz* @hook_INT_sub(%mpz* %0, %mpz* %2)
  %4 = call %mpz* @hook_INT_tdiv(%mpz* %3, %mpz* %1)
  ret %mpz* %4
}

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

define fastcc i1 @apply_rule_153() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_154() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_155() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_156() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_157() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_158(%block*, %block*) {
entry:
  %2 = call i1 @hook_KEQUAL_eq(%block* %0, %block* %1)
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_KEQUAL_eq(%block*, %block*)

define fastcc i1 @apply_rule_159() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_160() {
entry:
  ret i1 false
}

define fastcc %mpz* @apply_rule_161(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc %block* @apply_rule_162(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147776 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_eq = icmp eq i1 true, false
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %hook_BOOL_eq, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13
}

define fastcc %mpz* @apply_rule_163(%mpz*, %mpz*, %mpz*) {
entry:
  %3 = call %mpz* @hook_INT_bitRange(%mpz* %0, %mpz* %1, %mpz* %2)
  %4 = call %mpz* @hook_INT_sub(%mpz* %2, %mpz* @int_1)
  %5 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %4)
  %6 = call %mpz* @hook_INT_add(%mpz* %3, %mpz* %5)
  %7 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %2)
  %8 = call %mpz* @hook_INT_emod(%mpz* %6, %mpz* %7)
  %9 = call %mpz* @hook_INT_sub(%mpz* %2, %mpz* @int_1)
  %10 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %9)
  %11 = call %mpz* @hook_INT_sub(%mpz* %8, %mpz* %10)
  ret %mpz* %11
}

declare %mpz* @hook_INT_bitRange(%mpz*, %mpz*, %mpz*)

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

define fastcc i1 @apply_rule_164() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_165(%mpz*, %mpz*) {
entry:
  %2 = call %mpz* @hook_INT_tmod(%mpz* %1, %mpz* %0)
  %3 = call i1 @hook_INT_eq(%mpz* %2, %mpz* @int_0)
  ret i1 %3
}

declare i1 @hook_INT_eq(%mpz*, %mpz*)

define fastcc i1 @apply_rule_166() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_167() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_168(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147799 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_not = xor i1 true, true
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %hook_BOOL_not, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13
}

define fastcc i1 @apply_rule_169(%mpz*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 0
  store %blockheader { i64 1407383473487972 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 2
  store %mpz* %0, %mpz** %3
  %4 = bitcast { %blockheader, [0 x i64], %mpz* }* %2 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %5, i64 0, i32 0
  store %blockheader { i64 1407383473487972 }, %blockheader* %inj2
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %5, i64 0, i32 2
  store %mpz* %1, %mpz** %6
  %7 = bitcast { %blockheader, [0 x i64], %mpz* }* %5 to %block*
  %8 = call i1 @hook_KEQUAL_eq(%block* %4, %block* %7)
  ret i1 %8
}

define fastcc %block* @apply_rule_170(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147776 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
  %hook_BOOL_ne = xor i1 false, false
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %hook_BOOL_ne, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13
}

define fastcc i1 @apply_rule_171(i1, i1) {
entry:
  %hook_BOOL_eq = icmp eq i1 %0, %1
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define fastcc %mpz* @apply_rule_172(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc i1 @apply_rule_173(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_174() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_175() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_176() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_177(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_178() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_179() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_180() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_181(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_182() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_183() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_184(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_185(%map*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 0
  store %blockheader { i64 1407383473487972 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %2, i64 0, i32 2
  store %mpz* @int_0, %mpz** %3
  %4 = bitcast { %blockheader, [0 x i64], %mpz* }* %2 to %block*
  %5 = call %block* @hook_MAP_lookup(%map* %0, %block* %4)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %7
}

declare %block* @hook_MAP_lookup(%map*, %block*)

define fastcc i1 @apply_rule_186() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_187() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_188() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_189() {
entry:
  ret i1 false
}

define fastcc %mpz* @apply_rule_190(%mpz*, %mpz*, %mpz*) {
entry:
  %3 = call %mpz* @hook_INT_shr(%mpz* %0, %mpz* %1)
  %4 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %2)
  %5 = call %mpz* @hook_INT_emod(%mpz* %3, %mpz* %4)
  ret %mpz* %5
}

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

define fastcc i1 @apply_rule_191() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_192() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_193(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_194(%map*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645358 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @token_2450474d to %block*), %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %5 = call %block* @hook_MAP_lookup(%map* %0, %block* %4)
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %7
}

define fastcc i1 @apply_rule_195() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_196() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_197(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_198() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_199() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_200(%mpz*, %mpz*) {
entry:
  %2 = call i1 @hook_INT_eq(%mpz* %0, %mpz* %1)
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not
}

define fastcc i1 @apply_rule_201() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_202(i1, i1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %2, i64 0, i32 0
  store %blockheader { i64 5629508124147819 }, %blockheader* %inj
  %3 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %2, i64 0, i32 2
  store i1 %0, i1* %3
  %4 = bitcast { %blockheader, [0 x i64], i1 }* %2 to %block*
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %5, i64 0, i32 0
  store %blockheader { i64 5629508124147819 }, %blockheader* %inj2
  %6 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %5, i64 0, i32 2
  store i1 %1, i1* %6
  %7 = bitcast { %blockheader, [0 x i64], i1 }* %5 to %block*
  %8 = call i1 @hook_KEQUAL_eq(%block* %4, %block* %7)
  ret i1 %8
}

define fastcc i1 @apply_rule_203(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_204(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645352 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [5 x i8] }* @token_746f6b656e to %block*), %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %10
}

define fastcc i1 @apply_rule_205(i1) {
entry:
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define fastcc %mpz* @apply_rule_206(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc i1 @apply_rule_207() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_208() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_209() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_210(%block*) {
entry:
  ret %block* %0
}

define fastcc i1 @apply_rule_211() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_212() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_213() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_214(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz*, %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int" = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %4, i64 0, i32 0
  store %blockheader { i64 1688862745165870 }, %blockheader* %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %4, i64 0, i32 2
  store %mpz* @int_1, %mpz** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %4, i64 0, i32 3
  store %mpz* @int_2, %mpz** %6
  %7 = bitcast { %blockheader, [0 x i64], %mpz*, %mpz* }* %4 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %14
}

define fastcc i1 @apply_rule_215() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_216() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_217() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %2 = inttoptr i64 253403070465 to %block*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %6
}

define fastcc i1 @apply_rule_218(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_219() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_220() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_221(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147772 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
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
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %phi9, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13

then6:                                            ; preds = %then4
  br label %hook_BOOL_and7

hook_BOOL_and7:                                   ; preds = %then6, %then4
  %phi8 = phi i1 [ false, %then6 ], [ false, %then4 ]
  br label %hook_BOOL_and5
}

define fastcc i1 @apply_rule_222() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_223() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_224() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_225(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_226(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147799 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
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
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %phi6, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13
}

define fastcc i1 @apply_rule_227() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_228(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_229(%block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 3659187582140527 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645356 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 0
  store %blockheader { i64 5629508124147772 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
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
  %5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %4, i64 0, i32 2
  store i1 %phi9, i1* %5
  %6 = bitcast { %blockheader, [0 x i64], i1 }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13

else6:                                            ; preds = %else4
  br label %hook_BOOL_or7

hook_BOOL_or7:                                    ; preds = %else6, %else4
  %phi8 = phi i1 [ true, %else6 ], [ true, %else4 ]
  br label %hook_BOOL_or5
}

define fastcc i1 @apply_rule_230(i1) {
entry:
  ret i1 %0
}
