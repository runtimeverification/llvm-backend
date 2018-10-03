target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%map = type { i64, i8*, i8* }

@"token_$PGM" = global { %blockheader, [4 x i8] } { %blockheader { i64 4 }, [4 x i8] c"$PGM" }
@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer
@int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @int_1_limbs, i32 0, i32 0) }
@int_1_limbs = global [1 x i64] [i64 1]

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define %block* @apply_rule_0(%block* %K2) {
entry:
  ret %block* %K2
}

define %block* @apply_rule_1(%block* %K1, %block* %K2, %block* %K3) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %K1, %block** %1
  %2 = call %block* @"eval_append{SortK{}, SortK{}}"(%block* %K2, %block* %K3)
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %2, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %4
}

declare noalias i8* @malloc(i64)

declare %block* @"eval_append{SortK{}, SortK{}}"(%block*, %block*)

define %block* @apply_rule_205(%map*) {
entry:
  %VarInit = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %2 = alloca %map
  store %map %VarInit, %map* %2
  %3 = call %block* @"eval_LblinitKCell{SortMap{}}"(%map* %2)
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = call %block* @"eval_LblinitStateCell{}"()
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  ret %block* %7
}

declare %block* @"eval_LblinitKCell{SortMap{}}"(%map*)

declare %block* @"eval_LblinitStateCell{}"()

define %block* @apply_rule_206(%map*) {
entry:
  %VarInit = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %2 = alloca %map
  store %map %VarInit, %map* %2
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356052 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* bitcast ({ %blockheader, [4 x i8] }* @"token_$PGM" to %block*), %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %6, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %11 = call %block* @hook_MAP_lookup(%map* %2, %block* %10)
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  ret %block* %13
}

declare %block* @hook_MAP_lookup(%map*, %block*)

define i1 @apply_rule_207() {
entry:
  ret i1 true
}

define %block* @apply_rule_208(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355915 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK1)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK1, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

declare i32 @getTag(%block*)

define %block* @apply_rule_209(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS1, %block* %VarS2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarS1)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarS1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarS1, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %tag6 = call i32 @getTag(%block* %VarS2)
  %11 = icmp ne i32 %tag6, -1
  br i1 %11, label %notString7, label %notInjection9

notString7:                                       ; preds = %merge
  %12 = icmp uge i32 %tag6, 126
  br i1 %12, label %geFirst8, label %notInjection9

geFirst8:                                         ; preds = %notString7
  %13 = icmp ule i32 %tag6, 148
  br i1 %13, label %merge10, label %notInjection9

notInjection9:                                    ; preds = %geFirst8, %notString7, %merge
  %malloccall11 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall11 to { %blockheader, [0 x i64], %block* }*
  %inj12 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj12
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 2
  store %block* %VarS2, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %14 to %block*
  br label %merge10

merge10:                                          ; preds = %notInjection9, %geFirst8
  %phi13 = phi %block* [ %16, %notInjection9 ], [ %VarS2, %geFirst8 ]
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %phi13, %block** %17
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %19, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %21, %block** %22
  %23 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %23, %block** %24
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %26
}

define %block* @apply_rule_210(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1, %block* %VarK2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 0
  store %blockheader { i64 281487861612558 }, %blockheader* %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK1)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK1, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %malloccall17 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %24 = bitcast i8* %malloccall17 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %24, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq18
  %tag19 = call i32 @getTag(%block* %VarK2)
  %25 = icmp ne i32 %tag19, -1
  br i1 %25, label %notString20, label %notInjection22

notString20:                                      ; preds = %merge13
  %26 = icmp uge i32 %tag19, 126
  br i1 %26, label %geFirst21, label %notInjection22

geFirst21:                                        ; preds = %notString20
  %27 = icmp ule i32 %tag19, 148
  br i1 %27, label %merge23, label %notInjection22

notInjection22:                                   ; preds = %geFirst21, %notString20, %merge13
  %malloccall24 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %28 = bitcast i8* %malloccall24 to { %blockheader, [0 x i64], %block* }*
  %inj25 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %28, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj25
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %28, i64 0, i32 2
  store %block* %VarK2, %block** %29
  %30 = bitcast { %blockheader, [0 x i64], %block* }* %28 to %block*
  br label %merge23

