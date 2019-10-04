target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare fastcc %block* @step(%block*)
declare void @koreCollect(%block **)

@depth = thread_local global i64 zeroinitializer
@INTERVAL = internal thread_local global i64 @GC_INTERVAL@
@current_interval = thread_local global i64 0

@gc_roots = global [256 x i8 *] zeroinitializer

define void @set_gc_interval(i64 %interval) {
  store i64 %interval, i64* @INTERVAL
  ret void
}

define i1 @finished_rewriting() {
entry:
  %depth = load i64, i64* @depth
  %hasDepth = icmp sge i64 %depth, 0
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
  %currInterval = load i64, i64* @current_interval
  %interval = load i64, i64* @INTERVAL
  %isCollect = icmp eq i64 %interval, %currInterval
  %currIntervalPlusOne = add i64 %currInterval, 1
  %newInterval = select i1 %isCollect, i64 0, i64 %currIntervalPlusOne
  store i64 %newInterval, i64* @current_interval
  ret i1 %isCollect
}

define %block* @take_steps(i64 %depth, %block* %subject) {
  store i64 %depth, i64* @depth
  %result = call fastcc %block* @step(%block* %subject)
  ret %block* %result
}
