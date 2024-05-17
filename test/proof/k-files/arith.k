module ARITH-SYNTAX

  imports DOMAINS

  syntax Exp ::= add(Exp, Exp) [strict(1, 2)]
  syntax Exp ::= minus(Exp, Exp) [strict(2, 1)]
  syntax Exp ::= well(Exp, Exp, Exp)
  syntax Exp ::= Int

endmodule

module ARITH

  imports ARITH-SYNTAX

  syntax KResult ::= Int | Bool

  rule [add-rule] : add(A:Int, B:Int) => A +Int B
  rule [minus-rule] : minus(A:Int, B:Int) => A -Int B

  context well(HOLE:Exp, _:Exp, _:Exp)
  context well(_:Int, _:Exp, HOLE:Exp)
  context well(_:Int, HOLE:Exp, _:Int)

  rule [well-rule] : well(X:Int, Y:Int, Z:Int) => X +Int Y -Int Z

endmodule
