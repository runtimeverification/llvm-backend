target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%string = type { %blockheader, [0 x i8] } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length (or buffer capacity for string pointed by stringbuffers), bytes
%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%mpz = type { i32, i32, i64* }

declare void @abort() #0

define weak fastcc %block* @"eval_LblgetGeneratedCounterCell{SortGeneratedTopCell{}}"(%block*) {
  call void @abort()
  unreachable
}

declare i32 @getTagForFreshSort(i8*)
declare %mpz* @hook_INT_add(%mpz*, %mpz*)
declare i8* @evaluateFunctionSymbol(i32, i8**)
declare i8* @getTerminatedString(%string*)

@fresh_int_1 = global %mpz { i32 1, i32 1, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @fresh_int_1_limbs, i32 0, i32 0) }
@fresh_int_1_limbs = global [1 x i64] [i64 1]

define i8* @get_fresh_constant(%string* %sort, %block* %top) {
entry:
  %counterCell = call fastcc %block* @"eval_LblgetGeneratedCounterCell{SortGeneratedTopCell{}}"(%block* %top)
  %counterCellPointer = getelementptr %block, %block* %counterCell, i64 0, i32 1, i64 0
  %mpzPtrPtr = bitcast i64** %counterCellPointer to %mpz**
  %currCounter = load %mpz*, %mpz** %mpzPtrPtr
  %nextCounter = call %mpz* @hook_INT_add(%mpz* %currCounter, %mpz* @fresh_int_1)
  store %mpz* %nextCounter, %mpz** %mpzPtrPtr
  %sortData = call i8* @getTerminatedString(%string* %sort)
  %tag = call i32 @getTagForFreshSort(i8* %sortData)
  %args = alloca i8*
  %voidPtr = bitcast %mpz* %currCounter to i8*
  store i8* %voidPtr, i8** %args
  %retval = call i8* @evaluateFunctionSymbol(i32 %tag, i8** %args)
  ret i8* %retval
}

attributes #0 = { noreturn }
