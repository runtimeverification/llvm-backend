module TEST
  imports BOOL
  imports INT

  syntax KItem ::= foo() [function]

  configuration <k> $PGM:K </k>

  rule true => foo()

  rule foo() => !I:Int
endmodule
