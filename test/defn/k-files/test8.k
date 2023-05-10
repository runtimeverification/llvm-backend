module TEST
  imports DEFAULT-STRATEGY
  imports INT

  configuration <k> $PGM:K </k> <s/>

  rule <k> 5 => 6 </k> <s> #STUCK() => .K ...</s>
  rule <k> I => I +Int 1 </k> requires I <Int 5
endmodule
