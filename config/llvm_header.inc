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
declare ptr @kore_alloc(i64)
declare void @memcpy(ptr, ptr, i64)
declare void @error_on_get(i64, i64)
declare void @error_on_start_substr(i64, i64)
declare void @error_on_end_substr(ptr, i64)
declare void @interger_overflow(i64)
declare void @buffer_overflow_replace_at(i64, i64, i64)
declare void @buffer_overflow_update(i64, i64)
declare void @error_on_update(i64)
declare i1 @hook_BYTES_mutableBytesEnabled()

define ptr @copy_if_needed_in_llvm(ptr %b) {
entry:
  %mutable_bytes_enabled = call i1 @hook_BYTES_mutableBytesEnabled()
  br i1 %mutable_bytes_enabled, label %return_b, label %copy_needed

return_b:
  ret ptr %b

copy_needed:
  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_bytes = mul i64 %len, 8 ; 8 bytes per byte
  %alloc_size = add i64 %len_bytes, 40 ; 8 bytes for header, rest for data
  %alloc = call ptr @kore_alloc(i64 %alloc_size)

  store i64 %len, ptr %alloc

  %data_ptr = getelementptr i8, ptr %alloc, i64 8
  %b_data_ptr = getelementptr i8, ptr %b, i64 8

  call void @memcpy(ptr %data_ptr, ptr %b_data_ptr, i64 %len_bytes)

  ret ptr %data_ptr
}

@exit_int_0 = constant %mpz { i32 0, i32 0, ptr getelementptr inbounds ([0 x i64], ptr @exit_int_0_limbs, i32 0, i32 0) }
@exit_int_0_limbs = constant [0 x i64] zeroinitializer

