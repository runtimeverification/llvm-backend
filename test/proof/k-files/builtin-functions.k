module BUILTIN-FUNCTIONS-SYNTAX

  imports BOOL-SYNTAX
  imports INT-SYNTAX
  imports STRING-SYNTAX
  imports BYTES-SYNTAX

  syntax Int    ::= double(Int)  [function, total]
  syntax Bool   ::= isPos(Int)   [function, total]
  syntax Int    ::= abs(Int)     [function, total]
  syntax String ::= head(String) [function]
  syntax Bytes  ::= head(Bytes)  [function]

  syntax KItem ::= #next(KItem)

endmodule

module BUILTIN-FUNCTIONS

  imports BOOL
  imports INT
  imports STRING
  imports BYTES
  imports K-EQUAL
  imports BUILTIN-FUNCTIONS-SYNTAX

  rule [double-rule]   : double(I:Int)  => I *Int 2
  rule [ispos-rule]    : isPos(I:Int)   => I >Int 0
  rule [abs-rule]      : abs(I:Int)     => #if (I <Int 0) #then (0 -Int I) #else I #fi
  rule [str-head-rule] : head(S:String) => substrString(S, 0, 1)
  rule [bts-head-rule] : head(B:Bytes)  => substrBytes(B, 0, 1)

  rule #next(I) => I

endmodule
