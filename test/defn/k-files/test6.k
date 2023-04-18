module TEST
  imports INT
  imports SET

  syntax KItem ::= done(Int, Set)
  syntax KItem ::= val(Int, Int) [symbol]
  syntax KItem ::= foo(Int, Int, Set) [function]

  rule foo(A, _, SetItem(A)) => done(1, .Set)
  rule foo(A, B, SetItem(val(A, 0)) SetItem(B)) => done(3, .Set)
  rule foo(A, _, SetItem(val(A, 0))) => done(2, .Set)
  rule foo(_, _, _) => done(6, .Set) [owise]
  rule foo(A, _, SetItem(val(A, 1)) B) => done(4, B)
  rule foo(_, _, .Set) => done(0, .Set)
  rule foo(A, B, SetItem(val(A, 1)) SetItem(B) S) => done(5, S)
endmodule
