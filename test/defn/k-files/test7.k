module TEST
  imports INT
  configuration <k> $PGM:K </k> <exit-code exit=""> 0 </exit-code>

  rule <k> I => .K </k> <exit-code> _ => I </exit-code>
endmodule
