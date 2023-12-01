module SORTS
  imports DOMAINS

  syntax Int ::= func() [function, label(func), symbol]

  syntax Foo ::= foo() [label(foo), symbol]
  syntax Bar ::= Foo
               | bar() [label(bar), symbol]

  rule func() => 0
  rule foo() => bar()
endmodule
