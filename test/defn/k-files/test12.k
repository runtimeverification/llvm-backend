module TEST
  imports INT

  syntax KItem ::= "foo"
                 | "bar"
                 | "baz"

  rule foo #Or bar #Or baz => 0
endmodule
