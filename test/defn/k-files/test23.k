module TEST
  imports INT
  imports MAP
  imports STRING

  syntax Int ::= bar(Map) [function]
  syntax Int ::= foo(Map, Int) [function]

  rule 2 => bar(1 |-> 2 3 |-> 4)
  rule bar(M) => maxInt(foo(M, 2), foo(M, 4))
  rule foo(_, _) => 1 [owise]
  rule foo(A |-> B M, B) => 0
endmodule
