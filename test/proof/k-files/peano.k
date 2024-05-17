module PEANO-SYNTAX

    syntax Nat ::= "0"
                 | s(Nat)        [overload(Nat)]

    syntax Exp ::= Nat
    syntax Exp ::= s(Exp)        [overload(Nat), strict]
    syntax Exp ::= add(Exp, Exp) [seqstrict]
    syntax Exp ::= mul(Exp, Exp) [seqstrict]

endmodule

module PEANO

    imports PEANO-SYNTAX

    syntax KResult ::= Nat

    rule [add-0] : add(0, M:Nat) => M
    rule [add-1] : add(s(N:Nat), M:Nat) => s(add(N, M))

    rule [mul-0] : mul(0, _:Nat) => 0
    rule [mul-1] : mul(s(N:Nat), M:Nat) => add(M, mul(N, M))

endmodule