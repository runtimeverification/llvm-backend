module CONDITIONAL-FUNCTION-SYNTAX

  imports INT-SYNTAX

  syntax Foo ::= foo(Int)
               | bar(Foo)

  syntax Int ::= factorial(Int) [function]

endmodule

module CONDITIONAL-FUNCTION

  imports INT
  imports CONDITIONAL-FUNCTION-SYNTAX

  rule [bar-rl] : bar(foo(X:Int)) => foo(factorial(X))

  rule [fac-rec] : factorial(X:Int) => X *Int factorial(X -Int 1)
      requires X >=Int 1

  rule [fac-base] : factorial(0) => 1

endmodule
