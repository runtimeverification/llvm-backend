module CELL-COLLECTION-SYNTAX

  imports INT-SYNTAX
  imports STRING-SYNTAX

  syntax KItem ::= "#exec"

endmodule

module CELL-COLLECTION

  imports INT
  imports STRING
  imports CELL-COLLECTION-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <messages>
      <message multiplicity="*" type="Map">
        <msgID> 0  </msgID>
        <value> 0  </value>
        <to>    "" </to>
      </message>
    </messages>

  rule [exec-rl]:
    <k> #exec => .K </k>
    <messages> _ => .Bag </messages>

endmodule
