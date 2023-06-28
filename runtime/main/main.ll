target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

declare %block* @parseConfiguration(i8*)
declare i64 @atol(i8*)

declare %block* @take_steps(i64, %block*)
declare void @finish_rewriting(%block*, i1) #0

declare void @initStaticObjects()

@statistics.flag = private constant [13 x i8] c"--statistics\00"
@binary_out.flag = private constant [16 x i8] c"--binary-output\00"
@proof_out.flag = private constant [15 x i8] c"--proof-output\00"

@output_file = external global i8*
@statistics = external global i1
@binary_output = external global i1
@proof_output = external global i1

declare i32 @strcmp(i8* %a, i8* %b)

define void @parse_flags(i32 %argc, i8** %argv) {
entry:
  store i1 0, i1* @statistics
  br label %header

header:
  %idx = phi i32 [ 4, %entry ], [ %idx.inc, %inc ]
  %continue = icmp slt i32 %idx, %argc
  br i1 %continue, label %body, label %exit

body:
  %argv.idx = getelementptr inbounds i8*, i8** %argv, i32 %idx
  %arg = load i8*, i8** %argv.idx
  br label %body.stats

body.stats:
  %stats.cmp = call i32 @strcmp(i8* %arg, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @statistics.flag, i64 0, i64 0))
  %stats.eq = icmp eq i32 %stats.cmp, 0
  br i1 %stats.eq, label %set.stats, label %binary.body

set.stats:
  store i1 1, i1* @statistics
  br label %binary.body

binary.body:
  %binary.cmp = call i32 @strcmp(i8* %arg, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @binary_out.flag, i64 0, i64 0))
  %binary.eq = icmp eq i32 %binary.cmp, 0
  br i1 %binary.eq, label %binary.set, label %proof.body

binary.set:
  store i1 1, i1* @binary_output
  br label %proof.body

proof.body:
  %proof.cmp = call i32 @strcmp(i8* %arg, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @proof_out.flag, i64 0, i64 0))
  %proof.eq = icmp eq i32 %proof.cmp, 0
  br i1 %proof.eq, label %proof.set, label %body.tail

proof.set:
  store i1 1, i1* @proof_output
  br label %body.tail

body.tail:
  br label %inc

inc:
  %idx.inc = add i32 %idx, 1
  br label %header

exit:
  ret void
}

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
  
  call void @parse_flags(i32 %argc, i8** %argv)

  call void @initStaticObjects()

  %ret = call %block* @parseConfiguration(i8* %filename)
  %result = call %block* @take_steps(i64 %depth, %block* %ret)
  call void @finish_rewriting(%block* %result, i1 0)
  unreachable
}

attributes #0 = { noreturn }
