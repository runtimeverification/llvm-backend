target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare %block* @step(%block*)
declare void @finish_rewriting(%block*) #0

define void @take_steps_no_depth(%block* %subject) #0 {
entry:
  br label %loop
loop:
  %currSubject = phi %block* [ %subject, %entry ], [ %nextSubject, %loop ]
  %nextSubject = call %block* @step(%block* %currSubject)
  br label %loop
}


define void @take_steps_depth(i32 %depth, %block* %subject) #0 {
entry:
  br label %loop
loop:
  %currDepth = phi i32 [ %depth, %entry ], [ %nextDepth, %nextStep ]
  %currSubject = phi %block* [ %subject, %entry ], [ %nextSubject, %nextStep ]
  %atEnd = icmp eq i32 %currDepth, 0
  br i1 %atEnd, label %finished, label %nextStep
finished:
  call void @finish_rewriting(%block* %subject)
  unreachable
nextStep:
  %nextSubject = call %block* @step(%block* %currSubject)
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
