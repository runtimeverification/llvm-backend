module SIMPLIFY
  imports INT
  imports BOOL

  syntax Int ::= foo() [function, klabel(foo), symbol]
               | bar() [function, klabel(bar), symbol]

  syntax Bool ::= baz() [function, klabel(baz), symbol]
                | qux() [function, klabel(qux), symbol]

  rule foo() => 328
  rule bar() => 562

  rule baz() => true
  rule qux() => false
endmodule
