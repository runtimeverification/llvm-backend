module EVALUATE
  imports INT
  imports BOOL

  syntax Foo ::= foo(Int) [label(foo), symbol]
               | bar(Int) [label(bar), symbol]

  syntax Foo ::= f(Foo, Foo) [function, label(f), symbol]
  rule f(foo(A), bar(B)) => foo(A +Int B)

  syntax Int ::= baz() [function, label(baz), symbol]
  syntax Bool ::= qux(Int) [function, label(qux), symbol]

  rule baz() => 78
  rule qux(I) => I ==Int 34
endmodule