merge23:                                          ; preds = %notInjection22, %geFirst21
  %phi26 = phi %block* [ %30, %notInjection22 ], [ %VarK2, %geFirst21 ]
  %31 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %24, i64 0, i32 2
  store %block* %phi26, %block** %31
  %32 = inttoptr i64 536870912001 to %block*
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %24, i64 0, i32 3
  store %block* %32, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %24 to %block*
  %35 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 3
  store %block* %34, %block** %35
  %36 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %11 to %block*
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %36, %block** %37
  %38 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %38
  %39 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %40 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %39, %block** %40
  %41 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %42 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %41, %block** %42
  %43 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %44 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %43, %block** %44
  %45 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %45
  %46 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %46
}

define %mpz* @apply_rule_211(%mpz* %VarI1, %mpz* %VarI2) {
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

define %block* @apply_rule_212(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355914 }, %blockheader* %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK0)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK0, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define i1 @apply_rule_213() {
entry:
  ret i1 false
}

define %block* @apply_rule_214(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %11 = inttoptr i64 21474836481 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %18, %block** %19
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %21
}

define i1 @apply_rule_215() {
entry:
  ret i1 true
}

define i1 @apply_rule_216() {
entry:
  ret i1 true
}

define i1 @apply_rule_217() {
entry:
  ret i1 true
}

