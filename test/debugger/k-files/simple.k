module SIMPLE
  imports INT

  rule I => I +Int 1
    requires I <Int 100
endmodule
