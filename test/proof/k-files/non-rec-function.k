module NON-REC-FUNCTION-SYNTAX

  syntax Foo ::= "a"
               | bar(Foo)
               | baz(Foo)
               | id(Foo) [function, total]

endmodule

module NON-REC-FUNCTION

  imports NON-REC-FUNCTION-SYNTAX

  rule [id-rule]: id(X:Foo) => X
  rule [bar-rule]: bar(baz(X:Foo)) => id(id(bar(X)))

endmodule
