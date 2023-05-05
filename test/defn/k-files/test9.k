module TEST
  imports INT

  syntax Operands ::= List{Operand, ","} [klabel(ops)]
  syntax Ints ::= List{Int, ","} [klabel(ops)]
  syntax NEInts ::= NeList{Int, ","} [klabel(ops)]
  syntax Operands ::= Ints
  syntax Operand ::= Int
  syntax Ints ::= NEInts

  rule O:Operand, Os:Operands => (O):KItem
endmodule
