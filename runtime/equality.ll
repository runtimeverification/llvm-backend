target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%map = type { i64, i8 * } ; im::ordmap::OrdMap
%set = type { i64, i8 * } ; im::ordset::OrdSet
%list = type { i64, [7 x i64] } ; im::vector:Vector
; %map = type { i64, i8 *, i8 * } ; im::hashmap::HashMap
; %set = type { i8 *, i8 *, i64 } ; im::hashset::HashSet
;%list = type { i64, i64, i8 *, i8 *, i8 *, i8 *, i8 * } ; im::vector::Vector
%mpz = type { i32, i32, i64 * } ; mpz_t
%layoutitem = type { i64, i16 }
%layout = type { i8, %layoutitem* }

declare i32 @memcmp(i8* %ptr1, i8* %ptr2, i64 %num)
declare void @abort() #0
declare %layout* @getLayoutData(i16)

declare i1 @hook_MAP_eq(%map*, %map*)
declare i1 @hook_LIST_eq(%list*, %list*)
declare i1 @hook_SET_eq(%set*, %set*)
declare i1 @hook_INT_eq(%mpz*, %mpz*)

@HDR_MASK = private constant i64 -246290604621825 ; 0xffff1fffffffffff, cf header.h

define i1 @hook_KEQUAL_eq(%block* %arg1, %block* %arg2) {
entry:
  %arg1intptr = ptrtoint %block* %arg1 to i64
  %arg2intptr = ptrtoint %block* %arg2 to i64
  %arg1leastbit = trunc i64 %arg1intptr to i1
  %arg2leastbit = trunc i64 %arg2intptr to i1
  %eq = icmp eq i1 %arg1leastbit, %arg2leastbit
  br i1 %eq, label %getTag, label %exit
getTag:
  br i1 %arg1leastbit, label %constant, label %block
constant:
  %eqconstant = icmp eq i64 %arg1intptr, %arg2intptr
  br label %exit
block:
  %arg1hdrptr = getelementptr inbounds %block, %block* %arg1, i64 0, i32 0, i32 0
  %arg2hdrptr = getelementptr inbounds %block, %block* %arg2, i64 0, i32 0, i32 0
  %arg1hdr = load i64, i64* %arg1hdrptr
  %arg2hdr = load i64, i64* %arg2hdrptr
  %mask = load i64, i64* @HDR_MASK
  %arg1len = and i64 %arg1hdr, %mask
  %arg2len = and i64 %arg2hdr, %mask
  %eqblock = icmp eq i64 %arg1len, %arg2len
  br i1 %eqblock, label %getChildren, label %exit
getChildren:
  %arglayout = lshr i64 %arg1hdr, 48
  %isString = icmp eq i64 %arglayout, 0
  br i1 %isString, label %eqString, label %compareChildren
eqString:
  %str1ptrlong = getelementptr inbounds %block, %block* %arg1, i64 0, i32 1, i64 0
  %str2ptrlong = getelementptr inbounds %block, %block* %arg2, i64 0, i32 1, i64 0
  %str1ptr = bitcast i64** %str1ptrlong to i8*
  %str2ptr = bitcast i64** %str2ptrlong to i8*
  %retval = call i32 @memcmp(i8* %str1ptr, i8* %str2ptr, i64 %arg1len)
  %eqcontents = icmp eq i32 %retval, 0
  br label %exit
compareChildren:
  %arglayoutshort = trunc i64 %arglayout to i16
  %layoutPtr = call %layout* @getLayoutData(i16 %arglayoutshort)
  %layoutData = load %layout, %layout* %layoutPtr
  %length = extractvalue %layout %layoutData, 0
  %children = extractvalue %layout %layoutData, 1
  br label %loop
loop:
  %counter = phi i8 [ %length, %compareChildren ], [ %sub1, %compareMap ], [ %sub1, %compareList ], [ %sub1, %compareSet ], [ %sub1, %compareInt ], [ %sub1, %compareBool ], [ %sub1, %compareSymbol ]
  %index = sub i8 %length, %counter
  %indexlong = zext i8 %index to i64
  %sub1 = sub i8 %counter, 1
  %finished = icmp eq i8 %counter, 0
  br i1 %finished, label %exit, label %compareChild
compareChild:
  %offsetPtr = getelementptr %layoutitem, %layoutitem* %children, i64 %indexlong, i32 0
  %offset = load i64, i64* %offsetPtr
  %kindPtr = getelementptr %layoutitem, %layoutitem* %children, i64 %indexlong, i32 1
  %kind = load i16, i16* %kindPtr
  %child1intptr = add i64 %arg1intptr, %offset
  %child2intptr = add i64 %arg2intptr, %offset
  switch i16 %kind, label %stuck [ i16 1, label %compareMap
                                   i16 2, label %compareList
				   i16 3, label %compareSet
				   i16 4, label %compareInt
				   i16 5, label %stuck
				   i16 6, label %stuck
				   i16 7, label %compareBool
				   i16 8, label %compareSymbol
				   i16 9, label %stuck ]
compareMap:
  %map1ptr = inttoptr i64 %child1intptr to %map*
  %map2ptr = inttoptr i64 %child2intptr to %map*
  %comparedMap = call i1 @hook_MAP_eq(%map* %map1ptr, %map* %map2ptr)
  br i1 %comparedMap, label %loop, label %exit
compareList:
  %list1ptr = inttoptr i64 %child1intptr to %list*
  %list2ptr = inttoptr i64 %child2intptr to %list*
  %comparedList = call i1 @hook_LIST_eq(%list* %list1ptr, %list* %list2ptr)
  br i1 %comparedList, label %loop, label %exit
compareSet:
  %set1ptr = inttoptr i64 %child1intptr to %set*
  %set2ptr = inttoptr i64 %child2intptr to %set*
  %comparedSet = call i1 @hook_SET_eq(%set* %set1ptr, %set* %set2ptr)
  br i1 %comparedSet, label %loop, label %exit
compareInt:
  %int1ptrptr = inttoptr i64 %child1intptr to %mpz**
  %int2ptrptr = inttoptr i64 %child2intptr to %mpz**
  %int1ptr = load %mpz*, %mpz** %int1ptrptr
  %int2ptr = load %mpz*, %mpz** %int2ptrptr
  %comparedInt = call i1 @hook_INT_eq(%mpz* %int1ptr, %mpz* %int2ptr)
  br i1 %comparedInt, label %loop, label %exit
compareBool:
  %bool1ptr = inttoptr i64 %child1intptr to i1*
  %bool2ptr = inttoptr i64 %child2intptr to i1*
  %bool1 = load i1, i1* %bool1ptr
  %bool2 = load i1, i1* %bool2ptr
  %comparedBool = icmp eq i1 %bool1, %bool2
  br i1 %comparedBool, label %loop, label %exit
compareSymbol:
  %child1ptrptr = inttoptr i64 %child1intptr to %block**
  %child2ptrptr = inttoptr i64 %child2intptr to %block**
  %child1ptr = load %block*, %block** %child1ptrptr
  %child2ptr = load %block*, %block** %child2ptrptr
  %comparedSymbol = call i1 @hook_KEQUAL_eq(%block* %child1ptr, %block* %child2ptr)
  br i1 %comparedSymbol, label %loop, label %exit
exit:
  %phi = phi i1 [ 0, %entry ], [ %eqconstant, %constant ], [ 0, %block ], [ %eqcontents, %eqString ], [ 1, %loop ], [ 0, %compareMap ], [ 0, %compareList ], [ 0, %compareSet ], [ 0, %compareInt ], [ 0, %compareBool ], [ 0, %compareSymbol ]
  ret i1 %phi
stuck:
  call void @abort()
  unreachable
}

define i1 @hook_KEQUAL_ne(%block* %arg1, %block* %arg2) {
  %eq = call i1 @hook_KEQUAL_eq(%block* %arg1, %block* %arg2)
  %ne = xor i1 %eq, 1
  ret i1 %ne
}

attributes #0 = { noreturn }
