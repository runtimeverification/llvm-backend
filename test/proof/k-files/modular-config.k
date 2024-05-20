module MODULAR-CONFIG-SYNTAX

  imports INT-SYNTAX
  imports STRING-SYNTAX

  syntax KItem ::= "#exec"

endmodule

module FOO-CELL

  imports INT-SYNTAX

  configuration
    <foo>
      <foo-p1> 0 </foo-p1>
      <foo-p2> 0 </foo-p2>
      <foo-p3> 0 </foo-p3>
    </foo>

endmodule

module BAR-CELL

  imports INT-SYNTAX

  configuration
    <bar>
      <bar-p1> 0 </bar-p1>
      <bar-p2> 0 </bar-p2>
      <bar-p3> 0 </bar-p3>
    </bar>

endmodule

module MODULAR-CONFIG
  imports INT
  imports STRING
  imports FOO-CELL
  imports BAR-CELL
  imports MODULAR-CONFIG-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <foo/>
    <bar/>

  rule [exec-rl]:
    <k> #exec => .K </k>
    <foo-p1> _ => 1 </foo-p1>
    <bar-p1> _ => 1 </bar-p1>

endmodule
