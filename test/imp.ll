target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%map = type { i64, i8*, i8* }

@token_2450474d = global { %blockheader, [4 x i8] } { %blockheader { i64 4 }, [4 x i8] c"$PGM" }
@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer
@int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_1_limbs, i32 0, i32 0) }
@int_1_limbs = global [1 x i64] [i64 1]

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
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
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

define fastcc %block* @apply_rule_205(%map*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %2 = call fastcc %block* @"eval_LblinitKCell{SortMap{}}"(%map* %0)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %2, %block** %3
  %4 = call fastcc %block* @"eval_LblinitStateCell{}"()
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %4, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  ret %block* %6
}

declare %block* @"eval_LblinitKCell{SortMap{}}"(%map*)

declare %block* @"eval_LblinitStateCell{}"()

define fastcc %block* @apply_rule_206(%map*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356052 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @token_2450474d to %block*), %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %10 = call %block* @hook_MAP_lookup(%map* %0, %block* %9)
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %12
}

declare %block* @hook_MAP_lookup(%map*, %block*)

define fastcc i1 @apply_rule_207() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_208(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355915 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

declare i32 @getTag(%block*)

define fastcc %block* @apply_rule_209(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %tag6 = call i32 @getTag(%block* %3)
  %15 = icmp ne i32 %tag6, -1
  br i1 %15, label %notString7, label %notInjection9

notString7:                                       ; preds = %merge
  %16 = icmp uge i32 %tag6, 126
  br i1 %16, label %geFirst8, label %notInjection9

geFirst8:                                         ; preds = %notString7
  %17 = icmp ule i32 %tag6, 148
  br i1 %17, label %merge10, label %notInjection9

notInjection9:                                    ; preds = %geFirst8, %notString7, %merge
  %malloccall11 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %18 = bitcast i8* %malloccall11 to { %blockheader, [0 x i64], %block* }*
  %inj12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %18, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj12
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %18, i64 0, i32 2
  store %block* %3, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %18 to %block*
  br label %merge10

merge10:                                          ; preds = %notInjection9, %geFirst8
  %phi13 = phi %block* [ %20, %notInjection9 ], [ %3, %geFirst8 ]
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %phi13, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %23, %block** %24
  %25 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %25, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %27, %block** %28
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %29
  %30 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %30
}

define fastcc %block* @apply_rule_210(%block*, %block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %8 = icmp ne i32 %tag, -1
  br i1 %8, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %9 = icmp uge i32 %tag, 126
  br i1 %9, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %10 = icmp ule i32 %tag, 148
  br i1 %10, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %2, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %13, %notInjection ], [ %2, %geFirst ]
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %phi, %block** %14
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %15, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612558 }, %blockheader* %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %17 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %17, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %18 = icmp ne i32 %tag9, -1
  br i1 %18, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %19 = icmp uge i32 %tag9, 126
  br i1 %19, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %20 = icmp ule i32 %tag9, 148
  br i1 %20, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %21 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %21, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj15
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %21, i64 0, i32 2
  store %block* %3, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block* }* %21 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %23, %notInjection12 ], [ %3, %geFirst11 ]
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %17, i64 0, i32 2
  store %block* %phi16, %block** %24
  %25 = inttoptr i64 536870912001 to %block*
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %17, i64 0, i32 3
  store %block* %25, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %17 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %27, %block** %28
  %malloccall17 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %29 = bitcast i8* %malloccall17 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %29, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq18
  %tag19 = call i32 @getTag(%block* %4)
  %30 = icmp ne i32 %tag19, -1
  br i1 %30, label %notString20, label %notInjection22

notString20:                                      ; preds = %merge13
  %31 = icmp uge i32 %tag19, 126
  br i1 %31, label %geFirst21, label %notInjection22

geFirst21:                                        ; preds = %notString20
  %32 = icmp ule i32 %tag19, 148
  br i1 %32, label %merge23, label %notInjection22

notInjection22:                                   ; preds = %geFirst21, %notString20, %merge13
  %malloccall24 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %33 = bitcast i8* %malloccall24 to { %blockheader, [0 x i64], %block* }*
  %inj25 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %33, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj25
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %33, i64 0, i32 2
  store %block* %4, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %33 to %block*
  br label %merge23

