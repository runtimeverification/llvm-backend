module IMP-SUM
  imports INT

  configuration <k> $PGM </k>

  syntax KItem ::= start(Int) | loop(Int, Int) | done(Int)

  rule <k> start(I) => loop(I, 0) </k>
  rule <k> loop(I:Int, S:Int) => loop(I -Int 1, S +Int I) </k> requires I >Int 0
  rule <k> loop(I, S) => done(S) </k> requires I ==Int 0
endmodule
