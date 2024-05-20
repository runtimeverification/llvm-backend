module LET-SYNTAX

  imports INT-SYNTAX

  syntax Int ::= exp(Int) [function]

endmodule

module LET

  imports INT
  imports LET-SYNTAX

  rule [exp-rl]:
    exp(X:Int) =>
      #let S = X *Int X #in
      #let D = 2 *Int S #in
        D -Int X

endmodule
