module SUM-CELL-SYNTAX
  imports INT-SYNTAX
endmodule

module SUM-CELL
  imports SUM-CELL-SYNTAX
  imports INT

  configuration <k> $PGM:Int </k>
                <sum> 0 </sum>

  rule <k> I:Int => .K </k>
       <sum> SUM:Int => SUM +Int I </sum>
endmodule
