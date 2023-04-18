module TEST
  imports INT
  imports MAP

  syntax Int ::= foo(Map) [function]
  syntax KItem ::= bar(Int)

  rule 0 => foo(0 |-> 1 bar(0) |-> bar(1) bar(1) |-> 2)
  rule foo(bar(I) |-> J:Int _:Map) => J
endmodule
