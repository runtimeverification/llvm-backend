target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"
 
%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%mpz = type { i32, i32, i64* }

declare i64 @__gmpz_get_ui(%mpz*)

@exit_int_0 = global %mpz { i32 0, i32 0, i64* getelementptr inbounds ([0 x i64], [0 x i64]* @exit_int_0_limbs, i32 0, i32 0) }
@exit_int_0_limbs = global [0 x i64] zeroinitializer

define weak tailcc %mpz* @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(%block*) {
  ret %mpz* @exit_int_0
}

define i32 @get_exit_code(%block* %subject) {
  %exit_z = call tailcc %mpz* @"eval_LblgetExitCode{SortGeneratedTopCell{}}"(%block* %subject)
  %exit_ul = call i64 @__gmpz_get_ui(%mpz* %exit_z)
  %exit_trunc = trunc i64 %exit_ul to i32
  ret i32 %exit_trunc
}
