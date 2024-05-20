module IS-ZERO-SYNTAX

  imports INT-SYNTAX
  imports BOOL-SYNTAX

  syntax Bool ::= isZero(Int) [function, total]

endmodule

module IS-ZERO

  imports INT
  imports BOOL
  imports IS-ZERO-SYNTAX

  rule [zero]     : isZero(0)     => true
  rule [non-zero] : isZero(_:Int) => false [owise]

endmodule
