module KITEM
  imports INT

  syntax KItem ::= foo() [function, klabel(foo), symbol]
                 | bar() [klabel(bar), symbol]

  rule foo() => bar()
endmodule
