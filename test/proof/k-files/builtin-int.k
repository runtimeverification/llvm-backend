module BUILTIN-INT-SYNTAX

  imports INT-SYNTAX

  syntax Foo ::= foo(Int)
               | succ(Foo)

endmodule

module BUILTIN-INT

  imports INT
  imports BUILTIN-INT-SYNTAX

  rule succ(foo(X:Int)) => foo(X +Int 1)

endmodule
