module TREE-REVERSE-INT-SYNTAX

    imports INT-SYNTAX

    syntax Tree ::= node(Tree, Tree) | Int
    syntax Tree ::= reverse(Tree) [function, total]
    syntax KItem ::= "#reverse-one"
    syntax KItem ::= "#reverse-one-five"
    syntax KItem ::= "#next"

endmodule

module TREE-REVERSE-INT

    imports INT
    imports TREE-REVERSE-INT-SYNTAX

    rule [base-case] : reverse(X:Int) => X
    rule [rec-case] :  reverse(node(T1, T2)) => node(reverse(T2), reverse(T1))

    // Test simple injections in functions
    rule [rev-one-five] : <k> #reverse-one-five => reverse(node(1:Int, 5:Int)) </k>

    // Test reduction of nested injections
    rule [rev-one] : <k> #reverse-one => reverse(1) </k>

endmodule
