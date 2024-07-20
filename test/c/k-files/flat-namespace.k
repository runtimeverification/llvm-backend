module FLAT-NAMESPACE-SYNTAX
  imports BYTES-SYNTAX

  syntax Foo ::= foo()      [symbol(foo)]
               | bar(Bytes) [symbol(bar)]
endmodule

module FLAT-NAMESPACE
  imports DOMAINS
  imports BYTES
  imports FLAT-NAMESPACE-SYNTAX

  rule foo() => bar(Int2Bytes(10, 3489, BE))
endmodule
