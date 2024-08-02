module BUILTIN-HOOK-EVENTS-SYNTAX

  imports BOOL-SYNTAX

  syntax Foo ::= foo(Bool)
  
endmodule

module BUILTIN-HOOK-EVENTS

  imports BOOL
  imports BUILTIN-HOOK-EVENTS-SYNTAX

  rule foo(B) => notBool notBool B

endmodule
