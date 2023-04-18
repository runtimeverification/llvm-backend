module TEST
  imports INT

  syntax Bar ::= bar()
  syntax Foo ::= foo()
  syntax Foo ::= Bar

  rule bar() => 0
  rule foo() => bar()
endmodule
