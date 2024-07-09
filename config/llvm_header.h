R"LLVM(
target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

; K types in LLVM

; A K value in the LLVM backend can be one of the following values:

; an uninterpreted domain value: \dv{Id}("foo")
; a symbol with 0 or more arguments: foo{}()
; a map: dotMap{}()
; an associative list: dotList{}()
; a set: dotSet{}()
; an array: dotArray{}()
; an arbitrary precision integer: \dv{Int}("0")
; an arbitrary precision float: \dv{Float}("0.0")
; a domain string: \dv{String}("\"\"")
; a byte array: \dv{Bytes}("b\"\"")
; a string buffer: \dv{StringBuffer}("")
; a domain boolean: \dv{Bool}("false")
; a machine integer: \dv{MInt}("0p8")

; For each type, a value of that type has the following llvm type:

; token: %string *
; symbol with 0 arguments: i32
; symbol with 1 or more arguments: %block *
; map: %map
; list: %list
; set: %set
; array: %list
; integer: %mpz *
; float: %floating *
; string: %string *
; bytes: %string *
; string buffer: %stringbuffer *
; boolean: i1
; machine integer of N bits: iN

; We also define the following LLVM structure types:

%string = type { %blockheader, [0 x i8] } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length (or buffer capacity for string pointed by stringbuffers), bytes
%stringbuffer = type { i64, i64, ptr } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, string length, current contents
%map = type { { ptr, i64 } } ; immer::map
%rangemap = type { { { { ptr, { ptr } } } } } ; rng_map::RangeMap
%set = type { { ptr, i64 } } ; immer::set
%iter = type { { ptr, ptr, i32, [14 x ptr] }, { { ptr, i64 } } } ; immer::map_iter / immer::set_iter
%list = type { { i64, i32, ptr, ptr } } ; immer::flex_vector
%mpz = type { i32, i32, ptr } ; mpz_t
%mpz_hdr = type { %blockheader, %mpz } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, mpz_t
%floating = type { i64, { i64, i32, i64, ptr } } ; exp, mpfr_t
%floating_hdr = type { %blockheader, %floating } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, floating
%blockheader = type { i64 }
%block = type { %blockheader, [0 x ptr] } ; 16-bit layout, 8-bit length, 32-bit tag, children

%layout = type { i8, ptr } ; number of children, array of children
%layoutitem = type { i64, i16 } ; offset, category

; The layout of a block uniquely identifies the categories of its children as
; well as how to allocate/deallocate them and whether to follow their pointers
; during gc. Roughly speaking, the following rules apply:

; %string *: malloc/free, do not follow
; iN: noop, do not follow
; %map, %set, %list: noop/drop_in_place, follow
; %block *: managed heap, follow
; %mpz *: malloc/mpz_clear->free, do not follow
; %floating *: malloc/mpfr_clear->free, do not follow
; %stringbuffer *: malloc->malloc/free->free, do not follow

; We also automatically generate for each unique layout id a struct type
; corresponding to the actual layout of that block. For example, if we have
; the symbol symbol foo{Map{}, Int{}, Exp{}} : Exp{}, we would generate the type:

; %layoutN = type { %blockheader, [0 x i64 *], %map, %mpz *, %block * }

; Interface to the configuration parser
declare ptr @parse_configuration(ptr)
declare void @print_configuration(ptr, ptr)

; finish_rewriting.ll

declare i64 @__gmpz_get_ui(ptr)

@exit_int_0 = global %mpz { i32 0, i32 0, ptr getelementptr inbounds ([0 x i64], ptr @exit_int_0_limbs, i32 0, i32 0) }
@exit_int_0_limbs = global [0 x i64] zeroinitializer

define weak tailcc ptr @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(ptr) {
  ret ptr @exit_int_0
}

define i32 @get_exit_code(ptr %subject) {
  %exit_z = call tailcc ptr @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(ptr %subject)
  %exit_ul = call i64 @__gmpz_get_ui(ptr %exit_z)
  %exit_trunc = trunc i64 %exit_ul to i32
  ret i32 %exit_trunc
}

; fresh.ll

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

; getTag.ll

