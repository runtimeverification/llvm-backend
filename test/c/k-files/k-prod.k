module K-PROD
  imports DOMAINS

  syntax KItem ::= bar() [klabel(bar), symbol]

  syntax K ::= foo() [function, klabel(foo), symbol]
  rule foo() => bar()
endmodule
