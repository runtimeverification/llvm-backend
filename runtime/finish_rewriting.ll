target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"
 
%blockheader = type { i64 } 
%block = type { %blockheader, [0 x ptr] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%mpz = type { i32, i32, ptr }

declare i64 @__gmpz_get_ui(ptr)

@exit_int_0 = global %mpz { i32 0, i32 0, ptr getelementptr inbounds ([0 x i64], ptr @exit_int_0_limbs, i32 0, i32 0) }
@exit_int_0_limbs = global [0 x i64] zeroinitializer

define weak tailcc ptr @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(ptr) {
  ret ptr @exit_int_0
}

define i32 @get_exit_code(ptr %subject) {
  %exit_z = call tailcc ptr @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(ptr %subject)
  %exit_ul = call i64 @__gmpz_get_ui(ptr %exit_z)
  %exit_trunc = trunc i64 %exit_ul to i32
  ret i32 %exit_trunc
}
