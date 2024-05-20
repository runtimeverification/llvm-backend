module MEMO-FUNCTION-SYNTAX

  syntax Foo ::= "a" | "b"
  syntax Foo ::= next(Foo) [function, memo]

endmodule

module MEMO-FUNCTION

  imports MEMO-FUNCTION-SYNTAX

  rule [next-a] : next(a) => b
  rule [next-b] : next(b) => a

endmodule
