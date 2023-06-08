module SIMPLIFY
  imports INT

  syntax Int ::= foo() [function, klabel(foo), symbol]
               | bar() [function, klabel(bar), symbol]

  rule foo() => 328
  rule bar() => 562
endmodule
