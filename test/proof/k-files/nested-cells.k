module NESTED-CELLS-SYNTAX

  imports INT-SYNTAX
  imports STRING-SYNTAX

  syntax KItem ::= "#exec"

endmodule

module NESTED-CELLS

  imports INT
  imports STRING
  imports NESTED-CELLS-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <person>
      <name> "" </name>
      <id>
        <p1> 0 </p1>
        <p2> 0 </p2>
      </id>
      <age>  0 </age>
    </person>

  rule [exec-rl]:
    <k> #exec => .K </k>
    <name> _ => "Foo Bar" </name>
    <p1>   _ => 1         </p1>
    <p2>   _ => 2         </p2>
    <age>  _ => 10        </age>

endmodule
