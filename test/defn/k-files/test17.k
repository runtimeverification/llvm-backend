module TEST
  imports LIST
  imports BOOL
  imports K-EQUAL

  syntax List ::= foo() [function]

  rule foo() => .List

  rule B:Bool => #if B #then foo() #else .List #fi
endmodule
