target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 }
%mpz = type { i32, i32, i64* }
%block = type { %blockheader, [0 x i64*] }
%map = type { { i8*, i64 } }
%list = type { { i64, i32, i8*, i8* } }
%set = type { { i8*, i64 } }
%floating = type { i64, { i64, i32, i64, i64* } }
%string = type { %blockheader, [0 x i8] }

define %block addrspace(1)* addrspace(1)* @addrspace_0_to_1_p1s_blocks(%block addrspace(1)** %in) {
  %out = addrspacecast %block addrspace(1)** %in to %block addrspace(1)* addrspace(1)*
  ret %block addrspace(1)* addrspace(1)* %out
}

define %block addrspace(1)* @addrspace_0_to_1_s_blocks(%block* %in) {
  %out = addrspacecast %block* %in to %block addrspace(1)*
  ret %block addrspace(1)* %out
}

define %mpz addrspace(1)* @addrspace_0_to_1_s_mpzs(%mpz* %in) {
  %out = addrspacecast %mpz* %in to %mpz addrspace(1)*
  ret %mpz addrspace(1)* %out
}

define %floating addrspace(1)* @addrspace_0_to_1_s_floatings(%floating* %in) {
  %out = addrspacecast %floating* %in to %floating addrspace(1)*
  ret %floating addrspace(1)* %out
}

define %map addrspace(1)* @addrspace_0_to_1_s_maps(%map* %in) {
  %out = addrspacecast %map* %in to %map addrspace(1)*
  ret %map addrspace(1)* %out
}

define %list addrspace(1)* @addrspace_0_to_1_s_lists(%list* %in) {
  %out = addrspacecast %list* %in to %list addrspace(1)*
  ret %list addrspace(1)* %out
}

define %set addrspace(1)* @addrspace_0_to_1_s_sets(%set* %in) {
  %out = addrspacecast %set* %in to %set addrspace(1)*
  ret %set addrspace(1)* %out
}

define %string addrspace(1)* @addrspace_0_to_1_s_strings(%string* %in) {
  %out = addrspacecast %string* %in to %string addrspace(1)*
  ret %string addrspace(1)* %out
}

define i8 addrspace(1)* @addrspace_0_to_1_i8(i8* %in) {
  %out = addrspacecast i8* %in to i8 addrspace(1)*
  ret i8 addrspace(1)* %out
}

define i1 addrspace(1)* @addrspace_0_to_1_i1(i1* %in) {
  %out = addrspacecast i1* %in to i1 addrspace(1)*
  ret i1 addrspace(1)* %out
}

define i32 addrspace(1)* @addrspace_0_to_1_i32(i32* %in) {
  %out = addrspacecast i32* %in to i32 addrspace(1)*
  ret i32 addrspace(1)* %out
}

define i64 addrspace(1)* @addrspace_0_to_1_i64(i64* %in) {
  %out = addrspacecast i64* %in to i64 addrspace(1)*
  ret i64 addrspace(1)* %out
}

define i8* @addrspace_1_to_0_i8(i8 addrspace(1)* %in) {
  %out = addrspacecast i8 addrspace(1)* %in to i8*
  ret i8* %out
}

define %block addrspace(1)* @inttoptr_i64.p1s_blocks(i64 %in) {
  %out = inttoptr i64 %in to %block addrspace(1)*
  ret %block addrspace(1)* %out
}

define i64 @ptrtoint_i64.p1s_blocks(%block addrspace(1)* %in) {
  %out = ptrtoint %block addrspace(1)* %in to i64
  ret i64 %out
}
