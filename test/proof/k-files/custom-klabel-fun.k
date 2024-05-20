module CUSTOM-KLABEL-FUN-SYNTAX

  syntax Foo ::= "a"
               | bar1(Foo) [function, klabel(bar1_fun)]
               | bar2(Foo) [function, klabel(bar2_fun), symbol]
               | baz1(Foo) [function, klabel(baz_fun)]
               | baz2(Foo) [function, klabel(baz_fun)]

endmodule

module CUSTOM-KLABEL-FUN

  imports CUSTOM-KLABEL-FUN-SYNTAX

  rule [bar1-rule] : bar1(X:Foo) => bar2(X)
  rule [bar2-rule] : bar2(X:Foo) => baz1(X)
  rule [baz1-rule] : baz1(X:Foo) => baz2(X)
  rule [baz2-rule] : baz2(X:Foo) => X

endmodule
