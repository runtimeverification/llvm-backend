target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%map = type { i8 *, i64 } ; immer::map
%set = type { i8 *, i64 } ; immer::set
%list = type { i64, i32, i8 *, i8 * } ; immer::flex_vector
%mpz = type { i32, i32, i64 * } ; mpz_t
%floating = type { i64, { i64, i32, i64, i64 * } } ; exp, mpfr_t
%layoutitem = type { i64, i16 }
%layout = type { i8, %layoutitem* }

declare i32 @memcmp(i8* %ptr1, i8* %ptr2, i64 %num)
declare void @abort() #0
declare %layout* @getLayoutData(i16)

declare i1 @hook_MAP_eq(%map*, %map*)
declare i1 @hook_LIST_eq(%list*, %list*)
declare i1 @hook_SET_eq(%set*, %set*)
declare i1 @hook_INT_eq(%mpz*, %mpz*)
declare i1 @hook_FLOAT_trueeq(%floating*, %floating*)
declare i1 @hook_STRING_eq(%block*, %block*)
declare i1 @hook_KEQUAL_eq(%block* %arg1, %block* %arg2)

define i1 @hook_KEQUAL_ne(%block* %arg1, %block* %arg2) {
  %eq = call i1 @hook_KEQUAL_eq(%block* %arg1, %block* %arg2)
  %ne = xor i1 %eq, 1
  ret i1 %ne
}

define i1 @hook_BOOL_eq(i1 %arg1, i1 %arg2) #1 {
  %eq = icmp eq i1 %arg1, %arg2
  ret i1 %eq
}

attributes #0 = { noreturn }
attributes #1 = { alwaysinline }
