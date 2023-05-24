module HANDLE-BYTES
  imports BYTES

  syntax Bytes ::= foo(Bytes) [function, klabel(foo), symbol]
  rule foo(B) => B
endmodule
