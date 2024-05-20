module TREE-REVERSE-SYNTAX

    syntax Tree ::= "a" | "b" | "c" | node(Tree, Tree)
    syntax Tree ::= reverse(Tree) [function, total]
    syntax KItem ::= "#Init"
    syntax KItem ::= "#next"

endmodule

module TREE-REVERSE

    imports TREE-REVERSE-SYNTAX

    rule [base-case-a] : reverse(a) => a
    rule [base-case-b] : reverse(b) => b
    rule [base-case-c] : reverse(c) => c
    rule [rec-case] :  reverse(node(T1, T2)) => node(reverse(T2), reverse(T1))
    rule [init] : <k> #Init => #next </k>
    rule [next] : <k> #next => reverse(node(a, b)) </k>

endmodule