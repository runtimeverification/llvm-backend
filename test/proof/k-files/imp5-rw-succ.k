module INTERNAL-DOMAIN-SYNTAX

  // Domain constructors
  syntax IBool ::= "true" | "false"
  syntax Nat ::= "0" | s(Nat)            [overload(Nat)]

  // Functions (as constructors)
  syntax NatExp ::= Nat
                  | s(NatExp)            [overload(Nat), strict]
                  | NatExp "+Nat" NatExp [seqstrict]
                  | NatExp "-Nat" NatExp [seqstrict]

  syntax IBoolExp ::= IBool
                    | NatExp ">Nat" NatExp [strict]

  // Aditional syntactic sugar for the transfer example
  syntax NatExp ::= "12345" | "10" | "100" | "200" | "1"

endmodule

module INTERNAL-DOMAIN

  imports INTERNAL-DOMAIN-SYNTAX

  // Nat plus
  rule 0 +Nat M:Nat => M
  rule s(N:Nat) +Nat M:Nat => s(N +Nat M)

  // Nat monus
  rule 0 -Nat _:Nat => 0
  rule s(N:Nat) -Nat 0 => s(N)
  rule s(N:Nat) -Nat s(M:Nat) => N -Nat M

  // Nat greater than
  rule 0 >Nat _:Nat => false
  rule s(_:Nat) >Nat 0 => true
  rule s(N:Nat) >Nat s(M:Nat) => N >Nat M

endmodule

module IMP5-RW-SUCC-SYNTAX

  imports INTERNAL-DOMAIN-SYNTAX

  syntax Id2 ::= "x1" | "x2" | "x3" | "x4" | "x5" | "ret"
  syntax AExp  ::= NatExp | Id2
                 | "(" AExp ")"               [bracket]
                 > AExp "+" AExp              [left, seqstrict]
                 | AExp "-" AExp              [left, seqstrict]
  syntax BExp  ::= IBoolExp
                 | AExp ">" AExp              [seqstrict]
                 | "(" BExp ")"               [bracket]
  syntax Block ::= "{" "}"
                 | "{" Stmt "}"
  syntax Stmt  ::= Block
                 | Id2 "=" AExp ";"           [strict(2)]
                 | "if" "(" BExp ")"
                   Block "else" Block         [strict(1)]
                 | "while" "(" BExp ")" Block
                 > Stmt Stmt                  [left]
  syntax Pgm ::= Stmt

endmodule

module IMP5-RW-SUCC

  imports IMP5-RW-SUCC-SYNTAX
  imports INTERNAL-DOMAIN

  syntax KResult ::= Nat | IBool

  configuration <T>
                  <k color="green"> $PGM:Pgm </k>
                  <x1  color="red"> 0:Nat </x1>
                  <x2  color="red"> 0:Nat </x2>
                  <x3  color="red"> 0:Nat </x3>
                  <x4  color="red"> 0:Nat </x4>
                  <x5  color="red"> 0:Nat </x5>
                  <ret color="red"> 0:Nat </ret>
                </T>

  rule <k> x1  => I ...</k> <x1>  I </x1>
  rule <k> x2  => I ...</k> <x2>  I </x2>
  rule <k> x3  => I ...</k> <x3>  I </x3>
  rule <k> x4  => I ...</k> <x4>  I </x4>
  rule <k> x5  => I ...</k> <x5>  I </x5>
  rule <k> ret => I ...</k> <ret> I </ret>

  rule I1 + I2 => I1 +Nat I2
  rule I1 - I2 => I1 -Nat I2

  rule I1 > I2 => I1 >Nat I2

  rule {} => .K
  rule {S} => S

  rule <k> x1  = I:Nat; => .K ...</k> <x1>  ( _ => I ) </x1>
  rule <k> x2  = I:Nat; => .K ...</k> <x2>  ( _ => I ) </x2>
  rule <k> x3  = I:Nat; => .K ...</k> <x3>  ( _ => I ) </x3>
  rule <k> x4  = I:Nat; => .K ...</k> <x4>  ( _ => I ) </x4>
  rule <k> x5  = I:Nat; => .K ...</k> <x5>  ( _ => I ) </x5>
  rule <k> ret = I:Nat; => .K ...</k> <ret> ( _ => I ) </ret>

  rule S1:Stmt S2:Stmt => S1 ~> S2
  rule if (true)  S else _ => S
  rule if (false) _ else S => S

  rule while (B) S => if (B) {S while (B) S} else {}

  // For the transfer example:
  // Values are arbitrary, but they preserve the original flow of control of the transfer example
  rule 12345 => 0 // 0
  rule 10 => s(0) // 1
  rule 100 => s(s(s(s(s(s(s(s(s(s(0))))))))))  // 10
  rule 200 => s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(s(0)))))))))))))))))))) // 20
  rule 1 => s(0) // 1

endmodule
