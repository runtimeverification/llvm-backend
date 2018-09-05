target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }
%map = type { i64, i8*, i8* }
%set = type { i8*, i8*, i64 }

@int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @int_0_limbs, i32 0, i32 0) }
@int_0_limbs = global [0 x i64] zeroinitializer

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define i1 @side_condition_230(%map %VarRho, %block* %VarX) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 562958543356006 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %VarX, %block** %1
  %2 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  %3 = alloca %map
  store %map %VarRho, %map* %3
  %4 = call %set @hook_MAP_keys(%map* %3)
  %5 = alloca %set
  store %set %4, %set* %5
  %6 = call i1 @hook_SET_in(%block* %2, %set* %5)
  %hook_BOOL_not = xor i1 %6, true
  ret i1 %hook_BOOL_not
}

declare noalias i8* @malloc(i64)

declare %set @hook_MAP_keys(%map*)

declare i1 @hook_SET_in(%block*, %set*)

define i1 @side_condition_258(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

declare i1 @hook_INT_ne(%mpz*, %mpz*)

define i1 @side_condition_297(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_le(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_le(%mpz*, %mpz*)

define i1 @side_condition_304(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_327(%mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ne(%mpz* %VarI2, %mpz* @int_0)
  ret i1 %0
}

define i1 @side_condition_350(%block* %VarK0) {
entry:
  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ %blockheader, [0 x i64], %block* }* getelementptr ({ %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* null, i32 1) to i64))
  %0 = bitcast i8* %malloccall to { %blockheader, [0 x i64], %block* }*
  %inj = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 0
  store %blockheader { i64 562958543355995 }, %blockheader* %inj
  %1 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %0, i64 0, i32 2
  store %block* %VarK0, %block** %1
  %2 = bitcast { %blockheader, [0 x i64], %block* }* %0 to %block*
  %3 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %2)
  ret i1 %3
}

declare i1 @"eval_LblisKResult{SortK{}}"(%block*)

define i1 @side_condition_359(%mpz* %VarI1, %mpz* %VarI2) {
entry:
  %0 = call i1 @hook_INT_ge(%mpz* %VarI1, %mpz* %VarI2)
  ret i1 %0
}

declare i1 @hook_INT_ge(%mpz*, %mpz*)
