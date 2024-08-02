module TYPE-CAST-SYNTAX

  syntax Oof ::= "symbol"

  syntax Foo ::= Bar
  syntax Bar ::= Oof

  syntax Foo ::= f(Foo)
               | g(Oof)
               | h()

endmodule

module TYPE-CAST

  imports TYPE-CAST-SYNTAX

  rule [f-rl] : f(X:Foo) => g({X}:>Oof)
  rule [g-rl] : g(_:Oof) => h()

endmodule
