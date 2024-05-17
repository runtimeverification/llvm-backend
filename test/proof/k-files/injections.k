module INJECTIONS-SYNTAX

  imports INT-SYNTAX
  imports BOOL-SYNTAX

  syntax Foo ::= foo(Int)
  syntax Bar ::= bar(Int)
  syntax Oof ::= oof(Int)

  syntax Foo ::= Bar
  syntax Bar ::= Oof

  syntax Foo ::= f(Foo)
               | g(Foo)

endmodule

module INJECTIONS

  imports INT
  imports BOOL
  imports INJECTIONS-SYNTAX

  rule [f-rl] : f(_:Bar) => g(oof(1))
  rule [g-rl] : g(_:Bar) => g(foo(1))

endmodule
