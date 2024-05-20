module TWO-COUNTERS-SYNTAX

  imports INT-SYNTAX

  syntax State ::= state(Int, Int)

endmodule

module TWO-COUNTERS

  imports INT
  imports TWO-COUNTERS-SYNTAX

  rule [count-step] : state(M, N) => state((M -Int 1), (N +Int M))
    requires M >=Int 1

endmodule
