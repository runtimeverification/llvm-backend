module TEST
  imports INT

  configuration <k> $PGM:K </k> <exit-code exit=""> 0 </exit-code>

  syntax KItem ::= foo() | bar() [function]
  rule foo() => bar()
endmodule
