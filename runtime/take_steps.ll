target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare %block* @step(%block*)
declare void @finish_rewriting(%block*) #0
declare void @koreCollect(%block **)

define %block* @take_steps_no_depth(%block* %subject) {
entry:
  br label %loop

loop:
  %currSubject = phi %block* [ %subject, %entry ], [ %finalSubject, %finish ]
  %currInterval = phi i32 [ 2000, %entry ], [ %nextInterval, %finish ]
  %nextSubject = call %block* @step(%block* %currSubject)
  %intptr = ptrtoint %block* %nextSubject to i64
  %isnull = icmp eq i64 %intptr, 0
  br i1 %isnull, label %finished, label %checkCollect
finished:
  ret %block* %currSubject
checkCollect:
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
  %finalSubject = phi %block* [ %collected, %collect ], [ %nextSubject, %checkCollect ]
  br label %loop
}


define %block* @take_steps_depth(i32 %depth, %block* %subject) {
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
  ret %block* %currSubject
nextStep:
  %nextSubject = call %block* @step(%block* %currSubject)
  %intptr = ptrtoint %block* %nextSubject to i64
  %isnull = icmp eq i64 %intptr, 0
  br i1 %isnull, label %finished, label %checkCollect
checkCollect:
  %isCollect = icmp eq i32 %currInterval, 0
  br i1 %isCollect, label %collect, label %finish
collect:
  store %block* %nextSubject, %block** %subjPtr
  call void @koreCollect(%block** %subjPtr)
  %collected = load %block*, %block** %subjPtr
  br label %finish
finish:
  %finalSubject = phi %block* [ %collected, %collect ], [ %nextSubject, %checkCollect ]
  %nextDepth = sub i32 %currDepth, 1
  br label %loop
}

define %block* @take_steps(i32 %depth, %block* %subject) {
  %hasDepth = icmp sge i32 %depth, 0
  br i1 %hasDepth, label %if, label %else
if:
  %result = call %block* @take_steps_depth(i32 %depth, %block* %subject)
  ret %block* %result
else:
  %result2 = call %block* @take_steps_no_depth(%block* %subject)
  ret %block* %result2
}
