module MACRO-SYNTAX

  imports BOOL-SYNTAX
  imports INT-SYNTAX

  syntax Bool ::= inRange(Int) [function]

  syntax Int ::= "UPPER"   [alias]
               | "LOWER"   [macro]

  rule [up-alias]: UPPER => 99
  rule [lo-macro]: LOWER => 0

endmodule

module MACRO
  imports BOOL
  imports INT
  imports MACRO-SYNTAX

  rule [inrange] : inRange(X:Int) => LOWER <=Int X andBool X <=Int UPPER

endmodule
