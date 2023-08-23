// Copyright (c) K Team. All Rights Reserved.
module K-RULE-FIND-TEST
    imports INT
    syntax Foo ::= foo(Bar)
    syntax Bar ::= bar() | bar2()

    rule [fooRule]: foo(bar()) => bar()

    syntax KResult ::= Int
    syntax A ::= "a" | Int
                | foo( A, A ) [seqstrict, klabel(foo)]
                | bar( A, A )
                | baz( A, A ) [strict(2), klabel(baz)]

    context [bar1]: bar( HOLE,    _ )
    context [bar2]: bar(    _, HOLE )

    rule a => 0
endmodule