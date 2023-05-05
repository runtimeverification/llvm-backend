module TEST
  imports SET
  imports INT
  imports BOOL

  configuration <k> $PGM:Int </k> <env> .Set </env>

  syntax KItem ::= pair(K, K)
  syntax Set ::= tclosure(Set) [function]

  rule <k> 1 => .K </k> <env> _ => tclosure(SetItem(pair(1, 2)) SetItem(pair(2, 3)) SetItem(pair(3, 4))) </env>
  rule tclosure(R) => R [owise]
  rule tclosure(SetItem(pair(A, B)) SetItem(pair(B, C)) R) => tclosure(SetItem(pair(A, B)) SetItem(pair(B, C)) SetItem(pair(A, C)) R) requires notBool pair(A, C) in R

endmodule
