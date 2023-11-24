module EVALUATE
  imports INT

  syntax Foo ::= foo(Int) [label(foo), symbol]
               | bar(Int) [label(bar), symbol]

  syntax Foo ::= f(Foo, Foo) [function, label(f), symbol]
  rule f(foo(A), bar(B)) => foo(A +Int B)
endmodule
