target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }
%map = type { i64, i8*, i8* }

@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define i1 @side_condition_208(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

declare noalias i8* @malloc(i64)

declare i32 @getTag(%block*)

declare i1 @"eval_LblisKResult{SortK{}}"(%block*)

define i1 @side_condition_210(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_211(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

declare i1 @hook_INT_ne(%mpz*, %mpz*)

define i1 @side_condition_212(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_214(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_223(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_le(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define i1 @side_condition_225(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_234(%map*, %block* %VarX) {
entry:
  %VarRho = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarX)
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %entry
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarX, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarX, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = alloca %map
  store %map %VarRho, %map* %11
  %12 = call i1 @hook_MAP_in_keys(%block* %10, %map* %11)
  %hook_BOOL_not = xor i1 %12, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_MAP_in_keys(%block*, %map*)

define i1 @side_condition_237(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ge(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_ge(%mpz*, %mpz*)

define i1 @side_condition_250(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_269(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_272(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_293(i1 %VarC) {
entry:
  ret i1 %VarC
}

define i1 @side_condition_308(i1 %VarC) {
entry:
  %hook_BOOL_not = xor i1 %VarC, true
  ret i1 %hook_BOOL_not
}

define i1 @side_condition_314(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_321(%block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarK0)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

geFirst:                                          ; preds = %entry
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarK0, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarK0, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  br i1 %10, label %then, label %hook_BOOL_and

then:                                             ; preds = %merge
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %11 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %tag4 = call i32 @getTag(%block* %VarHOLE)
  %12 = icmp uge i32 %tag4, 126
  br i1 %12, label %geFirst5, label %notInjection6

hook_BOOL_and:                                    ; preds = %merge7, %merge
  %phi11 = phi i1 [ %hook_BOOL_not, %merge7 ], [ %10, %merge ]
  ret i1 %phi11

geFirst5:                                         ; preds = %then
  %13 = icmp ule i32 %tag4, 148
  br i1 %13, label %merge7, label %notInjection6

notInjection6:                                    ; preds = %geFirst5, %then
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall8 to { %blockheader, [0 x i64], %block* }*
  %inj9 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj9
  %15 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %14, i64 0, i32 2
  store %block* %VarHOLE, %block** %15
  %16 = bitcast { %blockheader, [0 x i64], %block* }* %14 to %block*
  br label %merge7

merge7:                                           ; preds = %notInjection6, %geFirst5
  %phi10 = phi %block* [ %16, %notInjection6 ], [ %VarHOLE, %geFirst5 ]
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 2
  store %block* %phi10, %block** %17
  %18 = inttoptr i64 536870912001 to %block*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %11, i64 0, i32 3
  store %block* %18, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %11 to %block*
  %21 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %20)
  %hook_BOOL_not = xor i1 %21, true
  br label %hook_BOOL_and
}

define i1 @side_condition_325(%map*, %block* %VarX) {
entry:
  %VarRho = load %map, %map* %0
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarX)
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %entry
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarX, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarX, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %11 = alloca %map
  store %map %VarRho, %map* %11
  %12 = call i1 @hook_MAP_in_keys(%block* %10, %map* %11)
  ret i1 %12
}

define i1 @side_condition_327(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}

define i1 @side_condition_329(%block* %VarHOLE) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarHOLE)
  %1 = icmp uge i32 %tag, 126
  br i1 %1, label %geFirst, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

geFirst:                                          ; preds = %then
  %2 = icmp ule i32 %tag, 148
  br i1 %2, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %3 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %3, i64 0, i32 2
  store %block* %VarHOLE, %block** %4
  %5 = bitcast { %blockheader, [0 x i64], %block* }* %3 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %5, %notInjection ], [ %VarHOLE, %geFirst ]
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %6
  %7 = inttoptr i64 536870912001 to %block*
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %7, %block** %8
  %9 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %10 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %9)
  %hook_BOOL_not = xor i1 %10, true
  br label %hook_BOOL_and
}
