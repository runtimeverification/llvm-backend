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
; symbol with 1 or more arguments: %block *
; map: i8 *
; list: i8 *
; set: i8 *
; array: i8 *
; integer: %mpz *
; float: %mpfr *
; string: %string *
; bytes: %string *
; string buffer: %string **
; boolean: i1
; machine integer of N bits: iN

; We also define the following LLVM structure types:

%string = type { i64, [0 x i8] } ; length, bytes
%mpz = type { i32, i32, i64 * } ; mpz_t
%mpfr = type { i64, i32, i64, i64 * } ; mpfr_t
%blockheader = type { i8, i16, i32 } // length in words, layout, symbol
%block = type { %blockheader, [0 x i64 *] } ; header, children

; The layout of a block uniquely identifies the categories of its children as
; well as how to allocate/deallocate them and whether to follow their pointers
; during gc. Roughly speaking, the following rules apply:

; %string *: malloc/free, do not follow
; iN: noop, do not follow
; i8 *: alloced by rust, follow
; %block *: managed heap, follow
; %mpz *: malloc->mpz_init/mpz_clear->free, do not follow
; %mpfr *: malloc->mpfr_init2/mpfr_clear->free, do not follow
; %string **: malloc->malloc/free->free, do not follow

; We also automatically generate for each unique layout id a struct type
; corresponding to the actual layout of that block. For example, if we have
; the symbol symbol foo{Map{}, Int{}, Exp{}} : Exp{}, we would generate the type:

; %layoutN = type { %blockheader, [0 x i64 *], i8 *, %mpz *, %block * }
