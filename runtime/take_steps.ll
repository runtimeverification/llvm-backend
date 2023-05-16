target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare fastcc %block* @k_step(%block*)
declare fastcc %block** @stepAll(%block*, i64*)
declare void @serializeConfigurationToFile(i8*, %block*)
declare void @writeLongToFile(i8*, i64)

@proof_output = external global i1
@output_file = external global i8*
@depth = thread_local global i64 zeroinitializer
@steps = thread_local global i64 zeroinitializer
@current_interval = thread_local global i64 0
@GC_THRESHOLD = thread_local global i64 @GC_THRESHOLD@

@gc_roots = global [256 x i8 *] zeroinitializer

define void @set_gc_threshold(i64 %threshold) {
  store i64 %threshold, i64* @GC_THRESHOLD
  ret void
}

define i64 @get_gc_threshold() {
  %threshold = load i64, i64* @GC_THRESHOLD
  ret i64 %threshold
}

define i1 @finished_rewriting() {
entry:
  %depth = load i64, i64* @depth
  %hasDepth = icmp sge i64 %depth, 0
  %steps = load i64, i64* @steps
  %stepsPlusOne = add i64 %steps, 1
  store i64 %stepsPlusOne, i64* @steps
  br i1 %hasDepth, label %if, label %else
if:
  %depthMinusOne = sub i64 %depth, 1
  store i64 %depthMinusOne, i64* @depth
  %finished = icmp eq i64 %depth, 0
  ret i1 %finished
else:
  ret i1 false
}

define %block* @take_steps(i64 %depth, %block* %subject) {
  %proof_output = load i1, i1* @proof_output
  br i1 %proof_output, label %if, label %merge
if:
  %output_file = load i8*, i8** @output_file
  call void @writeLongToFile(i8* %output_file, i64 18446744073709551615)
  call void @serializeConfigurationToFile(i8* %output_file, %block* %subject)
  br label %merge
merge:
  store i64 %depth, i64* @depth
  %result = call fastcc %block* @k_step(%block* %subject)
  ret %block* %result
}

define %block** @take_search_step(%block* %subject, i64* %count) {
  store i64 -1, i64* @depth
  %result = call fastcc %block** @stepAll(%block* %subject, i64* %count)
  ret %block** %result
}

define i64 @get_steps() {
entry:
  %steps = load i64, i64* @steps
  ret i64 %steps
}