define tailcc ptr @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(ptr) {
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

define tailcc ptr @"eval_LblgetGeneratedCounterCell{SortGeneratedTopCell{}}"(ptr) {
  call void @abort()
  unreachable
}

declare i32 @get_tag_for_fresh_sort(ptr)
declare ptr @hook_INT_add(ptr, ptr)
declare ptr @evaluate_function_symbol(i32, ptr)
declare ptr @get_terminated_string(ptr)

@fresh_int_1 = constant %mpz { i32 1, i32 1, ptr getelementptr inbounds ([1 x i64], ptr @fresh_int_1_limbs, i32 0, i32 0) }
@fresh_int_1_limbs = constant [1 x i64] [i64 1]

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

declare i32 @memcmp(ptr, ptr, i64)
declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

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
declare void @write_configuration_to_proof_trace(ptr, ptr, i1)

@proof_output = external global i1
@proof_writer = external global ptr
@depth = thread_local global i64 zeroinitializer
@steps = thread_local global i64 zeroinitializer
@current_interval = thread_local global i64 0
@GC_THRESHOLD = thread_local global i64 @GC_THRESHOLD@

@gc_roots = thread_local global [256 x ptr] zeroinitializer

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
  %proof_writer = load ptr, ptr @proof_writer
  call void @write_configuration_to_proof_trace(ptr %proof_writer, ptr %subject, i1 true)
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

define i256 @hook_BYTES_get256(ptr %b, i256 %off) {
entry:
  ; Check for buffer overflow
  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_256 = zext i64 %len to i256
  %off_lt_len = icmp ult i256 %off, %len_256
  br i1 %off_lt_len, label %valid_off, label %overflow
overflow:
  %off_trunc = trunc i256 %off to i64
  call void @error_on_get(i64 %off_trunc, i64 %len)
  unreachable
valid_off:
  ; Load the byte at the specified offset
  %data_ptr = getelementptr %string, ptr %b, i256 0, i32 1, i256 0
  %byte_ptr = getelementptr i8, ptr %data_ptr, i256 %off
  %byte_value = load i8, ptr %byte_ptr

  ; Return the byte value as an i256
  %result = zext i8 %byte_value to i256
  ret i256 %result
}

define ptr @hook_BYTES_substr256(ptr %input, i256 %start, i256 %end) {
entry:
  %end_lt_start = icmp ult i256 %end, %start
  br i1 %end_lt_start, label %error, label %check_length
error:
  %end_trunc = trunc i256 %end to i64
  %start_trunc = trunc i256 %start to i64
  call void @error_on_start_substr(i64 %start_trunc, i64 %end_trunc)
  unreachable
check_length:
  %input_hdr = load i64, ptr %input
  %input_len = and i64 %input_hdr, 1099511627775 ; @LENGTH_MASK@
  %input_len_256 = zext i64 %input_len to i256
  %end_gt_input_len = icmp ugt i256 %end, %input_len_256
  br i1 %end_gt_input_len, label %error_length, label %calculate_length
error_length:
  %end_trunc2 = trunc i256 %end to i64
  call void @error_on_end_substr(ptr %input, i64 %end_trunc2)
  unreachable
calculate_length:
  %len = sub i256 %end, %start
  
  %bytes_len = mul i256 %len, 8
  %alloc_size = add i256 40, %bytes_len
  %alloc_size_i64 = trunc i256 %alloc_size to i64
  %alloc = call ptr @kore_alloc(i64 %alloc_size_i64)

  %len_i64 = trunc i256 %len to i64
  %bytes_ptr = getelementptr [40 x i8], ptr %alloc, i64 0, i64 0
  store i64 %len_i64, ptr %bytes_ptr

  %data_ptr = getelementptr [40 x i8], ptr %alloc, i64 0, i64 8
  %input_data_ptr = getelementptr %string, ptr %input, i256 0, i32 1, i256 0
  %start_ptr = getelementptr i8, ptr %input_data_ptr, i256 %start
  %end_ptr = getelementptr i8, ptr %input_data_ptr, i256 %end
  call void @memcpy(ptr %data_ptr, ptr %start_ptr, i64 %len_i64)

  %result = bitcast ptr %alloc to ptr
  ret ptr %result
}

define i256 @hook_BYTES_length256(ptr %b) {
entry:
  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_256 = zext i64 %len to i256
  ret i256 %len_256
}

define ptr @hook_BYTES_padRight256(ptr %b, i256 %length, i256 %v) {
entry:
  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_256 = zext i64 %len to i256
  %length_gt_len = icmp ugt i256 %length, %len_256
  br i1 %length_gt_len, label %pad, label %return_b

return_b:
  ret ptr %b

pad:
  %v_gt_255 = icmp ugt i256 %v, 255
  br i1 %v_gt_255, label %error, label %allocate

error:
  %v_trunc = trunc i256 %v to i64
  call void @interger_overflow(i64 %v_trunc)
  unreachable
  
allocate:
  %bytes_len = mul i256 %length, 8
  %alloc_size = add i256 %bytes_len, 40 ; 8 bytes for header, rest for data
  %alloc_size_i64 = trunc i256 %alloc_size to i64
  %alloc = call ptr @kore_alloc(i64 %alloc_size_i64)

  %length_i64 = trunc i256 %length to i64
  store i64 %length_i64, ptr %alloc

  %data_ptr = getelementptr i8, ptr %alloc, i64 8
  %b_data_ptr = getelementptr i8, ptr %b, i64 8
  call void @memcpy(ptr %data_ptr, ptr %b_data_ptr, i64 %len)

  %remaining_bytes = sub i64 %length_i64, %len
  %fill_ptr = getelementptr i8, ptr %data_ptr, i64 %len
  %v_i8 = trunc i256 %v to i8
  call void @llvm.memset.p0.i64(ptr %fill_ptr, i8 %v_i8, i64 %remaining_bytes, i1 false)

  %result = bitcast ptr %alloc to ptr
  ret ptr %result
}

define ptr @hook_BYTES_padLeft256(ptr %b, i256 %length, i256 %v) {
entry:
  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_256 = zext i64 %len to i256
  %length_gt_len = icmp ugt i256 %length, %len_256
  br i1 %length_gt_len, label %pad, label %return_b

return_b:
  ret ptr %b

pad:
  %v_gt_255 = icmp ugt i256 %v, 255
  br i1 %v_gt_255, label %error, label %allocate

error:
  %v_trunc = trunc i256 %v to i64
  call void @interger_overflow(i64 %v_trunc)
  unreachable

allocate:
  %bytes_len = mul i256 %length, 8
  %alloc_size = add i256 %bytes_len, 40 ; 8 bytes for header, rest for data
  %alloc_size_i64 = trunc i256 %alloc_size to i64
  %alloc = call ptr @kore_alloc(i64 %alloc_size_i64)

  %length_i64 = trunc i256 %length to i64
  store i64 %length_i64, ptr %alloc
  %data_ptr = getelementptr i8, ptr %alloc, i64 8
  %b_data_ptr = getelementptr i8, ptr %b, i64 8
  %remaining_bytes = sub i64 %length_i64, %len
  %fill_ptr = getelementptr i8, ptr %data_ptr, i64 0
  %v_i8 = trunc i256 %v to i8
  call void @llvm.memset.p0.i64(ptr %fill_ptr, i8 %v_i8, i64 %remaining_bytes, i1 false)

  %b_data_end_ptr = getelementptr i8, ptr %data_ptr, i64 %remaining_bytes
  call void @memcpy(ptr %b_data_end_ptr, ptr %b_data_ptr, i64 %len)

  %result = bitcast ptr %alloc to ptr
  ret ptr %result
}

define ptr @hook_BYTES_replaceAt256(ptr %dest, i256 %index, ptr %src) {
entry:
  call ptr @copy_if_needed_in_llvm(ptr %dest)
 
  %dest_hdr = load i64, ptr %dest
  %dest_len = and i64 %dest_hdr, 1099511627775 ; @LENGTH_MASK@
  %dest_len_256 = zext i64 %dest_len to i256

  %src_hdr = load i64, ptr %src
  %src_len = and i64 %src_hdr, 1099511627775 ; @LENGTH_MASK@
  %src_len_256 = zext i64 %src_len to i256

  %index_sum_dest_src = add i256 %index, %src_len_256
  %index_gt_dest_len = icmp ugt i256 %index_sum_dest_src, %dest_len_256
  
  br i1 %index_gt_dest_len, label %error, label %copy_data

error:
  %index_trunc = trunc i256 %index to i64
  %dest_len_trunc = trunc i256 %dest_len_256 to i64
  %src_len_trunc = trunc i256 %src_len_256 to i64
  call void @buffer_overflow_replace_at(i64 %index_trunc, i64 %dest_len_trunc, i64 %src_len_trunc)
  unreachable

copy_data:
  %data_ptr = getelementptr i8, ptr %dest, i64 8
  %src_data_ptr = getelementptr i8, ptr %src, i64 8
  %index_i64 = trunc i256 %index to i64
  %dest_offset_ptr = getelementptr i8, ptr %data_ptr, i64 %index_i64
  call void @memcpy(ptr %dest_offset_ptr, ptr %src_data_ptr, i64 %src_len)

  ; Return the modified destination
  ret ptr %dest
}

define ptr @hook_MINT_MInt2Bytes(i256 %mint) {
entry:
  %alloc = call ptr @kore_alloc(i64 40) ; 8 bytes for header, 32 bytes for MInt

  ; store 32 as lenght of the MInt in the fist 8 bytes
  %bytes_ptr = getelementptr [40 x i8], ptr %alloc, i64 0, i64 0
  store i64 32, ptr %bytes_ptr

  ; store the MInt in the next 32 bytes
  %mint_ptr = getelementptr [40 x i8], ptr %alloc, i64 0, i64 8
  %mint_vector = bitcast i256 %mint to <32 x i8>
  %reversed_mint = shufflevector <32 x i8> %mint_vector, <32 x i8> undef, <32 x i32> 
    <i32 31, i32 30, i32 29, i32 28, i32 27, i32 26, i32 25, i32 24,
     i32 23, i32 22, i32 21, i32 20, i32 19, i32 18, i32 17, i32 16,
     i32 15, i32 14, i32 13, i32 12, i32 11, i32 10, i32 9,  i32 8,
     i32 7,  i32 6,  i32 5,  i32 4,  i32 3,  i32 2,  i32 1,  i32 0>

  ; Store the reversed MInt bytes in the allocated memory
  %reversed_mint_ptr = alloca <32 x i8>
  store <32 x i8> %reversed_mint, ptr %reversed_mint_ptr
  call void @memcpy(ptr %mint_ptr, ptr %reversed_mint_ptr, i64 32)

  ; Return the pointer to the bytes
  ret ptr %alloc
}

define i256 @hook_MINT_Bytes2MInt(ptr %bytes) {
entry:
  %alloc = alloca [32 x i8]
  %bytes_ptr = getelementptr [32 x i8], ptr %alloc, i64 0, i64 0

  ; The first 8 bytes are the used by the header, so we skip them as
  ; we assume we will always have a 256-bit MInt
  %bytes_contents = getelementptr i8, ptr %bytes, i64 8
  call void @memcpy(ptr %bytes_ptr, ptr %bytes_contents, i64 32)

  ; Convert the bytes to a 256-bit integer
  %mint = load i256, ptr %bytes_ptr

  ; Reverse the byte order to convert from little-endian to big-endian
  %mint_vector = bitcast i256 %mint to <32 x i8>
  %reversed_mint = shufflevector <32 x i8> %mint_vector, <32 x i8> undef, <32 x i32> 
    <i32 31, i32 30, i32 29, i32 28, i32 27, i32 26, i32 25, i32 24,
     i32 23, i32 22, i32 21, i32 20, i32 19, i32 18, i32 17, i32 16,
     i32 15, i32 14, i32 13, i32 12, i32 11, i32 10, i32 9,  i32 8,
     i32 7,  i32 6,  i32 5,  i32 4,  i32 3,  i32 2,  i32 1,  i32 0>

  ; Convert the reversed vector back to i256
  %result = bitcast <32 x i8> %reversed_mint to i256
  ret i256 %result
}

define ptr @hook_BYTES_update256(ptr %b, i256 %index, i256 %value) {
entry:
  call ptr @copy_if_needed_in_llvm(ptr %b)

  %hdr = load i64, ptr %b
  %len = and i64 %hdr, 1099511627775 ; @LENGTH_MASK@
  %len_256 = zext i64 %len to i256

  %index_uge_len = icmp uge i256 %index, %len_256
  br i1 %index_uge_len, label %error, label %update_value

error:
  %index_trunc = trunc i256 %index to i64
  %len_trunc = trunc i256 %len_256 to i64
  call void @buffer_overflow_update(i64 %index_trunc, i64 %len_trunc)
  unreachable

update_value:
  %value_ge_255 = icmp ugt i256 %value, 255
  br i1 %value_ge_255, label %error_value, label %set_value

error_value:
  %value_trunc = trunc i256 %value to i64
  call void @error_on_update(i64 %value_trunc)
  unreachable

set_value:
  %data_ptr = getelementptr i8, ptr %b, i64 8
  %index_i64 = trunc i256 %index to i64
  %byte_ptr = getelementptr i8, ptr %data_ptr, i64 %index_i64
  %value_i8 = trunc i256 %value to i8
  store i8 %value_i8, ptr %byte_ptr

  ; Return the modified buffer
  ret ptr %b
}


define i256 @hook_MINT_pow256(i256 %base, i256 %exp) {
entry:
  ; Special cases: if exp == 0 return 1, if base == 0 return 0
  %exp_is_zero = icmp eq i256 %exp, 0
  br i1 %exp_is_zero, label %exp_zero, label %check_base

exp_zero:
  ret i256 1

check_base:
  %base_is_zero = icmp eq i256 %base, 0
  br i1 %base_is_zero, label %base_zero, label %loop

base_zero:
  ret i256 0

; Initialize result = 1, current_base = base, current_exp = exp
loop:
  %result = phi i256 [1, %check_base], [%next_result, %loop_body]
  %curr_base = phi i256 [%base, %check_base], [%next_base, %loop_body]
  %curr_exp = phi i256 [%exp, %check_base], [%next_exp, %loop_body]
  %exp_done = icmp eq i256 %curr_exp, 0
  br i1 %exp_done, label %done, label %loop_body

loop_body:
  ; if (curr_exp & 1) result *= curr_base
  %exp_and_1 = and i256 %curr_exp, 1
  %is_odd = icmp ne i256 %exp_and_1, 0
  %mul_result = mul i256 %result, %curr_base
  %next_result = select i1 %is_odd, i256 %mul_result, i256 %result
  ; curr_base *= curr_base
  %next_base = mul i256 %curr_base, %curr_base
  ; curr_exp >>= 1
  %next_exp = lshr i256 %curr_exp, 1
  br label %loop

done:
  ret i256 %result
}

define i64 @get_steps() {
entry:
  %steps = load i64, ptr @steps
  ret i64 %steps
}

define i1 @hook_MINT_eq_8(i8 %0, i8 %1) {
  %ret = icmp eq i8 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_16(i16 %0, i16 %1) {
  %ret = icmp eq i16 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_32(i32 %0, i32 %1) {
  %ret = icmp eq i32 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_64(i64 %0, i64 %1) {
  %ret = icmp eq i64 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_128(i128 %0, i128 %1) {
  %ret = icmp eq i128 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_160(i160 %0, i160 %1) {
  %ret = icmp eq i160 %0, %1
  ret i1 %ret
}

define i1 @hook_MINT_eq_256(i256 %0, i256 %1) {
  %ret = icmp eq i256 %0, %1
  ret i1 %ret
}

attributes #0 = { noreturn }
)LLVM"
