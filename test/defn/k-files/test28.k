module TEST
  imports STRING
  imports K-IO

  syntax KItem ::= mkstemp(String)

  rule mkstemp(Template) => #mkstemp(Template)
endmodule
