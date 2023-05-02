module TEST
  imports INT
  imports SET

  syntax KItem ::= bar(Int)
  syntax Int ::= foo(Set) [function]

  rule 0 => foo(SetItem(bar(0)) SetItem(bar(1)) SetItem(bar(2)))
  rule foo(SetItem(bar(I)) _) => I requires I ==Int 2
endmodule
