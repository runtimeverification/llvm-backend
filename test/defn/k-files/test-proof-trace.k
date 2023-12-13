module TEST-PROOF-TRACE-SYNTAX
  syntax Foo ::= a() | b() | c()
endmodule

module TEST-PROOF-TRACE
  imports TEST-PROOF-TRACE-SYNTAX
  rule a() => b()
  rule b() => c()
endmodule
