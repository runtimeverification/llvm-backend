module TEST
  imports INT
  imports BOOL
  imports STRING

  syntax Int ::= foo(Bool) [function]

  rule foo(_) => 1 [owise]
  rule foo(false) => 0
endmodule
