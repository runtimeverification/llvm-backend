target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

declare i32 @memcmp(i8*, i8*, i64);

define i1 @string_equal(i8* %str1, i8* %str2, i64 %len1, i64 %len2) {
  %len_eq = icmp eq i64 %len1, %len2
  %len_lt = icmp ult i64 %len1, %len2
  %min_len = select i1 %len_lt, i64 %len1, i64 %len2
  %result = call i32 @memcmp(i8* %str1, i8* %str2, i64 %min_len)
  %prefix_eq = icmp eq i32 %result, 0
  %retval = and i1 %len_eq, %prefix_eq
  ret i1 %retval
}
