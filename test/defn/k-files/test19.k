module TEST
  imports INT
  imports ID

  syntax Exp ::= "int" Id "=" Int ";" Exp
               | Exp "+" Exp [strict]
               | Exp "-" Exp [strict]
  syntax Exp ::= Int
  syntax KResult ::= Int

  configuration <k> $PGM:K </k> <state> .Map </state>

  rule <k> int X = I ; E => E ...</k> <state>... .Map => X |-> I ...</state>
  rule <k> A - B => 0 -Int C ...</k> <state>... _ |-> C ...</state>
  rule <k> A + B => C ...</k> <state>... _ |-> C ...</state>

endmodule
