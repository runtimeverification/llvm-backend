module TEST-SYNTAX
  syntax Expr ::= test()
endmodule

module TEST
  imports TEST-SYNTAX
  imports INT

  rule test() => +123

endmodule
