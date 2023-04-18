module TEST
  imports INT
  imports LIST

  syntax Operands ::= List{Operand,","} [klabel(ops)]
  syntax Ints ::= #toInts(List) [function]
  syntax Ints ::= List{Int,","} [klabel(ops)]
  syntax NEInts ::= NeList{Int,","} [klabel(ops)]
  syntax Ints ::= NEInts
  syntax Operands ::= Ints
  syntax Operand ::= Int

  rule #toInts(ListItem(I) ARGS) => I, #toInts(ARGS)
  rule #toInts(.List) => .NEInts
  rule I:Int => #toInts(ListItem(I))
endmodule
