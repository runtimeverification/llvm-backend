module TEST
  imports STRING-BUFFER
  imports INT

  configuration <T> <k> $PGM:Pgm </k> <buffer> .StringBuffer </buffer> </T>

  syntax Cmd ::= addStr(String)
  syntax Pgm ::= List{Cmd,";"}
  syntax Cmd ::= loop(Int)

  rule <k> .Pgm => StringBuffer2String(SB) </k> <buffer> SB </buffer>
  rule loop(I) ; Cmds => loop(I -Int 1) ; Cmds requires I >Int 0
  rule loop(I) ; Cmds => Cmds requires I ==Int 0
  rule <k> addStr(S) ; Cmds => Cmds </k> <buffer> SB => SB +String S </buffer>

endmodule
