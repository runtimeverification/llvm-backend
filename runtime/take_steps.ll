target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare %block* @step(%block*)
declare void @finish_rewriting(%block*) #0
declare void @koreCollect(%block **)

define void @take_steps_no_depth(%block* %subject) #0 {
entry:
  br label %loop

loop:
  %currSubject = phi %block* [ %subject, %entry ], [ %finalSubject, %finish ]
  %currInterval = phi i32 [ 2000, %entry ], [ %nextInterval, %finish ]
  %nextSubject = call %block* @step(%block* %currSubject)
  %isCollect = icmp eq i32 %currInterval, 0
  %currIntervalMinusOne = sub i32 %currInterval, 1
  %nextInterval = select i1 %isCollect, i32 2000, i32 %currIntervalMinusOne
  br i1 %isCollect, label %collect, label %finish
collect:
  %subjPtr = alloca %block*
  store %block* %nextSubject, %block** %subjPtr
  call void @koreCollect(%block** %subjPtr)
  %collected = load %block*, %block** %subjPtr
  br label %finish
finish:
  %finalSubject = phi %block* [ %collected, %collect ], [ %nextSubject, %loop ]
  br label %loop
}


define void @take_steps_depth(i32 %depth, %block* %subject) #0 {
entry:
  %subjPtr = alloca %block*
  br label %loop
loop:
  %currDepth = phi i32 [ %depth, %entry ], [ %nextDepth, %finish ]
  %currSubject = phi %block* [ %subject, %entry ], [ %finalSubject, %finish ]
  %currInterval = urem i32 %currDepth, 2000
  %atEnd = icmp eq i32 %currDepth, 0
  br i1 %atEnd, label %finished, label %nextStep
finished:
  call void @finish_rewriting(%block* %currSubject)
  unreachable
nextStep:
  %nextSubject = call %block* @step(%block* %currSubject)
  %isCollect = icmp eq i32 %currInterval, 0
  br i1 %isCollect, label %collect, label %finish
collect:
  store %block* %nextSubject, %block** %subjPtr
  call void @koreCollect(%block** %subjPtr)
  %collected = load %block*, %block** %subjPtr
  br label %finish
finish:
  %finalSubject = phi %block* [ %collected, %collect ], [ %nextSubject, %nextStep ]
  %nextDepth = sub i32 %currDepth, 1
  br label %loop
}

define void @take_steps(i32 %depth, %block* %subject) #0 {
  %hasDepth = icmp sge i32 %depth, 0
  br i1 %hasDepth, label %if, label %else
if:
  call void @take_steps_depth(i32 %depth, %block* %subject)
  unreachable
else:
  call void @take_steps_no_depth(%block* %subject)
  unreachable
}

attributes #0 = { noreturn }
