module DIV-BY-ZERO
  imports INT

  syntax Int ::= foo(Int, Int) [function, klabel(foo), symbol]

  rule foo(X, Y) => X requires X /Int Y ==Int 1
endmodule