define i32 @get_tag(ptr %arg) {
  %intptr = ptrtoint ptr %arg to i64
  %isConstant = trunc i64 %intptr to i1
  br i1 %isConstant, label %constant, label %block
constant:
  %taglong = lshr i64 %intptr, 32
  br label %exit
block:
  %hdrptr = getelementptr inbounds %block, ptr %arg, i64 0, i32 0, i32 0
  %hdr = load i64, ptr %hdrptr
  %layout = lshr i64 %hdr, @LAYOUT_OFFSET@
  %isstring = icmp eq i64 %layout, 0
  %tagorstring = select i1 %isstring, i64 -1, i64 %hdr
  br label %exit
exit:
  %phi = phi i64 [ %tagorstring, %block ], [ %taglong, %constant ]
  %tag = trunc i64 %phi to i32
  ret i32 %tag
}

; move_float.ll

; helper function for float hooks
define ptr @move_float(ptr %val) {
  %loaded = load %floating, ptr %val
  %malloccall = tail call ptr @kore_alloc_floating(i64 0)
  store %floating %loaded, ptr %malloccall
  ret ptr %malloccall

}

declare noalias ptr @kore_alloc_floating(i64)

; move_int.ll

; helper function for int hooks
define ptr @move_int(ptr %val) {
  %loaded = load %mpz, ptr %val
  %malloccall = tail call ptr @kore_alloc_integer(i64 0)
  store %mpz %loaded, ptr %malloccall
  ret ptr %malloccall
}

declare noalias ptr @kore_alloc_integer(i64)

; string_equal.ll

declare i32 @memcmp(ptr, ptr, i64);

define i1 @string_equal(ptr %str1, ptr %str2, i64 %len1, i64 %len2) {
  %len_eq = icmp eq i64 %len1, %len2
  %len_lt = icmp ult i64 %len1, %len2
  %min_len = select i1 %len_lt, i64 %len1, i64 %len2
  %result = call i32 @memcmp(ptr %str1, ptr %str2, i64 %min_len)
  %prefix_eq = icmp eq i32 %result, 0
  %retval = and i1 %len_eq, %prefix_eq
  ret i1 %retval
}

; take_steps.ll

declare tailcc ptr @k_step(ptr)
declare tailcc void @step_all(ptr)
declare void @serialize_configuration_to_file_v2(ptr, ptr)
declare void @write_uint64_to_file(ptr, i64)

@proof_output = external global i1
@output_file = external global ptr
@depth = thread_local global i64 zeroinitializer
@steps = thread_local global i64 zeroinitializer
@current_interval = thread_local global i64 0
@GC_THRESHOLD = thread_local global i64 @GC_THRESHOLD@

@gc_roots = global [256 x ptr] zeroinitializer

define void @set_gc_threshold(i64 %threshold) {
  store i64 %threshold, ptr @GC_THRESHOLD
  ret void
}

define i64 @get_gc_threshold() {
  %threshold = load i64, ptr @GC_THRESHOLD
  ret i64 %threshold
}

define i1 @finished_rewriting() {
entry:
  %depth = load i64, ptr @depth
  %hasDepth = icmp sge i64 %depth, 0
  %steps = load i64, ptr @steps
  %stepsPlusOne = add i64 %steps, 1
  store i64 %stepsPlusOne, ptr @steps
  br i1 %hasDepth, label %if, label %else
if:
  %depthMinusOne = sub i64 %depth, 1
  store i64 %depthMinusOne, ptr @depth
  %finished = icmp eq i64 %depth, 0
  ret i1 %finished
else:
  ret i1 false
}

define ptr @take_steps(i64 %depth, ptr %subject) {
  %proof_output = load i1, ptr @proof_output
  br i1 %proof_output, label %if, label %merge
if:
  %output_file = load ptr, ptr @output_file
  call void @write_uint64_to_file(ptr %output_file, i64 18446744073709551615)
  call void @serialize_configuration_to_file_v2(ptr %output_file, ptr %subject)
  br label %merge
merge:
  store i64 %depth, ptr @depth
  %result = call tailcc ptr @k_step(ptr %subject)
  ret ptr %result
}

define void @take_search_step(ptr %subject) {
  store i64 -1, ptr @depth
  call tailcc void @step_all(ptr %subject)
  ret void
}

define i64 @get_steps() {
entry:
  %steps = load i64, ptr @steps
  ret i64 %steps
}

attributes #0 = { noreturn }
)LLVM"
