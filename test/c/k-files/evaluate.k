module EVALUATE
  imports INT

  syntax Foo ::= foo(Int) [label(foo), symbol]
  syntax Foo ::= f(Foo) [function, total, label(f), symbol]
  rule f(foo(I)) => foo(I +Int 100)
endmodule
