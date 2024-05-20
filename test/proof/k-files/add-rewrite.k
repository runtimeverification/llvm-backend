module ADD-REWRITE-SYNTAX

  syntax Nat ::= z()
               | s(Nat)
  syntax Nat ::= add(Nat, Nat)

  syntax State ::= state(Nat, Nat)

endmodule

module ADD-REWRITE

  imports ADD-REWRITE-SYNTAX

  rule [add-zero] : add(z(), N:Nat) => N
  rule [add-succ] : add(s(N:Nat), M:Nat) => add(N, s(M))

  rule [state-next] : state(s(N:Nat), M:Nat) => add(s(N), M) ~> state(N, M)

  rule [state-succ] : s(M:Nat) ~> state(N:Nat, _:Nat) => state(N, s(M))
  rule [state-zero] : z() ~> state(N:Nat, _:Nat) => state(N, z())

endmodule
