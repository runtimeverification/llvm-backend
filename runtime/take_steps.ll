target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x ptr] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare tailcc ptr @k_step(ptr)
declare tailcc void @step_all(ptr)
declare void @serialize_configuration_to_proof_writer(ptr, ptr)
declare void @write_uint64_to_proof_trace(ptr, i64)

@proof_output = external global i1
@proof_writer = external global ptr
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
  %proof_writer = load ptr, ptr @proof_writer
  call void @write_uint64_to_proof_trace(ptr %proof_writer, i64 18446744073709551615)
  call void @serialize_configuration_to_proof_writer(ptr %proof_writer, ptr %subject)
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
