module INC 
  imports INT
  imports BOOL

  rule I => I +Int 1
    requires I >Int 0 andBool I <Int 100
  rule I => I -Int 1
    requires I <Int 0 andBool I >Int -100

endmodule
