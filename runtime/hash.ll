target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children
%map = type { i8 *, i64 } ; immer::map
%set = type { i8 *, i64 } ; immer::set
%list = type { i64, i32, i8 *, i8 * } ; immer::vector
%mpz = type { i32, i32, i64 * } ; mpz_t
%floating = type { i64, { i64, i32, i64, i64 * } } ; exp, mpfr_t
%layoutitem = type { i64, i16 }
%layout = type { i8, %layoutitem* }

declare void @abort() #0
declare %layout* @getLayoutData(i16)

declare void @map_hash(%map*, i8*)
declare void @list_hash(%list*, i8*)
declare void @set_hash(%set*, i8*)
declare void @int_hash(%mpz*, i8*)
declare void @float_hash(%floating*, i8*)

declare i64 @__gmpz_get_si(%mpz*)

declare void @add_hash64(i8*, i64)
declare void @add_hash8(i8*, i8)

@hash_depth = thread_local global i32 0
@HASH_THRESHOLD = private constant i32 5

define i1 @hash_enter() {
  %depth = load i32, i32* @hash_depth
  %threshold = load i32, i32* @HASH_THRESHOLD
  %cmp = icmp ult i32 %depth, %threshold
  %plus1 = add i32 %depth, 1
  store i32 %plus1, i32* @hash_depth
  ret i1 %cmp
}

define void @hash_exit() {
  %depth = load i32, i32* @hash_depth
  %sub1 = sub i32 %depth, 1
  store i32 %sub1, i32* @hash_depth
  ret void
}

define void @k_hash(%block* %arg, i8* %hasher) {
  %notdone = call i1 @hash_enter()
  %done = xor i1 %notdone, 1
  br i1 %done, label %exit, label %entry
entry:
  %argintptr = ptrtoint %block* %arg to i64
  %argleastbit = trunc i64 %argintptr to i1
  br i1 %argleastbit, label %constant, label %block
constant:
  call void @add_hash64(i8* %hasher, i64 %argintptr)
  br label %exit
block:
  %arghdrptr = getelementptr inbounds %block, %block* %arg, i64 0, i32 0, i32 0
  %arghdr = load i64, i64* %arghdrptr
  %arglayout = lshr i64 %arghdr, @LAYOUT_OFFSET@
  %arghdrcanon = and i64 %arghdr, @HDR_MASK@
  %isString = icmp eq i64 %arglayout, 0
  br i1 %isString, label %hashString, label %hashChildren
hashString:
  call void @add_hash64(i8* %hasher, i64 %arghdrcanon)
  %strptrlong = getelementptr inbounds %block, %block* %arg, i64 0, i32 1, i64 0
  %strptr = bitcast i64** %strptrlong to i8*
  br label %stringLoop
stringLoop:
  %counter = phi i64 [ %arghdrcanon, %hashString ], [ %sub1, %compareByte ]
  %sub1 = sub i64 %counter, 1
  %index = sub i64 %arghdrcanon, %counter
  %finished = icmp eq i64 %counter, 0
  br i1 %finished, label %exit, label %compareByte
compareByte:
  %byteptr = getelementptr i8, i8* %strptr, i64 %index
  %byte = load i8, i8* %byteptr
  call void @add_hash8(i8* %hasher, i8 %byte)
  br label %stringLoop
hashChildren:
  call void @add_hash64(i8* %hasher, i64 %arghdrcanon)
  %arglayoutshort = trunc i64 %arglayout to i16
  %layoutPtr = call %layout* @getLayoutData(i16 %arglayoutshort)
  %layoutData = load %layout, %layout* %layoutPtr
  %length = extractvalue %layout %layoutData, 0
  %children = extractvalue %layout %layoutData, 1
  br label %childrenLoop
childrenLoop:
  %counter2 = phi i8 [ %length, %hashChildren ], [ %sub12, %hashMap ], [ %sub12, %hashList ], [ %sub12, %hashSet ], [ %sub12, %hashInt ], [ %sub12, %hashFloat ], [ %sub12, %hashBool ], [ %sub12, %hashSymbol ]
  %index2 = sub i8 %length, %counter2
  %indexlong2 = zext i8 %index2 to i64
  %sub12 = sub i8 %counter2, 1
  %finished2 = icmp eq i8 %counter2, 0
  br i1 %finished2, label %exit, label %hashChild
hashChild:
  %offsetPtr = getelementptr %layoutitem, %layoutitem* %children, i64 %indexlong2, i32 0
  %offset = load i64, i64* %offsetPtr
  %kindPtr = getelementptr %layoutitem, %layoutitem* %children, i64 %indexlong2, i32 1
  %kind = load i16, i16* %kindPtr
  %childintptr = add i64 %argintptr, %offset
  switch i16 %kind, label %stuck [ i16 @MAP_LAYOUT@, label %hashMap
                                   i16 @LIST_LAYOUT@, label %hashList
				   i16 @SET_LAYOUT@, label %hashSet
				   i16 @INT_LAYOUT@, label %hashInt
				   i16 @FLOAT_LAYOUT@, label %hashFloat
				   i16 @STRINGBUFFER_LAYOUT@, label %stuck
				   i16 @BOOL_LAYOUT@, label %hashBool
				   i16 @SYMBOL_LAYOUT@, label %hashSymbol
				   i16 @VARIABLE_LAYOUT@, label %hashSymbol ]
hashMap:
  %mapptr = inttoptr i64 %childintptr to %map*
  call void @map_hash(%map* %mapptr, i8* %hasher)
  br label %childrenLoop
hashList:
  %listptr = inttoptr i64 %childintptr to %list*
  call void @list_hash(%list* %listptr, i8* %hasher)
  br label %childrenLoop
hashSet:
  %setptr = inttoptr i64 %childintptr to %set*
  call void @set_hash(%set* %setptr, i8* %hasher)
  br label %childrenLoop
hashInt:
  %intptrptr = inttoptr i64 %childintptr to %mpz**
  %intptr = load %mpz*, %mpz** %intptrptr
  call void @int_hash(%mpz* %intptr, i8* %hasher)
  br label %childrenLoop
hashFloat:
  %floatptrptr = inttoptr i64 %childintptr to %floating**
  %floatptr = load %floating*, %floating** %floatptrptr
  call void @float_hash(%floating* %floatptr, i8* %hasher)
  br label %childrenLoop
hashBool:
  %boolptr = inttoptr i64 %childintptr to i1*
  %bool = load i1, i1* %boolptr
  %byte2 = zext i1 %bool to i8
  call void @add_hash8(i8* %hasher, i8 %byte2)
  br label %childrenLoop
hashSymbol:
  %childptrptr = inttoptr i64 %childintptr to %block**
  %childptr = load %block*, %block** %childptrptr
  call void @k_hash(%block* %childptr, i8* %hasher)
  br label %childrenLoop
exit:
  call void @hash_exit()
  ret void
stuck:
  call void @abort()
  unreachable
}

attributes #0 = { noreturn }
