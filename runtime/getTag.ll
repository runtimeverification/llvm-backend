target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

%blockheader = type { i64 } 
%block = type { %blockheader, [0 x ptr] } ; 16-bit layout, 8-bit length, 32-bit tag, children

define i32 @get_tag(ptr %arg) {
  %intptr = ptrtoint ptr %arg to i64
  %isConstant = trunc i64 %intptr to i1
  br i1 %isConstant, label %constant, label %block
constant:
  %taglong = lshr i64 %intptr, 32
  br label %exit
block:
  %hdrptr = getelementptr inbounds %block, ptr %arg, i64 0, i32 0, i32 0
  %hdr = load i64, ptr %hdrptr
  %layout = lshr i64 %hdr, @LAYOUT_OFFSET@
  %isstring = icmp eq i64 %layout, 0
  %tagorstring = select i1 %isstring, i64 -1, i64 %hdr
  br label %exit
exit:
  %phi = phi i64 [ %tagorstring, %block ], [ %taglong, %constant ]
  %tag = trunc i64 %phi to i32
  ret i32 %tag
}
