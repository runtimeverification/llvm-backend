module BINARY-SIMPLIFY
  imports INT

  syntax Foo ::= bar(Int)      [klabel(bar), symbol]
               | foo(Int, Int) [function, klabel(foo), symbol]

  rule foo(A, 0) => bar(A)
  rule foo(A, B) => foo(A +Int 1, absInt(B) -Int 1) [owise]
endmodule
