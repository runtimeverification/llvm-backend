module LIST-FACTORY-SYNTAX

  imports DOMAINS-SYNTAX

  syntax OpCode ::= "load" | "store" | "noOp"
  syntax OpCodes ::= List{OpCode, ";"}

  syntax OpCodes ::= #revOps    ( OpCodes )           [function]
                   | #revOpsAux ( OpCodes , OpCodes ) [function]

endmodule

module LIST-FACTORY

  imports DOMAINS
  imports LIST-FACTORY-SYNTAX

  rule [rev-rule] : #revOps(OPS) => #revOpsAux(OPS, .OpCodes)

  rule [rev-aux-base] : #revOpsAux( .OpCodes , OPS' ) => OPS'
  rule [rev-aux-rec] :  #revOpsAux( OP ; OPS , OPS' ) => #revOpsAux( OPS , OP ; OPS' )

endmodule
