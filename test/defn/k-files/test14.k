module TEST
  imports INT
  imports LIST

  configuration <k> $PGM:K </k> <list> ListItem(0) ListItem(1) ListItem(2) </list>

  rule <k> 0 => 1 </k> <list> ListItem(0) ListItem(1) ...</list>
  rule <k> 0 => -1 </k> <list> ListItem(0) ListItem(1) ListItem(2) ListItem(3) </list>
endmodule
