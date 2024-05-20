module IMP5-SYNTAX

  imports INT-SYNTAX
  imports BOOL-SYNTAX

  syntax Id2 ::= "x1" | "x2" | "x3" | "x4" | "x5" | "ret"
  syntax AExp  ::= Int | Id2
                 | "-" Int                    [format(%1%2)]
                 | AExp "/" AExp              [left, seqstrict, color(pink)]
                 | "(" AExp ")"               [bracket]
                 | AExp "*" AExp              [left, seqstrict]
                 | AExp "%" AExp              [left, seqstrict]
                 > AExp "+" AExp              [left, seqstrict, color(pink)]
                 | AExp "-" AExp              [left, seqstrict, color(pink)]
  syntax BExp  ::= Bool
                 | AExp "==" AExp             [seqstrict]
                 | AExp "<=" AExp             [seqstrict]
                 | AExp "<" AExp              [seqstrict]
                 | AExp ">=" AExp             [seqstrict]
                 | AExp ">" AExp              [seqstrict]
                 | "!" BExp                   [strict, color(pink)]
                 | "(" BExp ")"               [bracket]
                 // | "nondet"
                 > BExp "&&" BExp             [left, strict(1), color(pink)]
  syntax Block ::= "{" "}"
                 | "{" Stmt "}"               [format(%1%i%n%2%d%n%3)]
  syntax Stmt  ::= Block
                 | Id2 "=" AExp ";"            [strict(2), color(pink), format(%1 %2 %3%4)]
                 | "if" "(" BExp ")"
                   Block "else" Block         [strict(1), colors(yellow, white, white, yellow), format(%1 %2%3%4 %5 %6 %7)]
                 | "while" "(" BExp ")" Block [colors(yellow,white,white), format(%1 %2%3%4 %5)]
                 > Stmt Stmt                  [left, format(%1%n%2)]
  syntax Pgm ::= Stmt
  syntax Pgm ::= "int" Ids ";" Stmt           [format(%1 %2%3%n%4), colors(yellow,pink)]
  syntax Ids ::= List{Id2,","}                 [format(%1%2 %3)]

  syntax Vars5 ::= Int "," Int "," Int "," Int "," Int "," Int

endmodule

module IMP5

  imports IMP5-SYNTAX
  imports INT
  imports BOOL

  syntax KResult ::= Int | Bool

  configuration <T color="yellow">
                  <k color="green"> $PGM:Pgm </k>
                  <s color="red"> ( 0 , 0 , 0 , 0 , 0 , 0 ):Vars5 </s>
                </T>

  rule <k> x1 => I ...</k> <s> I , _ , _ , _ , _ , _ </s>
  rule <k> x2 => I ...</k> <s> _ , I , _ , _ , _ , _ </s>
  rule <k> x3 => I ...</k> <s> _ , _ , I , _ , _ , _ </s>
  rule <k> x4 => I ...</k> <s> _ , _ , _ , I , _ , _ </s>
  rule <k> x5 => I ...</k> <s> _ , _ , _ , _ , I , _ </s>
  rule <k> ret => I ...</k> <s> _ , _ , _ , _ , _ , I </s>
  rule I1 / I2 => I1 /Int I2  requires I2 =/=Int 0
  rule I1 * I2 => I1 *Int I2
  rule I1 % I2 => I1 %Int I2
  rule I1 + I2 => I1 +Int I2
  rule I1 - I2 => I1 -Int I2
  rule - I1 => 0 -Int I1

  rule I1 == I2 => I1 ==Int I2
  rule I1 <= I2 => I1 <=Int I2
  rule I1 < I2 => I1 <Int I2
  rule I1 >= I2 => I1 >=Int I2
  rule I1 > I2 => I1 >Int I2
  rule ! T => notBool T
  // We do not have C to avoid requires
  rule true && B => B // requires C
  rule false && _ => false // requires notBool C

  rule {} => .K
  rule {S} => S

  rule <k> x1 = I:Int; => .K ...</k> <s> ( _ => I ) , _ , _ , _ , _ , _ </s>
  rule <k> x2 = I:Int; => .K ...</k> <s> _ , ( _ => I ) , _ , _ , _ , _ </s>
  rule <k> x3 = I:Int; => .K ...</k> <s> _ , _ , ( _ => I ) , _ , _ , _ </s>
  rule <k> x4 = I:Int; => .K ...</k> <s> _ , _ , _ , ( _ => I ) , _  , _ </s>
  rule <k> x5 = I:Int; => .K ...</k> <s> _ , _ , _ , _ , ( _ => I ) , _ </s>
  rule <k> ret = I:Int; => .K ...</k> <s> _ , _ , _ , _ , _ , ( _ => I ) </s>
  rule S1:Stmt S2:Stmt => S1 ~> S2
  rule if (true)  S else _ => S // requires C
  rule if (false) _ else S => S // requires notBool C

  rule while (B) S => if (B) {S while (B) S} else {}

  // We do not support these void declarations
  // rule <k> int (X,Xs => Xs);_ </k> <state> _:Map (.Map => X|->0) </state>
  // We have _L here instead of .Ids because we are not doing anything with declaration
  rule int _L; S => S

endmodule
