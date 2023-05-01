module TEST
  imports INT
  imports MAP

  syntax KItem ::= val(Int, Int) [symbol]
  syntax KItem ::= foo(Int, Map) [function]
  syntax KItem ::= bar(KItem, K) [symbol]
  syntax KItem ::= done(Int, Map)
  syntax KItem ::= baz() [symbol]

  rule foo(_, _) => done(2, .Map) [owise]
  rule foo(A, val(A, 1) |-> val(B, 0) B |-> C M) => done(C, M)
  rule foo(A, val(A, 0) |-> val(B, 0) B |-> C) => done(C, .Map)
  rule foo(A, val(A, 0) |-> B) => done(B, .Map)
  rule foo(A, A |-> 5) => done(1, .Map)
  rule foo(A, val(A, 1) |-> B C) => done(B, C)
  rule foo(_, .Map) => done(0, .Map)
endmodule
