module MINT-ARITH-SYNTAX

  imports DOMAINS
  imports MINT-SYNTAX

  syntax MInt{8}
  syntax MInt{32}
  syntax MInt{64}
  syntax MInt{160}
  syntax MInt{256}

  syntax KItem ::= add(Exp, Exp, Int)
  syntax Exp ::= Int | MInt{8} | MInt{32} | MInt{64} | MInt{160} | MInt{256}

endmodule

module MINT-ARITH

  imports MINT-ARITH-SYNTAX
  imports MINT

  syntax KResult ::= MInt{8} | MInt{32} | MInt{64} | MInt{160} | MInt{256}

  context add(HOLE => toMInt8(HOLE),   _,                       W) requires W ==Int 8
  context add(_,                       HOLE => toMInt8(HOLE),   W) requires W ==Int 8
  context add(HOLE => toMInt32(HOLE),  _,                       W) requires W ==Int 32
  context add(_,                       HOLE => toMInt32(HOLE),  W) requires W ==Int 32
  context add(HOLE => toMInt64(HOLE),  _,                       W) requires W ==Int 64
  context add(_,                       HOLE => toMInt64(HOLE),  W) requires W ==Int 64
  context add(HOLE => toMInt160(HOLE), _,                       W) requires W ==Int 160
  context add(_,                       HOLE => toMInt160(HOLE), W) requires W ==Int 160
  context add(HOLE => toMInt256(HOLE), _,                       W) requires W ==Int 256
  context add(_,                       HOLE => toMInt256(HOLE), W) requires W ==Int 256

  rule add(A:MInt{8},   B:MInt{8},   _) => A +MInt B
  rule add(A:MInt{32},  B:MInt{32},  _) => A +MInt B
  rule add(A:MInt{64},  B:MInt{64},  _) => A +MInt B
  rule add(A:MInt{160}, B:MInt{160}, _) => A +MInt B
  rule add(A:MInt{256}, B:MInt{256}, _) => A +MInt B

  syntax Exp8 ::= Int | MInt{8}
  syntax Exp ::= toMInt8(Exp8) | Exp8
  rule toMInt8(I:Int => Int2MInt(I))

  syntax Exp32 ::= Int | MInt{32}
  syntax Exp ::= toMInt32(Exp32) | Exp32
  rule toMInt32(I:Int => Int2MInt(I))

  syntax Exp64 ::= Int | MInt{64}
  syntax Exp ::= toMInt64(Exp64) | Exp64
  rule toMInt64(I:Int => Int2MInt(I))

  syntax Exp160 ::= Int | MInt{160}
  syntax Exp ::= toMInt160(Exp160) | Exp160
  rule toMInt160(I:Int => Int2MInt(I))

  syntax Exp256 ::= Int | MInt{256}
  syntax Exp ::= toMInt256(Exp256) | Exp256
  rule toMInt256(I:Int => Int2MInt(I))

endmodule
