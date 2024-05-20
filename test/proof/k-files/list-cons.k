module LIST-CONS-SYNTAX

  syntax OpCode ::= "load" | "store" | "noOp"
  syntax OpCodes ::= ".OpCodes" | OpCode ";" OpCodes

  syntax OpCodes ::= #revOps    ( OpCodes )           [function]
                   | #revOpsAux ( OpCodes , OpCodes ) [function]
endmodule

module LIST-CONS

  imports LIST-CONS-SYNTAX

  rule [rev-rule] : #revOps(OPS) => #revOpsAux(OPS, .OpCodes)

  rule [rev-aux-base] : #revOpsAux( .OpCodes , OPS' ) => OPS'
  rule [rev-aux-rec] :  #revOpsAux( OP ; OPS , OPS' ) => #revOpsAux( OPS , OP ; OPS' )

endmodule
