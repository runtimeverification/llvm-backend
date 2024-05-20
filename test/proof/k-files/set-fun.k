module SET-FUN-SYNTAX

  imports COLLECTIONS

  syntax Key ::= "a" | "b" | "c"

  syntax Bool ::= #hasKey( Key, Set ) [function]

endmodule

module SET-FUN

  imports SET-FUN-SYNTAX

  rule [has-key] : #hasKey(K, S) => SetItem(K) in S

endmodule