merge23:                                          ; preds = %notInjection22, %geFirst21
  %phi26 = phi %block* [ %35, %notInjection22 ], [ %4, %geFirst21 ]
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %29, i64 0, i32 2
  store %block* %phi26, %block** %36
  %37 = inttoptr i64 536870912001 to %block*
  %38 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %29, i64 0, i32 3
  store %block* %37, %block** %38
  %39 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %29 to %block*
  %40 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %39, %block** %40
  %41 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %42 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %15, i64 0, i32 2
  store %block* %41, %block** %42
  %43 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %15, i64 0, i32 3
  store %block* %1, %block** %43
  %44 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %15 to %block*
  %45 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %44, %block** %45
  %46 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %47 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %46, %block** %47
  %48 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  %49 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %48, %block** %49
  %50 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %0, %block** %50
  %51 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  ret %block* %51
}

define fastcc %mpz* @apply_rule_211(%mpz*, %mpz*) {
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

define fastcc %block* @apply_rule_212(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355914 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc i1 @apply_rule_213() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_214(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %13 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %13, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %14 = inttoptr i64 21474836481 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %13, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %13, i64 0, i32 3
  store %block* %1, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %13 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %19, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %21, %block** %22
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %24
}

define fastcc i1 @apply_rule_215() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_216() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_217() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_218(i1, i1) {
entry:
  %hook_BOOL_eq = icmp eq i1 %0, %1
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define fastcc i1 @apply_rule_219() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_220() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_221(%block*, %block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %8, i64 0, i32 0
  store %blockheader { i64 5066566760661073 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %8, i64 0, i32 2
  store %block* %2, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %8, i64 0, i32 3
  store %block* %3, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %8, i64 0, i32 4
  store %block* %4, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %8 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %13 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %13, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %13, i64 0, i32 2
  store %block* %12, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %13 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %20, %block** %21
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %0, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  ret %block* %23
}

define fastcc i1 @apply_rule_222() {
entry:
  ret i1 true
}

define fastcc %mpz* @apply_rule_223(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc i1 @apply_rule_224() {
entry:
  ret i1 true
}

define fastcc %mpz* @apply_rule_225(%mpz*, %mpz*) {
entry:
  %2 = call %mpz* @hook_INT_emod(%mpz* %0, %mpz* %1)
  %3 = call %mpz* @hook_INT_sub(%mpz* %0, %mpz* %2)
  %4 = call %mpz* @hook_INT_tdiv(%mpz* %3, %mpz* %1)
  ret %mpz* %4
}

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

define fastcc i1 @apply_rule_226(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_227(i1) {
entry:
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define fastcc i1 @apply_rule_228() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_229(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc i1 @apply_rule_230() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_231(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_232() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_233() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_234(%block*, %map*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612629 }, %blockheader* %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %3, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %0, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = inttoptr i64 536870912001 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %19, %block** %20
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %21 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %21, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %22 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %22, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq7
  %tag = call i32 @getTag(%block* %2)
  %23 = icmp ne i32 %tag, -1
  br i1 %23, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %24 = icmp uge i32 %tag, 126
  br i1 %24, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %25 = icmp ule i32 %tag, 148
  br i1 %25, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %26 = bitcast i8* %malloccall8 to { %blockheader, [0 x i64], %block* }*
  %inj9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %26, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj9
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %26, i64 0, i32 2
  store %block* %2, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %26 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %28, %notInjection ], [ %2, %geFirst ]
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %22, i64 0, i32 2
  store %block* %phi, %block** %29
  %30 = inttoptr i64 536870912001 to %block*
  %31 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %22, i64 0, i32 3
  store %block* %30, %block** %31
  %32 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %22 to %block*
  %malloccall10 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %33 = bitcast i8* %malloccall10 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %33, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq11
  %malloccall12 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %34 = bitcast i8* %malloccall12 to { %blockheader, [0 x i64], %mpz* }*
  %inj13 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %34, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj13
  %35 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %34, i64 0, i32 2
  store %mpz* @int_0, %mpz** %35
  %36 = bitcast { %blockheader, [0 x i64], %mpz* }* %34 to %block*
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %33, i64 0, i32 2
  store %block* %36, %block** %37
  %38 = inttoptr i64 536870912001 to %block*
  %39 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %33, i64 0, i32 3
  store %block* %38, %block** %39
  %40 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %33 to %block*
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %41 = bitcast i8* %malloccall14 to %map*
  call void @hook_MAP_update(%map* %41, %map* %1, %block* %32, %block* %40)
  %42 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %21, i64 0, i32 2
  %43 = load %map, %map* %41
  store %map %43, %map* %42
  %44 = bitcast { %blockheader, [0 x i64], %map }* %21 to %block*
  %45 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %44, %block** %45
  %46 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %46
}

declare void @hook_MAP_update(%map* sret, %map*, %block*, %block*)

define fastcc i1 @apply_rule_235() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_236() {
entry:
  ret i1 false
}

define fastcc %mpz* @apply_rule_237(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc i1 @apply_rule_238(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_239() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_240(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_241(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612602 }, %blockheader* %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %3, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %2, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc i1 @apply_rule_242(%block*, %block*) {
entry:
  %2 = call i1 @hook_KEQUAL_eq(%block* %0, %block* %1)
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_KEQUAL_eq(%block*, %block*)

define fastcc i1 @apply_rule_243(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_244(%block*, %block*, i1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %6, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
  %hook_BOOL_not = xor i1 %2, true
  %7 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %6, i64 0, i32 2
  store i1 %hook_BOOL_not, i1* %7
  %8 = bitcast { %blockheader, [0 x i64], i1 }* %6 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %8, %block** %9
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %16
}

define fastcc i1 @apply_rule_245() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_246() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_247() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_248() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to %map*
  call void @hook_MAP_unit(%map* %1)
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 2
  %3 = load %map, %map* %1
  store %map %3, %map* %2
  %4 = bitcast { %blockheader, [0 x i64], %map }* %0 to %block*
  ret %block* %4
}

declare void @hook_MAP_unit(%map* sret)

define fastcc i1 @apply_rule_249() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_250(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355917 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc %mpz* @apply_rule_251(%mpz*, %mpz*, %mpz*) {
entry:
  %3 = call %mpz* @hook_INT_shr(%mpz* %0, %mpz* %1)
  %4 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %2)
  %5 = call %mpz* @hook_INT_emod(%mpz* %3, %mpz* %4)
  ret %mpz* %5
}

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

define fastcc i1 @apply_rule_252() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_253(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612592 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %2, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %3, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc i1 @apply_rule_254() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_255() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_256() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_257(%mpz*, %mpz*) {
entry:
  %2 = call i1 @hook_INT_eq(%mpz* %0, %mpz* %1)
  %hook_BOOL_not = xor i1 %2, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_INT_eq(%mpz*, %mpz*)

define fastcc i1 @apply_rule_258(i1, i1) {
entry:
  %hook_BOOL_eq = icmp eq i1 %0, %1
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define fastcc i1 @apply_rule_259() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_260() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_261() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_262(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_263() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_264() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_265() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_266() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_267(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612612 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %3, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %2, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc i1 @apply_rule_268(%mpz*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 2
  store %mpz* %0, %mpz** %4
  %5 = bitcast { %blockheader, [0 x i64], %mpz* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %mpz* }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %11, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj5
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %11, i64 0, i32 2
  store %mpz* %1, %mpz** %12
  %13 = bitcast { %blockheader, [0 x i64], %mpz* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = inttoptr i64 536870912001 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %18 = call i1 @hook_KEQUAL_eq(%block* %9, %block* %17)
  ret i1 %18
}

define fastcc %block* @apply_rule_269(%block*, %block*, %mpz*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %7, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
  %8 = call %mpz* @hook_INT_tdiv(%mpz* %2, %mpz* %3)
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %7, i64 0, i32 2
  store %mpz* %8, %mpz** %9
  %10 = bitcast { %blockheader, [0 x i64], %mpz* }* %7 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %18
}

define fastcc i1 @apply_rule_270() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_271(%block*, %block*, %mpz*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %7, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
  %8 = call %mpz* @hook_INT_add(%mpz* %2, %mpz* %3)
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %7, i64 0, i32 2
  store %mpz* %8, %mpz** %9
  %10 = bitcast { %blockheader, [0 x i64], %mpz* }* %7 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %18
}

define fastcc %block* @apply_rule_272(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355916 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc i1 @apply_rule_273() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_274(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc i1 @apply_rule_275() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_276() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_277(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 5066566760661073 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %7, i64 0, i32 2
  store %block* %0, %block** %8
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543355921 }, %blockheader* %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt"
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612614 }, %blockheader* %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt"
  %tag = call i32 @getTag(%block* %3)
  %11 = icmp ne i32 %tag, -1
  br i1 %11, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %12 = icmp uge i32 %tag, 126
  br i1 %12, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %13 = icmp ule i32 %tag, 148
  br i1 %13, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 2
  store %block* %3, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %14 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %16, %notInjection ], [ %3, %geFirst ]
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %phi, %block** %17
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %18 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %18, i64 0, i32 0
  store %blockheader { i64 281487861612667 }, %blockheader* %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block"
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %18, i64 0, i32 2
  store %block* %0, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %18, i64 0, i32 3
  store %block* %3, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %18 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %21, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %23, %block** %24
  %25 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %7, i64 0, i32 3
  store %block* %25, %block** %26
  %27 = inttoptr i64 68719476737 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %7, i64 0, i32 4
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %7 to %block*
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %30 = bitcast i8* %malloccall8 to { %blockheader, [0 x i64], %block* }*
  %inj9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %30, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj9
  %31 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %30, i64 0, i32 2
  store %block* %29, %block** %31
  %32 = bitcast { %blockheader, [0 x i64], %block* }* %30 to %block*
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %32, %block** %33
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %2, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %38 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %37, %block** %38
  %39 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %1, %block** %39
  %40 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %40
}

define fastcc i1 @apply_rule_278() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_279(%block*, %mpz*, %map*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %7, %block** %8
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %9, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %3)
  %11 = icmp ne i32 %tag, -1
  br i1 %11, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %12 = icmp uge i32 %tag, 126
  br i1 %12, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %13 = icmp ule i32 %tag, 148
  br i1 %13, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 2
  store %block* %3, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %14 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %16, %notInjection ], [ %3, %geFirst ]
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %phi, %block** %17
  %18 = inttoptr i64 536870912001 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %21 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %21, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq6
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %22 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %mpz* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %22, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj8
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %22, i64 0, i32 2
  store %mpz* %1, %mpz** %23
  %24 = bitcast { %blockheader, [0 x i64], %mpz* }* %22 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %21, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = inttoptr i64 536870912001 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %21, i64 0, i32 3
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %21 to %block*
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %29 = bitcast i8* %malloccall9 to %map*
  call void @hook_MAP_update(%map* %29, %map* %2, %block* %20, %block* %28)
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %9, i64 0, i32 2
  %31 = load %map, %map* %29
  store %map %31, %map* %30
  %32 = bitcast { %blockheader, [0 x i64], %map }* %9 to %block*
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %32, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %34
}

define fastcc i1 @apply_rule_280() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_281() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_282(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_283(i1, i1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 2
  store i1 %0, i1* %4
  %5 = bitcast { %blockheader, [0 x i64], i1 }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], i1 }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %11, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj5
  %12 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %11, i64 0, i32 2
  store i1 %1, i1* %12
  %13 = bitcast { %blockheader, [0 x i64], i1 }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = inttoptr i64 536870912001 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %15, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %18 = call i1 @hook_KEQUAL_eq(%block* %9, %block* %17)
  ret i1 %18
}

define fastcc i1 @apply_rule_284() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_285(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543355908 }, %blockheader* %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp"
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %2, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %8, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc i1 @apply_rule_286() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_287(%block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %5, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %5, i64 0, i32 2
  store i1 false, i1* %6
  %7 = bitcast { %blockheader, [0 x i64], i1 }* %5 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %7, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %1, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  ret %block* %15
}

define fastcc i1 @apply_rule_288() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_289(%mpz*, %mpz*) {
entry:
  %2 = call %mpz* @hook_INT_tmod(%mpz* %1, %mpz* %0)
  %3 = call i1 @hook_INT_eq(%mpz* %2, %mpz* @int_0)
  ret i1 %3
}

define fastcc i1 @apply_rule_290() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_291() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_292(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_293(%block*) {
entry:
  ret %block* %0
}

define fastcc %block* @apply_rule_294(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc %mpz* @apply_rule_295(%mpz*, %mpz*, %mpz*) {
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

define fastcc i1 @apply_rule_296() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_297() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_298(%block*, %block*, %mpz*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %7, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
  %8 = call i1 @hook_INT_le(%mpz* %2, %mpz* %3)
  %9 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %7, i64 0, i32 2
  store i1 %8, i1* %9
  %10 = bitcast { %blockheader, [0 x i64], i1 }* %7 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %15, %block** %16
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %18
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define fastcc i1 @apply_rule_299() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_300(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_301() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_302() {
entry:
  ret i1 false
}

define fastcc %block* @apply_rule_303(%block*, %block*, %mpz*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %6, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
  %7 = call %mpz* @hook_INT_sub(%mpz* @int_0, %mpz* %2)
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %6, i64 0, i32 2
  store %mpz* %7, %mpz** %8
  %9 = bitcast { %blockheader, [0 x i64], %mpz* }* %6 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %9, %block** %10
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %17
}

define fastcc i1 @apply_rule_304() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_305() {
entry:
  ret i1 false
}

define fastcc %mpz* @apply_rule_306(%mpz*) {
entry:
  ret %mpz* %0
}

define fastcc %block* @apply_rule_307(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612594 }, %blockheader* %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %2, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %3, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc %block* @apply_rule_308(%block*) {
entry:
  ret %block* %0
}

define fastcc i1 @apply_rule_309() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_310() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_311() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_312(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612610 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %3, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %2, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc %block* @apply_rule_313(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612612 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %2, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %3, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc %block* @apply_rule_314(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355911 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc i1 @apply_rule_315() {
entry:
  ret i1 true
}

define fastcc i1 @apply_rule_316() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_317(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612592 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %3, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %2, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc i1 @apply_rule_318() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_319(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 0
  store %blockheader { i64 281487861612610 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 2
  store %block* %2, %block** %8
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %7, i64 0, i32 3
  store %block* %3, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %7 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %10, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %13, %block** %14
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %1, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %21
}

define fastcc i1 @apply_rule_320() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_321(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355910 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc i1 @apply_rule_322() {
entry:
  ret i1 false
}

define fastcc i1 @apply_rule_323(i1) {
entry:
  ret i1 %0
}

define fastcc i1 @apply_rule_324() {
entry:
  ret i1 true
}

define fastcc %block* @apply_rule_325(%block*, %map*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = inttoptr i64 536870912001 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %16 = call %block* @hook_MAP_lookup(%map* %1, %block* %15)
  %17 = call fastcc %block* @"eval_append{SortK{}, SortK{}}"(%block* %16, %block* %0)
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %19, %block** %20
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %21 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %21, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %21, i64 0, i32 2
  %23 = load %map, %map* %1
  store %map %23, %map* %22
  %24 = bitcast { %blockheader, [0 x i64], %map }* %21 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %26
}

define fastcc i1 @apply_rule_326(i1) {
entry:
  ret i1 %0
}

define fastcc %block* @apply_rule_327(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355912 }, %blockheader* %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc %block* @apply_rule_328(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %0, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %0, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %2, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %1, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc %block* @apply_rule_329(%block*, %block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %2, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %2, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %15 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 0
  store %blockheader { i64 562958543355913 }, %blockheader* %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %3)
  %17 = icmp ne i32 %tag9, -1
  br i1 %17, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %18 = icmp uge i32 %tag9, 126
  br i1 %18, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %19 = icmp ule i32 %tag9, 148
  br i1 %19, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj15
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %20, i64 0, i32 2
  store %block* %3, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block* }* %20 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %22, %notInjection12 ], [ %3, %geFirst11 ]
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 2
  store %block* %phi16, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %16, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %16 to %block*
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %15, i64 0, i32 2
  store %block* %26, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %15 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %36 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %35, %block** %36
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %0, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  ret %block* %38
}

define fastcc %block* @apply_rule_330(%block*, %block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %2)
  %6 = icmp ne i32 %tag, -1
  br i1 %6, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %7 = icmp uge i32 %tag, 126
  br i1 %7, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %8 = icmp ule i32 %tag, 148
  br i1 %8, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %9, i64 0, i32 2
  store %block* %2, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block* }* %9 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %11, %notInjection ], [ %2, %geFirst ]
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 2
  store %block* %phi, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %5, i64 0, i32 3
  store %block* %1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %5 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %0, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  ret %block* %19
}

define fastcc %block* @apply_rule_331(%block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %1, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %5, %block** %6
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %0, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  ret %block* %8
}

define fastcc i1 @apply_rule_332() {
entry:
  ret i1 true
}
