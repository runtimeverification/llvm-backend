module TEST
  imports STRING

  syntax TokenSort [token]
  syntax KItem ::= #parseKORE(String) [hook(KREFLECTION.parseKORE), function]

  configuration <T> <k> $PGM:KItem </k> </T>

  syntax KItem ::= bar(Foo) [symbol]
  syntax Foo ::= foo() [symbol]
endmodule
