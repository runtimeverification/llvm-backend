target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare %block* @parseConfiguration(i8*)
declare i64 @atol(i8*)

declare %block* @take_steps(i64, %block*)
declare void @finish_rewriting(%block*) #0

declare void @initStaticObjects()

@output_file = external global i8*

define i32 @main(i32 %argc, i8** %argv) {
entry:
  %filename_ptr = getelementptr inbounds i8*, i8** %argv, i64 1
  %filename = load i8*, i8** %filename_ptr
  %depth_ptr = getelementptr inbounds i8*, i8** %argv, i64 2
  %depth_str = load i8*, i8** %depth_ptr
  %depth = call i64 @atol(i8* %depth_str)
  %output_ptr = getelementptr inbounds i8*, i8** %argv, i64 3
  %output_str = load i8*, i8** %output_ptr
  store i8* %output_str, i8** @output_file

  call void @initStaticObjects()

  %ret = call %block* @parseConfiguration(i8* %filename)
  %result = call %block* @take_steps(i64 %depth, %block* %ret)
  call void @finish_rewriting(%block* %result)
  unreachable
}

attributes #0 = { noreturn }
