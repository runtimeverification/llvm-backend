module MAP-FUN-SYNTAX

  imports MAP
  imports INT

  syntax KItem ::= select(Map, KItem)
  syntax Map ::= merge(Map, Map) [function, total]

endmodule

module MAP-FUN

  imports MAP-FUN-SYNTAX

  // Select by key
  rule [select-rule] : select(Key |-> Val _:Map, Key) => Val

  rule [merge-rule] : merge(M1, M2) => M1 M2

endmodule
