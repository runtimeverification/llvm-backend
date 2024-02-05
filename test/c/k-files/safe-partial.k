module SAFE-PARTIAL
  imports INT

  syntax Int ::= foo(Int) [function, klabel(foo), symbol]
  rule foo(0) => 0
endmodule
