module SINGLE-REWRITE-SYNTAX

  syntax Foo ::= FooA() | FooB()

endmodule

module SINGLE-REWRITE

  imports SINGLE-REWRITE-SYNTAX

  rule [a-to-b]: FooA() => FooB()

endmodule
