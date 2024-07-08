target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%string = type { %blockheader, [0 x i8] } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length (or buffer capacity for string pointed by stringbuffers), bytes
%blockheader = type { i64 } 
%block = type { %blockheader, [0 x ptr] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%mpz = type { i32, i32, ptr }

declare void @abort() #0

define weak tailcc ptr @"eval_LblgetGeneratedCounterCell{SortGeneratedTopCell{}}"(ptr) {
  call void @abort()
  unreachable
}

declare i32 @get_tag_for_fresh_sort(ptr)
declare ptr @hook_INT_add(ptr, ptr)
declare ptr @evaluate_function_symbol(i32, ptr)
declare ptr @get_terminated_string(ptr)

@fresh_int_1 = global %mpz { i32 1, i32 1, ptr getelementptr inbounds ([1 x i64], ptr @fresh_int_1_limbs, i32 0, i32 0) }
@fresh_int_1_limbs = global [1 x i64] [i64 1]

define ptr @get_fresh_constant(ptr %sort, ptr %top) {
entry:
  %counterCell = call tailcc ptr @"eval_LblgetGeneratedCounterCell{SortGeneratedTopCell{}}"(ptr %top)
  %counterCellPointer = getelementptr %block, ptr %counterCell, i64 0, i32 1, i64 0
  %currCounter = load ptr, ptr %counterCellPointer
  %nextCounter = call ptr @hook_INT_add(ptr %currCounter, ptr @fresh_int_1)
  store ptr %nextCounter, ptr %counterCellPointer
  %sortData = call ptr @get_terminated_string(ptr %sort)
  %tag = call i32 @get_tag_for_fresh_sort(ptr %sortData)
  %args = alloca ptr
  store ptr %currCounter, ptr %args
  %retval = call ptr @evaluate_function_symbol(i32 %tag, ptr %args)
  ret ptr %retval
}

attributes #0 = { noreturn }
