module FRESH-GEN-SYNTAX

  imports INT-SYNTAX
  imports STRING-SYNTAX

  syntax KItem ::= "#init" | "#fresh"

endmodule

module FRESH-GEN

  imports INT
  imports STRING
  imports COLLECTIONS
  imports FRESH-GEN-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <g> .List </g>

  rule [init-rl] :
    <k> #init => #fresh ~> #fresh ~> #fresh </k>

  rule [fresh-rl] :
    <k> #fresh => .K ... </k>
    <g> _ ( .List => ListItem(!_ID:Int) ) </g>

endmodule
