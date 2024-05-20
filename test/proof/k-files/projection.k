module PROJECTION-SYNTAX

  syntax Foo ::= "a" | "b" | proj(Foo, Foo)

endmodule

module PROJECTION

  imports PROJECTION-SYNTAX

  rule [proj-rule]: proj(X:Foo, _:Foo) => X

endmodule
