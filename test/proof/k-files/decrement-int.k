module DECREMENT-INT

  imports INT

  rule [decrement] : I:Int => I -Int 1 requires I >Int 0

endmodule
