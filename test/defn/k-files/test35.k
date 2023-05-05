module TEST
  imports LIST
  imports INT

  syntax List ::= foo() [function]
  rule foo() => ListItem(0)
endmodule
