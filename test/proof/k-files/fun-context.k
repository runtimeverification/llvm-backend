module FUN-CONTEXT-SYNTAX

  imports INT-SYNTAX

  syntax Int ::= foo(Int) [function]
  syntax KItem ::= "#exec"

endmodule

module FUN-CONTEXT

  imports INT
  imports FUN-CONTEXT-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <bar> 100 </bar>

  rule [start] : <k> #exec => foo(0) </k>

  rule [foo-rl] : [[ foo(0) => I ]]
                 <bar> I </bar>

endmodule
