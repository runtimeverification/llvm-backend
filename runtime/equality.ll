target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

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
  %arg1len = and i64 %arg1hdr, @HDR_MASK@
  %arg2len = and i64 %arg2hdr, @HDR_MASK@
  %eqblock = icmp eq i64 %arg1len, %arg2len
  br i1 %eqblock, label %getChildren, label %exit
getChildren:
  %arglayout = lshr i64 %arg1hdr, @LAYOUT_OFFSET@
  %isString = icmp eq i64 %arglayout, 0
  br i1 %isString, label %eqString, label %compareChildren
eqString:
  %eqcontents = call i1 @hook_STRING_eq(%block* %arg1, %block* %arg2)
  br label %exit
compareChildren:
  %arglayoutshort = trunc i64 %arglayout to i16
  %layoutPtr = call %layout* @getLayoutData(i16 %arglayoutshort)
  %layoutData = load %layout, %layout* %layoutPtr
  %length = extractvalue %layout %layoutData, 0
  %children = extractvalue %layout %layoutData, 1
  br label %loop
loop:
  %counter = phi i8 [ %length, %compareChildren ], [ %sub1, %compareMap ], [ %sub1, %compareList ], [ %sub1, %compareSet ], [ %sub1, %compareInt ], [ %sub1, %compareFloat ], [ %sub1, %compareBool ], [ %sub1, %compareSymbol ], [ %sub1, %compareVariable ]
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
  switch i16 %kind, label %stuck [ i16 @MAP_LAYOUT@, label %compareMap
                                   i16 @LIST_LAYOUT@, label %compareList
				   i16 @SET_LAYOUT@, label %compareSet
				   i16 @INT_LAYOUT@, label %compareInt
				   i16 @FLOAT_LAYOUT@, label %compareFloat
				   i16 @STRINGBUFFER_LAYOUT@, label %stuck
				   i16 @BOOL_LAYOUT@, label %compareBool
				   i16 @SYMBOL_LAYOUT@, label %compareSymbol
				   i16 @VARIABLE_LAYOUT@, label %compareVariable ]
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
compareFloat:
  %float1ptrptr = inttoptr i64 %child1intptr to %floating**
  %float2ptrptr = inttoptr i64 %child2intptr to %floating**
  %float1ptr = load %floating*, %floating** %float1ptrptr
  %float2ptr = load %floating*, %floating** %float2ptrptr
  %comparedFloat = call i1 @hook_FLOAT_trueeq(%floating* %float1ptr, %floating* %float2ptr)
  br i1 %comparedFloat, label %loop, label %exit
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
compareVariable:
  %var1ptrptr = inttoptr i64 %child1intptr to %block**
  %var2ptrptr = inttoptr i64 %child2intptr to %block**
  %var1ptr = load %block*, %block** %var1ptrptr
  %var2ptr = load %block*, %block** %var2ptrptr
  %comparedVar = call i1 @hook_STRING_eq(%block* %var1ptr, %block* %var2ptr)
  br i1 %comparedVar, label %loop, label %exit
exit:
  %phi = phi i1 [ 0, %entry ], [ %eqconstant, %constant ], [ 0, %block ], [ %eqcontents, %eqString ], [ 1, %loop ], [ 0, %compareMap ], [ 0, %compareList ], [ 0, %compareSet ], [ 0, %compareInt ], [ 0, %compareFloat ], [ 0, %compareBool ], [ 0, %compareSymbol ], [ 0, %compareVariable ]
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

define i1 @hook_BOOL_eq(i1 %arg1, i1 %arg2) #1 {
  %eq = icmp eq i1 %arg1, %arg2
  ret i1 %eq
}

attributes #0 = { noreturn }
attributes #1 = { alwaysinline }
