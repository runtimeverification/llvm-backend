declare i8* @__rg_alloc(i64, i64)
declare i8* @__rg_alloc_zeroed(i64, i64)
declare void @__rg_dealloc(i8*, i64, i64)
declare i8* @__rg_realloc(i8*, i64, i64, i64)

define i8* @__rust_alloc(i64, i64) {
  %3 = tail call i8* @__rg_alloc(i64 %0, i64 %1)
  ret i8* %3
}
define i8* @__rust_alloc_zeroed(i64, i64) {
  %3 = tail call i8* @__rg_alloc_zeroed(i64 %0, i64 %1)
  ret i8* %3
}
define void @__rust_dealloc(i8*, i64, i64) {
  tail call void @__rg_dealloc(i8* %0, i64 %1, i64 %2)
  ret void
}
define i8* @__rust_realloc(i8*, i64, i64, i64) {
  %5 = tail call i8* @__rg_realloc(i8* %0, i64 %1, i64 %2, i64 %3)
  ret i8* %5
}
