module DV

imports DOMAINS

syntax Foo ::= foo(Int)
             | succ(Foo)

rule succ(foo(X:Int)) => foo(X +Int 1)

endmodule