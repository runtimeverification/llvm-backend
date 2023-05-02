module TEST
  imports INT

  syntax Int ::= foo(Int) [function]

  rule foo(0) => 0
endmodule
