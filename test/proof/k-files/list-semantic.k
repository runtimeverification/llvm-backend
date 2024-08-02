module LIST-SEMANTIC-SYNTAX

  imports LIST

  syntax OpCode ::= "load" | "store" | "noOp"

  syntax List ::= #revOps    ( List )        [function]
                | #revOpsAux ( List , List ) [function]

endmodule

module LIST-SEMANTIC

  imports LIST-SEMANTIC-SYNTAX

  rule [rev-rule] : #revOps(OPS) => #revOpsAux(OPS, .List)

  rule [rev-aux-base] : #revOpsAux( .List , OPS' ) => OPS'
  rule [rev-aux-rec] :  #revOpsAux( ListItem(OP) OPS , OPS' ) => #revOpsAux( OPS , ListItem(OP) OPS' )

endmodule
