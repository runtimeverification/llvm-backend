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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %VarX, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %VarX, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = alloca %map
  store %map %VarRho, %map* %12
  %13 = call i1 @hook_MAP_in_keys(%block* %11, %map* %12)
  %hook_BOOL_not = xor i1 %13, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
  br label %hook_BOOL_and
}

define i1 @side_condition_321(%block* %VarHOLE, %block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %VarK0)
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarK0, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarK0, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  br i1 %11, label %then, label %hook_BOOL_and

then:                                             ; preds = %merge
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %12 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %tag4 = call i32 @getTag(%block* %VarHOLE)
  %13 = icmp ne i32 %tag4, -1
  br i1 %13, label %notString5, label %notInjection7

hook_BOOL_and:                                    ; preds = %merge8, %merge
  %phi12 = phi i1 [ %hook_BOOL_not, %merge8 ], [ %11, %merge ]
  ret i1 %phi12

notString5:                                       ; preds = %then
  %14 = icmp uge i32 %tag4, 126
  br i1 %14, label %geFirst6, label %notInjection7

geFirst6:                                         ; preds = %notString5
  %15 = icmp ule i32 %tag4, 148
  br i1 %15, label %merge8, label %notInjection7

notInjection7:                                    ; preds = %geFirst6, %notString5, %then
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %16 = bitcast i8* %malloccall9 to { %blockheader, [0 x i64], %block* }*
  %inj10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj10
  %17 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %16, i64 0, i32 2
  store %block* %VarHOLE, %block** %17
  %18 = bitcast { %blockheader, [0 x i64], %block* }* %16 to %block*
  br label %merge8

merge8:                                           ; preds = %notInjection7, %geFirst6
  %phi11 = phi %block* [ %18, %notInjection7 ], [ %VarHOLE, %geFirst6 ]
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 2
  store %block* %phi11, %block** %19
  %20 = inttoptr i64 536870912001 to %block*
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %12, i64 0, i32 3
  store %block* %20, %block** %21
  %22 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %12 to %block*
  %23 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %22)
  %hook_BOOL_not = xor i1 %23, true
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
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %VarX, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %VarX, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = alloca %map
  store %map %VarRho, %map* %12
  %13 = call i1 @hook_MAP_in_keys(%block* %11, %map* %12)
  ret i1 %13
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
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
  %1 = icmp ne i32 %tag, -1
  br i1 %1, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %2 = icmp uge i32 %tag, 126
  br i1 %2, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %3 = icmp ule i32 %tag, 148
  br i1 %3, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %4 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %5 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %4, i64 0, i32 2
  store %block* %VarHOLE, %block** %5
  %6 = bitcast { %blockheader, [0 x i64], %block* }* %4 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %6, %notInjection ], [ %VarHOLE, %geFirst ]
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %phi, %block** %7
  %8 = inttoptr i64 536870912001 to %block*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %8, %block** %9
  %10 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %11 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %10)
  %hook_BOOL_not = xor i1 %11, true
  br label %hook_BOOL_and
}
