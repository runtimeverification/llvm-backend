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

define i1 @side_condition_208(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

declare noalias i8* @malloc(i64)

declare i32 @getTag(%block*)

declare i1 @"eval_LblisKResult{SortK{}}"(%block*)

define i1 @side_condition_210(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_211(%mpz*) {
entry:
  %1 = call i1 @hook_INT_ne(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

declare i1 @hook_INT_ne(%mpz*, %mpz*)

define i1 @side_condition_212(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_214(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_223(%mpz*, %mpz*) {
entry:
  %2 = call i1 @hook_INT_le(%mpz* %0, %mpz* %1)
  ret i1 %2
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define i1 @side_condition_225(%mpz*) {
entry:
  %1 = call i1 @hook_INT_ne(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

define i1 @side_condition_234(%map*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %1)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %1, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %10 = inttoptr i64 536870912001 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = call i1 @hook_MAP_in_keys(%block* %12, %map* %0)
  %hook_BOOL_not = xor i1 %13, true
  ret i1 %hook_BOOL_not
}

declare i1 @hook_MAP_in_keys(%block*, %map*)

define i1 @side_condition_237(%mpz*, %mpz*) {
entry:
  %2 = call i1 @hook_INT_ge(%mpz* %0, %mpz* %1)
  ret i1 %2
}

declare i1 @hook_INT_ge(%mpz*, %mpz*)

define i1 @side_condition_250(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_269(%mpz*) {
entry:
  %1 = call i1 @hook_INT_ne(%mpz* %0, %mpz* @int_0)
  ret i1 %1
}

define i1 @side_condition_272(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_293(i1) {
entry:
  ret i1 %0
}

define i1 @side_condition_308(i1) {
entry:
  %hook_BOOL_not = xor i1 %0, true
  ret i1 %hook_BOOL_not
}

define i1 @side_condition_314(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_321(%block*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %1)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %1, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %10 = inttoptr i64 536870912001 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %12)
  br i1 %13, label %then, label %hook_BOOL_and

then:                                             ; preds = %merge
  %malloccall2 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall2 to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq3 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq3
  %tag4 = call i32 @getTag(%block* %0)
  %15 = icmp ne i32 %tag4, -1
  br i1 %15, label %notString5, label %notInjection7

hook_BOOL_and:                                    ; preds = %merge8, %merge
  %phi12 = phi i1 [ %hook_BOOL_not, %merge8 ], [ %13, %merge ]
  ret i1 %phi12

notString5:                                       ; preds = %then
  %16 = icmp uge i32 %tag4, 126
  br i1 %16, label %geFirst6, label %notInjection7

geFirst6:                                         ; preds = %notString5
  %17 = icmp ule i32 %tag4, 148
  br i1 %17, label %merge8, label %notInjection7

notInjection7:                                    ; preds = %geFirst6, %notString5, %then
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %18 = bitcast i8* %malloccall9 to { %blockheader, [0 x i64], %block* }*
  %inj10 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %18, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj10
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %18, i64 0, i32 2
  store %block* %0, %block** %19
  %20 = bitcast { %blockheader, [0 x i64], %block* }* %18 to %block*
  br label %merge8

merge8:                                           ; preds = %notInjection7, %geFirst6
  %phi11 = phi %block* [ %20, %notInjection7 ], [ %0, %geFirst6 ]
  %21 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 2
  store %block* %phi11, %block** %21
  %22 = inttoptr i64 536870912001 to %block*
  %23 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %14, i64 0, i32 3
  store %block* %22, %block** %23
  %24 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %14 to %block*
  %25 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %24)
  %hook_BOOL_not = xor i1 %25, true
  br label %hook_BOOL_and
}

define i1 @side_condition_325(%map*, %block*) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %2 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %1)
  %3 = icmp ne i32 %tag, -1
  br i1 %3, label %notString, label %notInjection

notString:                                        ; preds = %entry
  %4 = icmp uge i32 %tag, 126
  br i1 %4, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %5 = icmp ule i32 %tag, 148
  br i1 %5, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %entry
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %6 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 0
  store %blockheader { i64 562958543356044 }, %blockheader* %inj
  %7 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %6, i64 0, i32 2
  store %block* %1, %block** %7
  %8 = bitcast { %blockheader, [0 x i64], %block* }* %6 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %8, %notInjection ], [ %1, %geFirst ]
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 2
  store %block* %phi, %block** %9
  %10 = inttoptr i64 536870912001 to %block*
  %11 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %2, i64 0, i32 3
  store %block* %10, %block** %11
  %12 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %2 to %block*
  %13 = call i1 @hook_MAP_in_keys(%block* %12, %map* %0)
  ret i1 %13
}

define i1 @side_condition_327(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356046 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}

define i1 @side_condition_329(%block*) {
entry:
  br i1 true, label %then, label %hook_BOOL_and

then:                                             ; preds = %entry
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 0
  store %blockheader { i64 281487861612693 }, %blockheader* %kseq
  %tag = call i32 @getTag(%block* %0)
  %2 = icmp ne i32 %tag, -1
  br i1 %2, label %notString, label %notInjection

hook_BOOL_and:                                    ; preds = %merge, %entry
  %phi2 = phi i1 [ %hook_BOOL_not, %merge ], [ true, %entry ]
  ret i1 %phi2

notString:                                        ; preds = %then
  %3 = icmp uge i32 %tag, 126
  br i1 %3, label %geFirst, label %notInjection

geFirst:                                          ; preds = %notString
  %4 = icmp ule i32 %tag, 148
  br i1 %4, label %merge, label %notInjection

notInjection:                                     ; preds = %geFirst, %notString, %then
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %5 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 0
  store %blockheader { i64 562958543356035 }, %blockheader* %inj
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %5, i64 0, i32 2
  store %block* %0, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block* }* %5 to %block*
  br label %merge

merge:                                            ; preds = %notInjection, %geFirst
  %phi = phi %block* [ %7, %notInjection ], [ %0, %geFirst ]
  %8 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 2
  store %block* %phi, %block** %8
  %9 = inttoptr i64 536870912001 to %block*
  %10 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %1, i64 0, i32 3
  store %block* %9, %block** %10
  %11 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %1 to %block*
  %12 = call fastcc i1 @"eval_LblisKResult{SortK{}}"(%block* %11)
  %hook_BOOL_not = xor i1 %12, true
  br label %hook_BOOL_and
}
