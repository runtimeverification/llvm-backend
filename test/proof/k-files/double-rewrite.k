module DOUBLE-REWRITE-SYNTAX

  syntax Foo ::= FooA() | FooB() | FooC()

endmodule

module DOUBLE-REWRITE

  imports DOUBLE-REWRITE-SYNTAX

  rule [a-to-b] : FooA() => FooB()
  rule [b-to-c] : FooB() => FooC()

endmodule
