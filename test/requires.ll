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

define i1 @side_condition_217(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ge(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_ge(%mpz*, %mpz*)

define i1 @side_condition_243(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

declare i1 @hook_INT_ne(%mpz*, %mpz*)

define i1 @side_condition_253(%map %VarRho, %block* %VarX) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612656 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543356006 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarX, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = inttoptr i64 377957122049 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %8 = alloca %map
  store %map %VarRho, %map* %8
  %9 = call i1 @hook_MAP_in_keys(%block* %7, %map* %8)
  %hook_BOOL_not = xor i1 %9, true
  ret i1 %hook_BOOL_not
}

declare noalias i8* @malloc(i64)

declare i1 @hook_MAP_in_keys(%block*, %map*)

define i1 @side_condition_267(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_275(%map %VarRho, %block* %VarX) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612656 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543356006 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarX, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = inttoptr i64 377957122049 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %8 = alloca %map
  store %map %VarRho, %map* %8
  %9 = call i1 @hook_MAP_in_keys(%block* %7, %map* %8)
  ret i1 %9
}

define i1 @side_condition_302(%block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block*, %block* }* getelementptr ({ %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block*, %block* }*
  %kseq = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 0
  store %blockheader { i64 281487861612656 }, %blockheader* %kseq
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %1 = bitcast i8* %malloccall1 to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 0
  store %blockheader { i64 562958543355999 }, %blockheader* %inj
  %2 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %1, i64 0, i32 2
  store %block* %VarK0, %block** %2
  %3 = bitcast { %blockheader, [0 x i64], %block* }* %1 to %block*
  %4 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 2
  store %block* %3, %block** %4
  %5 = inttoptr i64 377957122049 to %block*
  %6 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %0, i64 0, i32 3
  store %block* %5, %block** %6
  %7 = bitcast { %blockheader, [0 x i64], %block*, %block* }* %0 to %block*
  %8 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %7)
  ret i1 %8
}

declare i1 @"eval_LblisKResult{SortK{}}"(%block*)

define i1 @side_condition_313(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_le(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define i1 @side_condition_315(i1 %VarC) {
entry:
  %hook_BOOL_not = xor i1 %VarC, true
  ret i1 %hook_BOOL_not
}

define i1 @side_condition_329(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_359(i1 %VarC) {
entry:
  ret i1 %VarC
}
