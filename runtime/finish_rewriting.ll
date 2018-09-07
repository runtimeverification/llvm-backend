target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare void @printConfiguration(i32, %block*)
declare void @exit(i32)

define void @finish_rewriting(%block* %subject) #0 {
  call void @printConfiguration(i32 1, %block* %subject)
  call void @exit(i32 0)
  unreachable
}

attributes #0 = { noreturn }
