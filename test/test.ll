target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%map = type { i64, i8*, i8* }

@token_token = global { %blockheader, [5 x i8] } { %blockheader { i64 5 }, [5 x i8] c"token" }
@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer
@int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_1_limbs, i32 0, i32 0) }
@int_1_limbs = global [1 x i64] [i64 1]
@int_2 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_2_limbs, i32 0, i32 0) }
@int_2_limbs = global [1 x i64] [i64 2]
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
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
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

define i1 @apply_rule_3(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_4() {
entry:
  ret i1 true
}

define i1 @apply_rule_5() {
entry:
  ret i1 true
}

define i1 @apply_rule_6() {
entry:
  ret i1 false
}

define %block* @apply_rule_7(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645285 }, %blockheader* %inj
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

define i1 @apply_rule_8() {
entry:
  ret i1 false
}

define %block* @apply_rule_9(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198540 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
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
  ret i1 false
}

define i1 @apply_rule_13() {
entry:
  ret i1 true
}

define i1 @apply_rule_14() {
entry:
  ret i1 true
}

define i1 @apply_rule_15() {
entry:
  ret i1 true
}

define i1 @apply_rule_16(i1 %VarB) {
entry:
  %hook_BOOL_not = xor i1 %VarB, true
  ret i1 %hook_BOOL_not
}

define %block* @apply_rule_17(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198540 }, %blockheader* %"Lblbaz'LParUndsRParUnds'TEST'UndsUnds'Bool"
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

define i1 @apply_rule_18() {
entry:
  ret i1 false
}

define i1 @apply_rule_19() {
entry:
  ret i1 true
}

define i1 @apply_rule_20() {
entry:
  ret i1 true
}

define i1 @apply_rule_21(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_22(%map %VarM) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %1 = alloca %block*
  %2 = alloca %map
  store %map %VarM, %map* %2
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 3096233333751848 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* @int_0, %mpz** %4
  %5 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
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

define %block* @apply_rule_23(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198557 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
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

define %block* @apply_rule_24(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198557 }, %blockheader* %"Lblnot'LParUndsRParUnds'TEST'UndsUnds'Bool"
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

define i1 @apply_rule_25() {
entry:
  ret i1 false
}

define i1 @apply_rule_26(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_27(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_28(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_29(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_30(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz*, %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int" = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 844437815033865 }, %blockheader* %"Lbl'UndsPlusUndsUnds'TEST'UndsUnds'Int'Unds'Int"
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

define i1 @apply_rule_31() {
entry:
  ret i1 true
}

define i1 @apply_rule_32() {
entry:
  ret i1 true
}

define i1 @apply_rule_33() {
entry:
  ret i1 false
}

define i1 @apply_rule_34() {
entry:
  ret i1 false
}

define %block* @apply_rule_35() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %2 = inttoptr i64 47244640257 to %block*
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  ret %block* %6
}

define i1 @apply_rule_36() {
entry:
  ret i1 true
}

define i1 @apply_rule_37() {
entry:
  ret i1 true
}

define i1 @apply_rule_38(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_39() {
entry:
  ret i1 false
}

define i1 @apply_rule_40() {
entry:
  ret i1 false
}

define i1 @apply_rule_41() {
entry:
  ret i1 true
}

define i1 @apply_rule_42() {
entry:
  ret i1 true
}

define i1 @apply_rule_43() {
entry:
  ret i1 true
}

define i1 @apply_rule_44() {
entry:
  ret i1 true
}

define i1 @apply_rule_45(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_46() {
entry:
  ret i1 false
}

define i1 @apply_rule_47(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_48() {
entry:
  ret i1 false
}

define i1 @apply_rule_49() {
entry:
  ret i1 true
}

define i1 @apply_rule_50() {
entry:
  ret i1 false
}

define i1 @apply_rule_51() {
entry:
  ret i1 true
}

define i1 @apply_rule_52(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_53(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_54() {
entry:
  ret i1 true
}

define i1 @apply_rule_55() {
entry:
  ret i1 true
}

define i1 @apply_rule_56() {
entry:
  ret i1 true
}

define i1 @apply_rule_57() {
entry:
  ret i1 true
}

define i1 @apply_rule_58() {
entry:
  ret i1 false
}

define %block* @apply_rule_59(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198541 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
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

define %block* @apply_rule_60(%map %VarInit) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %1 = alloca %block*
  %2 = alloca %map
  store %map %VarInit, %map* %2
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281483566645286 }, %blockheader* %inj
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

define i1 @apply_rule_61() {
entry:
  ret i1 true
}

define i1 @apply_rule_62() {
entry:
  ret i1 true
}

define i1 @apply_rule_63(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_64() {
entry:
  ret i1 false
}

define i1 @apply_rule_65() {
entry:
  ret i1 true
}

define i1 @apply_rule_66() {
entry:
  ret i1 false
}

define i1 @apply_rule_67() {
entry:
  ret i1 true
}

define i1 @apply_rule_68() {
entry:
  ret i1 false
}

define %block* @apply_rule_69(%block* %VarDotVar0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281483566645248 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 2251812698587180 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281483566645284 }, %blockheader* %inj
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool" = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198541 }, %blockheader* %"Lbleq'LParUndsRParUnds'TEST'UndsUnds'Bool"
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

define i1 @apply_rule_70(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_71() {
entry:
  ret i1 true
}

define i1 @apply_rule_72() {
entry:
  ret i1 false
}

attributes #0 = { noreturn }
