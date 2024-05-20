module CONCURRENT-COUNTERS-SYNTAX

  imports INT-SYNTAX

  syntax State ::= state(Int, Int)

endmodule

module CONCURRENT-COUNTERS

  imports INT
  imports CONCURRENT-COUNTERS-SYNTAX

  rule [count-rule1] : state(M, N) => state((M -Int 1), (N +Int M))
    requires M >=Int 3 [priority(50)]
 
  rule [count-rule2] : state(M, N) => state((M -Int 1), (N -Int 1))
    requires M >=Int 1 [priority(60)]

endmodule
