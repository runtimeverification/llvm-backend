module PRIORITIZED-RULE-SYNTAX

  syntax Foo ::= FooA() | FooB() | FooC()

endmodule

module PRIORITIZED-RULE

  imports PRIORITIZED-RULE-SYNTAX

  rule [a-to-b]: FooA() => FooB() [priority(60)]
  rule [a-to-c]: FooA() => FooC() [priority(40)]

endmodule
