module C
  imports BOOL

  syntax Bool ::= foo() [function, klabel(foo), symbol]
                | bar() [function, klabel(bar), symbol]
                | baz() [function, klabel(baz), symbol]

  rule foo() => true
  rule bar() => false
  rule baz() => foo() andBool bar()
endmodule