define i1 @apply_rule_218(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_219() {
entry:
  ret i1 true
}

define i1 @apply_rule_220() {
entry:
  ret i1 false
}

define %block* @apply_rule_221(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1, %block* %VarK2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 5066566760661073 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarK1, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 4
  store %block* %VarK2, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %7, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
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

define i1 @apply_rule_222() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_223(%mpz* %VarI1) {
entry:
  ret %mpz* %VarI1
}

define i1 @apply_rule_224() {
entry:
  ret i1 true
}

define %mpz* @apply_rule_225(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_emod(%mpz* %VarI1, %mpz* %VarI2)
  %1 = call %mpz* @hook_INT_sub(%mpz* %VarI1, %mpz* %0)
  %2 = call %mpz* @hook_INT_tdiv(%mpz* %1, %mpz* %VarI2)
  ret %mpz* %2
}

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

define i1 @apply_rule_226(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_227(i1 %VarB) {
entry:
  %hook_BOOL_not = xor i1 %VarB, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_228() {
entry:
  ret i1 false
}

define %block* @apply_rule_229(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarS)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarS, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarS, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
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

define i1 @apply_rule_230() {
entry:
  ret i1 false
}

define i1 @apply_rule_231(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_232() {
entry:
  ret i1 false
}

define i1 @apply_rule_233() {
entry:
  ret i1 false
}

define %block* @apply_rule_234(%block* %"Var'Unds'15", %map*, %block* %VarX, %block* %VarXs) {
entry:
  %VarRho = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612629 }, %blockheader* %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt"
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %VarXs, %block** %5
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %"Var'Unds'15", %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356031 }, %blockheader* %inj
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %7, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %10, %block** %11
  %12 = inttoptr i64 536870912001 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %14, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %16, %block** %17
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %18 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %18, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %19 = alloca %map
  store %map %VarRho, %map* %19
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %20, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq7
  %tag = call i32 @getTag(%block* %VarX)
  %21 = icmp ne i32 %tag, -1
  br i1 %21, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %22 = icmp uge i32 %tag, 126
  br i1 %22, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %23 = icmp ule i32 %tag, 148
  br i1 %23, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %24 = bitcast i8* %malloccall8 to { %blockheader, [0 x i64], %block* }*
  %inj9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %24, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj9
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %24, i64 0, i32 2
  store %block* %VarX, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block* }* %24 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %26, %notInjection ], [ %VarX, %geFirst ]
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %20, i64 0, i32 2
  store %block* %phi, %block** %27
  %28 = inttoptr i64 536870912001 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %20, i64 0, i32 3
  store %block* %28, %block** %29
  %30 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %20 to %block*
  %malloccall10 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %31 = bitcast i8* %malloccall10 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %31, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq11
  %malloccall12 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %32 = bitcast i8* %malloccall12 to { %blockheader, [0 x i64], %mpz* }*
  %inj13 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %32, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj13
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %32, i64 0, i32 2
  store %mpz* @int_0, %mpz** %33
  %34 = bitcast { %blockheader, [0 x i64], %mpz* }* %32 to %block*
  %35 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %31, i64 0, i32 2
  store %block* %34, %block** %35
  %36 = inttoptr i64 536870912001 to %block*
  %37 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %31, i64 0, i32 3
  store %block* %36, %block** %37
  %38 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %31 to %block*
  %39 = alloca %map
  call void @hook_MAP_update(%map* %39, %map* %19, %block* %30, %block* %38)
  %40 = load %map, %map* %39
  %41 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %18, i64 0, i32 2
  store %map %40, %map* %41
  %42 = bitcast { %blockheader, [0 x i64], %map }* %18 to %block*
  %43 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %42, %block** %43
  %44 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  ret %block* %44
}

declare void @hook_MAP_update(%map* sret, %map*, %block*, %block*)

define i1 @apply_rule_235() {
entry:
  ret i1 true
}

define i1 @apply_rule_236() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_237(%mpz* %VarI2) {
entry:
  ret %mpz* %VarI2
}

define i1 @apply_rule_238(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_239() {
entry:
  ret i1 false
}

define i1 @apply_rule_240(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_241(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612602 }, %blockheader* %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarK0, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define i1 @apply_rule_242(%block* %VarK1, %block* %VarK2) {
entry:
  %0 = call i1 @hook_KEQUAL_eq(%block* %VarK1, %block* %VarK2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_KEQUAL_eq(%block*, %block*)

define i1 @apply_rule_243(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_244(%block* %VarDotVar0, %block* %VarDotVar1, i1 %VarT) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
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

define i1 @apply_rule_245() {
entry:
  ret i1 false
}

define i1 @apply_rule_246() {
entry:
  ret i1 true
}

define i1 @apply_rule_247() {
entry:
  ret i1 false
}

define %block* @apply_rule_248() {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %1 = alloca %map
  call void @hook_MAP_unit(%map* %1)
  %2 = load %map, %map* %1
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %0, i64 0, i32 2
  store %map %2, %map* %3
  %4 = bitcast { %blockheader, [0 x i64], %map }* %0 to %block*
  ret %block* %4
}

declare void @hook_MAP_unit(%map* sret)

define i1 @apply_rule_249() {
entry:
  ret i1 true
}

define %block* @apply_rule_250(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355917 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK1)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK1, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define %mpz* @apply_rule_251(%mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN) {
entry:
  %0 = call %mpz* @hook_INT_shr(%mpz* %VarI, %mpz* %VarIDX)
  %1 = call %mpz* @hook_INT_shl(%mpz* @int_1, %mpz* %VarLEN)
  %2 = call %mpz* @hook_INT_emod(%mpz* %0, %mpz* %1)
  ret %mpz* %2
}

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

define i1 @apply_rule_252() {
entry:
  ret i1 false
}

define %block* @apply_rule_253(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612592 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarK1, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define i1 @apply_rule_254() {
entry:
  ret i1 false
}

define i1 @apply_rule_255() {
entry:
  ret i1 true
}

define i1 @apply_rule_256() {
entry:
  ret i1 false
}

define i1 @apply_rule_257(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_eq(%mpz* %VarI1, %mpz* %VarI2)
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_INT_eq(%mpz*, %mpz*)

define i1 @apply_rule_258(i1 %VarB1, i1 %VarB2) {
entry:
  %hook_BOOL_eq = icmp eq i1 %VarB1, %VarB2
  %hook_BOOL_not = xor i1 %hook_BOOL_eq, true
  ret i1 %hook_BOOL_not
}

define i1 @apply_rule_259() {
entry:
  ret i1 false
}

define i1 @apply_rule_260() {
entry:
  ret i1 true
}

define i1 @apply_rule_261() {
entry:
  ret i1 false
}

define i1 @apply_rule_262(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_263() {
entry:
  ret i1 true
}

define i1 @apply_rule_264() {
entry:
  ret i1 false
}

define i1 @apply_rule_265() {
entry:
  ret i1 true
}

define i1 @apply_rule_266() {
entry:
  ret i1 true
}

define %block* @apply_rule_267(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612612 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarK0, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define i1 @apply_rule_268(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %1, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %1, i64 0, i32 2
  store %mpz* %VarI1, %mpz** %2
  %3 = bitcast { %blockheader, [0 x i64], %mpz* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = inttoptr i64 536870912001 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %mpz* }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %9, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj5
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %9, i64 0, i32 2
  store %mpz* %VarI2, %mpz** %10
  %11 = bitcast { %blockheader, [0 x i64], %mpz* }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = inttoptr i64 536870912001 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 3
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %8 to %block*
  %16 = call i1 @hook_KEQUAL_eq(%block* %7, %block* %15)
  ret i1 %16
}

define %block* @apply_rule_269(%block* %VarDotVar0, %block* %VarDotVar1, %mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
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

define i1 @apply_rule_270() {
entry:
  ret i1 false
}

define %block* @apply_rule_271(%block* %VarDotVar0, %block* %VarDotVar1, %mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
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

define %block* @apply_rule_272(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355916 }, %blockheader* %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK0)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK0, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define i1 @apply_rule_273() {
entry:
  ret i1 false
}

define %block* @apply_rule_274(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarS)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarS, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarS, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
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

define i1 @apply_rule_275() {
entry:
  ret i1 true
}

define i1 @apply_rule_276() {
entry:
  ret i1 false
}

define %block* @apply_rule_277(%block* %VarB, %block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 5066566760661073 }, %blockheader* %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarB, %block** %4
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543355921 }, %blockheader* %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt"
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 0
  store %blockheader { i64 281487861612614 }, %blockheader* %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt"
  %tag = call i32 @getTag(%block* %VarS)
  %7 = icmp ne i32 %tag, -1
  br i1 %7, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %8 = icmp uge i32 %tag, 126
  br i1 %8, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %9 = icmp ule i32 %tag, 148
  br i1 %9, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %10, i64 0, i32 2
  store %block* %VarS, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block* }* %10 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %12, %notInjection ], [ %VarS, %geFirst ]
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 2
  store %block* %phi, %block** %13
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612667 }, %blockheader* %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block"
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %VarB, %block** %15
  %16 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %VarS, %block** %16
  %17 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %18 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %6, i64 0, i32 3
  store %block* %17, %block** %18
  %19 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %6 to %block*
  %20 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %19, %block** %20
  %21 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  %22 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 3
  store %block* %21, %block** %22
  %23 = inttoptr i64 68719476737 to %block*
  %24 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %3, i64 0, i32 4
  store %block* %23, %block** %24
  %25 = bitcast { %blockheader, [0 x i64], %block*, %block*, %block* }* %3 to %block*
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %26 = bitcast i8* %malloccall8 to { %blockheader, [0 x i64], %block* }*
  %inj9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %26, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj9
  %27 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %26, i64 0, i32 2
  store %block* %25, %block** %27
  %28 = bitcast { %blockheader, [0 x i64], %block* }* %26 to %block*
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %28, %block** %29
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %VarDotVar1, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %34 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %33, %block** %34
  %35 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %35
  %36 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %36
}

define i1 @apply_rule_278() {
entry:
  ret i1 true
}

define %block* @apply_rule_279(%block* %VarDotVar1, %mpz* %VarI, %map*, %block* %VarX) {
entry:
  %VarRho = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %3 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %VarDotVar1, %block** %3
  %4 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %4, %block** %5
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %6, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %7 = alloca %map
  store %map %VarRho, %map* %7
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarX)
  %9 = icmp ne i32 %tag, -1
  br i1 %9, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %10 = icmp uge i32 %tag, 126
  br i1 %10, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %11 = icmp ule i32 %tag, 148
  br i1 %11, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %12, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %12, i64 0, i32 2
  store %block* %VarX, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block* }* %12 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %14, %notInjection ], [ %VarX, %geFirst ]
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 2
  store %block* %phi, %block** %15
  %16 = inttoptr i64 536870912001 to %block*
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 3
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %8 to %block*
  %malloccall5 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %19 = bitcast i8* %malloccall5 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %19, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq6
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %mpz* }*
  %inj8 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %20, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj8
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %20, i64 0, i32 2
  store %mpz* %VarI, %mpz** %21
  %22 = bitcast { %blockheader, [0 x i64], %mpz* }* %20 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %19, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = inttoptr i64 536870912001 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %19, i64 0, i32 3
  store %block* %24, %block** %25
  %26 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %19 to %block*
  %27 = alloca %map
  call void @hook_MAP_update(%map* %27, %map* %7, %block* %18, %block* %26)
  %28 = load %map, %map* %27
  %29 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %6, i64 0, i32 2
  store %map %28, %map* %29
  %30 = bitcast { %blockheader, [0 x i64], %map }* %6 to %block*
  %31 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %30, %block** %31
  %32 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  ret %block* %32
}

define i1 @apply_rule_280() {
entry:
  ret i1 false
}

define i1 @apply_rule_281() {
entry:
  ret i1 true
}

define i1 @apply_rule_282(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_283(i1 %VarK1, i1 %VarK2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %1, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %1, i64 0, i32 2
  store i1 %VarK1, i1* %2
  %3 = bitcast { %blockheader, [0 x i64], i1 }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = inttoptr i64 536870912001 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %9 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], i1 }*
  %inj5 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %9, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj5
  %10 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %9, i64 0, i32 2
  store i1 %VarK2, i1* %10
  %11 = bitcast { %blockheader, [0 x i64], i1 }* %9 to %block*
  %12 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 2
  store %block* %11, %block** %12
  %13 = inttoptr i64 536870912001 to %block*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 3
  store %block* %13, %block** %14
  %15 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %8 to %block*
  %16 = call i1 @hook_KEQUAL_eq(%block* %7, %block* %15)
  ret i1 %16
}

define i1 @apply_rule_284() {
entry:
  ret i1 true
}

define %block* @apply_rule_285(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543355908 }, %blockheader* %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %5, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
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

define i1 @apply_rule_286() {
entry:
  ret i1 true
}

define %block* @apply_rule_287(%block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
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

define i1 @apply_rule_288() {
entry:
  ret i1 false
}

define i1 @apply_rule_289(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call %mpz* @hook_INT_tmod(%mpz* %VarI2, %mpz* %VarI1)
  %1 = call i1 @hook_INT_eq(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

define i1 @apply_rule_290() {
entry:
  ret i1 false
}

define i1 @apply_rule_291() {
entry:
  ret i1 true
}

define i1 @apply_rule_292(i1 %VarK) {
entry:
  ret i1 %VarK
}

define %block* @apply_rule_293(%block* %VarB1) {
entry:
  ret %block* %VarB1
}

define %block* @apply_rule_294(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarS)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356050 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarS, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarS, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
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

define %mpz* @apply_rule_295(%mpz* %VarI, %mpz* %VarIDX, %mpz* %VarLEN) {
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

define i1 @apply_rule_296() {
entry:
  ret i1 false
}

define i1 @apply_rule_297() {
entry:
  ret i1 false
}

define %block* @apply_rule_298(%block* %VarDotVar0, %block* %VarDotVar1, %mpz* %VarI1, %mpz* %VarI2) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], i1 }* getelementptr ({ %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], i1 }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %3, i64 0, i32 0
  store %blockheader { i64 5348033147437194 }, %blockheader* %inj
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

define i1 @apply_rule_299() {
entry:
  ret i1 false
}

define i1 @apply_rule_300(i1 %VarK) {
entry:
  ret i1 %VarK
}

define i1 @apply_rule_301() {
entry:
  ret i1 true
}

define i1 @apply_rule_302() {
entry:
  ret i1 false
}

define %block* @apply_rule_303(%block* %VarDotVar0, %block* %VarDotVar1, %mpz* %VarI1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %mpz* }* getelementptr ({ %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %mpz* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %3, i64 0, i32 0
  store %blockheader { i64 1688858450198654 }, %blockheader* %inj
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

define i1 @apply_rule_304() {
entry:
  ret i1 true
}

define i1 @apply_rule_305() {
entry:
  ret i1 false
}

define %mpz* @apply_rule_306(%mpz* %VarI) {
entry:
  ret %mpz* %VarI
}

define %block* @apply_rule_307(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612594 }, %blockheader* %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarK1, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define %block* @apply_rule_308(%block* %VarB2) {
entry:
  ret %block* %VarB2
}

define i1 @apply_rule_309() {
entry:
  ret i1 true
}

define i1 @apply_rule_310() {
entry:
  ret i1 false
}

define i1 @apply_rule_311() {
entry:
  ret i1 true
}

define %block* @apply_rule_312(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612610 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarK0, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define %block* @apply_rule_313(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612612 }, %blockheader* %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarK1, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define %block* @apply_rule_314(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355911 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK1)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK1, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define i1 @apply_rule_315() {
entry:
  ret i1 true
}

define i1 @apply_rule_316() {
entry:
  ret i1 true
}

define %block* @apply_rule_317(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612592 }, %blockheader* %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarK0, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define i1 @apply_rule_318() {
entry:
  ret i1 true
}

define %block* @apply_rule_319(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 0
  store %blockheader { i64 281487861612610 }, %blockheader* %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp"
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %3, i64 0, i32 3
  store %block* %VarK1, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %3 to %block*
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %7 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %7, i64 0, i32 2
  store %block* %6, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block* }* %7 to %block*
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

define i1 @apply_rule_320() {
entry:
  ret i1 true
}

define %block* @apply_rule_321(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355910 }, %blockheader* %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK0)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK0, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define i1 @apply_rule_322() {
entry:
  ret i1 false
}

define i1 @apply_rule_323(i1 %VarB) {
entry:
  ret i1 %VarB
}

define i1 @apply_rule_324() {
entry:
  ret i1 true
}

define %block* @apply_rule_325(%block* %VarDotVar1, %map*, %block* %VarX) {
entry:
  %VarRho = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %3 = alloca %map
  store %map %VarRho, %map* %3
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarX)
  %5 = icmp ne i32 %tag, -1
  br i1 %5, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %6 = icmp uge i32 %tag, 126
  br i1 %6, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %7 = icmp ule i32 %tag, 148
  br i1 %7, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %8 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %8, i64 0, i32 2
  store %block* %VarX, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block* }* %8 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %10, %notInjection ], [ %VarX, %geFirst ]
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 2
  store %block* %phi, %block** %11
  %12 = inttoptr i64 536870912001 to %block*
  %13 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %4, i64 0, i32 3
  store %block* %12, %block** %13
  %14 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %4 to %block*
  %15 = call %block* @hook_MAP_lookup(%map* %3, %block* %14)
  %16 = call %block* @"eval_append{SortK{}, SortK{}}"(%block* %15, %block* %VarDotVar1)
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %2, i64 0, i32 2
  store %block* %16, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %2 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %18, %block** %19
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %map }* getelementptr ({ %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* null, i32 1) to i64))
  %20 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %map }*
  %"Lbl'-LT-'state'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %20, i64 0, i32 0
  store %blockheader { i64 844442110001155 }, %blockheader* %"Lbl'-LT-'state'-GT-'"
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %20, i64 0, i32 2
  store %map %VarRho, %map* %21
  %22 = bitcast { %blockheader, [0 x i64], %map }* %20 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  ret %block* %24
}

