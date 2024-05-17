module ASSOC-FUNCTION-SYNTAX

  syntax Foo ::= "a" | "b" | "c" | "d"
  syntax Foo ::= Foo "+left"  Foo [function, left]
               | Foo "+right" Foo [function, right]

  syntax KItem ::= #next(Foo)

endmodule

module ASSOC-FUNCTION

  imports ASSOC-FUNCTION-SYNTAX

  rule [left]  : _X:Foo +left  Y:Foo => Y
  rule [right] : X:Foo +right _Y:Foo => X

  rule #next(Foo) => Foo

endmodule
