module LIST-ASSOC-SYNTAX

  syntax OpCode ::= "load" | "store" | "noOp"
  syntax OpCodes ::= OpCode
                   | OpCodes ";" OpCodes [right]

  syntax OpCodes ::= #revOps    ( OpCodes )           [function]
                   | #revOpsAux ( OpCodes , OpCodes ) [function]

endmodule

module LIST-ASSOC

  imports LIST-ASSOC-SYNTAX

  // Note that this adds an extra noOp at the end, but that's irrelevent for the test
  rule [rev-rule] : #revOps(OPS) => #revOpsAux(OPS, noOp)

  rule [rev-aux-base] : #revOpsAux( OP:OpCode , OPS' ) => OP ; OPS'
  rule [rev-aux-rec] :  #revOpsAux( OP ; OPS  , OPS' ) => #revOpsAux( OPS , OP ; OPS' )

endmodule