define i1 @apply_rule_326(i1 %VarB) {
entry:
  ret i1 %VarB
}

define %block* @apply_rule_327(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355912 }, %blockheader* %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK1)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK1, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK1, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define %block* @apply_rule_328(%block* %VarB, %block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarB)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarB, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarB, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
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

define %block* @apply_rule_329(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarHOLE, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarHOLE, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall4 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq5 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq5
  %malloccall6 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall6 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 0
  store %blockheader { i64 562958543355913 }, %blockheader* %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'"
  %malloccall7 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall7 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq8
  %tag9 = call i32 @getTag(%block* %VarK0)
  %13 = icmp ne i32 %tag9, -1
  br i1 %13, label %notString10, label %notInjection12

notString10:                                      ; preds = %merge
  %14 = icmp uge i32 %tag9, 126
  br i1 %14, label %geFirst11, label %notInjection12

geFirst11:                                        ; preds = %notString10
  %15 = icmp ule i32 %tag9, 148
  br i1 %15, label %merge13, label %notInjection12

notInjection12:                                   ; preds = %geFirst11, %notString10, %merge
  %malloccall14 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall14 to { %blockheader, [0 x i64], %block* }*
  %inj15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj15
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarK0, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge13

merge13:                                          ; preds = %notInjection12, %geFirst11
  %phi16 = phi %block* [ %18, %notInjection12 ], [ %VarK0, %geFirst11 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi16, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %11, i64 0, i32 2
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block* }* %11 to %block*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 2
  store %block* %24, %block** %25
  %26 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %10, i64 0, i32 3
  store %block* %VarDotVar1, %block** %26
  %27 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %10 to %block*
  %28 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %27, %block** %28
  %29 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %29, %block** %30
  %31 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %32 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %31, %block** %32
  %33 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %VarDotVar0, %block** %33
  %34 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  ret %block* %34
}

define %block* @apply_rule_330(%block* %VarDotVar0, %block* %VarDotVar1, %block* %VarS) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarS)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall3 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall3 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356045 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %VarS, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %VarS, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
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

define %block* @apply_rule_331(%block* %VarDotVar0, %block* %VarDotVar1) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %"Lbl'-LT-'T'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612544 }, %blockheader* %"Lbl'-LT-'T'-GT-'"
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %"Lbl'-LT-'k'-GT-'" = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355906 }, %blockheader* %"Lbl'-LT-'k'-GT-'"
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

define i1 @apply_rule_332() {
entry:
  ret i1 true
}
