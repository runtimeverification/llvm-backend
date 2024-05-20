module CAST-SYNTAX
  imports INT-SYNTAX
  imports ID-SYNTAX
  imports STRING-SYNTAX

  syntax Exp ::= Id | Int | String

endmodule

module CAST
  imports CAST-SYNTAX
  imports INT

  configuration <k> $PGM:Exp </k>
                <sum> 0 </sum>

  rule <k> Pgm:Exp => .K </k>
       <sum> SUM:Int => SUM +Int {Pgm}:>Int </sum>
endmodule
