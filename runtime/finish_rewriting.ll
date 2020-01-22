target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%mpz = type { i32, i32, i64* }

declare void @printConfiguration(i8*, %block*)
declare void @printConfigurationToFile(i8*, %block*)
declare void @exit(i32) #0
declare void @abort() #0
declare i64 @__gmpz_get_ui(%mpz*)

declare i8* @getStderr()

@stderr = external global i8*

@exit_int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @exit_int_0_limbs, i32 0, i32 0) }
@exit_int_0_limbs = global [0 x i64] zeroinitializer

define weak fastcc %mpz* @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(%block*) {
  ret %mpz* @exit_int_0
}

@output_file = global i8* zeroinitializer

define void @finish_rewriting(%block* %subject, i1 %error) #0 {
  %output = load i8*, i8** @output_file
  %outputintptr = ptrtoint i8* %output to i64
  %isnull = icmp eq i64 %outputintptr, 0
  br i1 %isnull, label %abort, label %print
abort:
  %stderr = call i8* @getStderr()
  call void @printConfigurationToFile(i8* %stderr, %block* %subject)
  call void @abort()
  unreachable
print:
  call void @printConfiguration(i8* %output, %block* %subject)
  br i1 %error, label %exit, label %exitCode
exitCode:
  %exit_z = call fastcc %mpz* @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(%block* %subject)
  %exit_ul = call i64 @__gmpz_get_ui(%mpz* %exit_z)
  %exit_trunc = trunc i64 %exit_ul to i32
  br label %exit
exit:
  %exit_ui = phi i32 [ %exit_trunc, %exitCode ], [ 113, %print ]
  call void @exit(i32 %exit_ui)
  unreachable
}

attributes #0 = { noreturn }
