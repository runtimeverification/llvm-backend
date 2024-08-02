module DECREMENT-SYNTAX

  syntax Nat ::= "0" | s(Nat)

endmodule

module DECREMENT

  imports DECREMENT-SYNTAX

  rule [decrement] : s(N:Nat) => N

endmodule
