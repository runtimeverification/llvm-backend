module ARITHMETIC-SYNTAX
  imports UNSIGNED-INT-SYNTAX

  syntax Exp ::= Int
               | Exp "+" Exp [left, strict]
endmodule

module STEPS
  imports ARITHMETIC-SYNTAX
  imports DOMAINS

  configuration
    <k> $PGM:Exp </k>

  rule A + B => A +Int B

  syntax Bool ::= isKResult(Exp) [function, symbol]
  rule isKResult(_::Int) => true
  rule isKResult(_)      => false [owise]
endmodule
