module TEST
  imports INT
  imports STRING

  configuration <T> <k> $PGM:Pgm </k> <m> .Map </m> </T>

  syntax Pgm ::= List{Cmd, ";"}

  syntax Cmd ::= makeMap(Int)
               | ret(Int)

  rule .Pgm => 0
  rule <k> makeMap(I) ; Cmds => Cmds </k> <m> M => M [ Int2String(I) <- I ] </m>
    requires I ==Int 0

  rule <k> makeMap(I) ; Cmds => makeMap(I -Int 1) ; Cmds </k> <m> M => M [ Int2String(I) <- I ] </m> requires I >Int 0

  rule <k> ret(I) ; Cmds => M [ Int2String(I) ] </k> <m> M </m>

endmodule
