module EXIT-CELL-SYNTAX

  imports INT-SYNTAX
  imports STRING-SYNTAX

  syntax KItem ::= #exec(Int)

endmodule

module EXIT-CELL

  imports INT
  imports STRING
  imports EXIT-CELL-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <status-code exit=""> 1 </status-code>

  rule [exec-rl]:
    <k> #exec(I:Int) => .K </k>
    <status-code> _ => I </status-code>

endmodule
