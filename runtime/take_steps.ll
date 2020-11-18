target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare fastcc %block* @step(%block*)
declare i8* @youngspace_ptr()
declare i8** @young_alloc_ptr()
declare i64 @ptrDiff(i8*, i8*)

@depth = thread_local global i64 zeroinitializer
@steps = thread_local global i64 zeroinitializer
@current_interval = thread_local global i64 0
@GC_THRESHOLD = thread_local global i64 @GC_THRESHOLD@

@gc_roots = global [256 x i8 *] zeroinitializer

define void @set_gc_threshold(i64 %threshold) {
  store i64 %threshold, i64* @GC_THRESHOLD
  ret void
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

define i1 @is_collection() {
entry:
  %threshold = load i64, i64* @GC_THRESHOLD
  %youngspaceStart = call i8* @youngspace_ptr()
  %youngspaceEndPtr = call i8** @young_alloc_ptr()
  %youngspaceEnd = load i8*, i8** %youngspaceEndPtr
  %youngspaceAllocatedBytes = call i64 @ptrDiff(i8* %youngspaceEnd, i8* %youngspaceStart)
  %allocatedTimes100 = mul i64 %youngspaceAllocatedBytes, 100
  %thresholdTimes95 = mul i64 %threshold, 95
  %collection = icmp ugt i64 %allocatedTimes100, %thresholdTimes95
  ret i1 %collection
}

define %block* @take_steps(i64 %depth, %block* %subject) {
  store i64 %depth, i64* @depth
  %result = call fastcc %block* @step(%block* %subject)
  ret %block* %result
}
