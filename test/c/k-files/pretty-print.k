module PRETTY-PRINT
  imports INT

  syntax Foo ::= "#foo" "[" Int "]" [format(%1 %2%n%i%3%n%d%4), klabel(foo), symbol]
endmodule
