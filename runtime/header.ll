; K types in LLVM

; A K value in the LLVM backend can be one of the following values:

; an uninterpreted domain value: \dv{Id}("foo")
; a symbol with 0 or more arguments: foo{}()
; a map: dotMap{}()
; an associative list: dotList{}()
; a set: dotSet{}()
; an array: dotArray{}()
; an arbitrary precision integer: \dv{Int}("0")
; an arbitrary precision float: \dv{Float}("0.0")
; a domain string: \dv{String}("\"\"")
; a byte array: \dv{Bytes}("b\"\"")
; a string buffer: \dv{StringBuffer}("")
; a domain boolean: \dv{Bool}("false")
; a machine integer: \dv{MInt}("0p8")

; For each type, a value of that type has the following llvm type:

; token: %string *
; symbol with 0 arguments: i32
; symbol with 1 or more arguments: %block A
; map: %map *
; list: %list *
; set: %set *
; array: %list *
; integer: %mpz *
; float: %mpfr *
; string: %string *
; bytes: %string *
; string buffer: %string **
; boolean: i1
; machine integer of N bits: iN *

; We also define the following LLVM structure types:

%string = type { i64, [0 x i8] } ; length, bytes
%mpz = type { i32, i32, i64 * } ; mpz_t
%mpfr = type { i64, i32, i64, i64 * } ; mpfr_t
%list = type { i64, i32, i8 *, i8 * } ; immer::flex_vector
%set = type { i8 *, i64 } ; immer::set
%map = type { i8 *, i64 } ; immer::map
%block = type { i8, i16, i32, [0 x i64 *] } ; length, layout, symbol, children

; The layout of a block uniquely identifies the categories of its children as
; well as how to allocate/deallocate them and whether to follow their pointers
; during gc. Roughly speaking, the following rules apply:

; %string *: malloc/free, do not follow
; iN: noop, do not follow
; %block *, %list *, %set *, %map *: managed heap, follow
; %mpz *: malloc->mpz_init/mpz_clear->free, do not follow
; %mpfr *: malloc->mpfr_init2/mpfr_clear->free, do not follow
; %string **: malloc->malloc/free->free, do not follow
; iN *: malloc/free, do not follow

