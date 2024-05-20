module INTERNAL-DOMAIN-SYNTAX

  // Domain constructors
  syntax IBool ::= "true" | "false"
  syntax IInt ::= "12345" | "10" | "100" | "200" | "1" | "90" | "210" | "0"

endmodule

module INTERNAL-DOMAIN

  imports INTERNAL-DOMAIN-SYNTAX

endmodule

module IMP5-RW-LITERAL-SYNTAX

  imports INTERNAL-DOMAIN-SYNTAX

  syntax Id2 ::= "x1" | "x2" | "x3" | "x4" | "x5" | "ret"
  syntax AExp  ::= IInt | Id2
                 > AExp "+" AExp
                 | AExp "-" AExp
  syntax BExp  ::= IBool
                 | AExp ">" AExp
  syntax Block ::= "{" "}"
                 | "{" StmtList "}"
  syntax Stmt  ::= Block
                 | Id2 "=" AExp ";"
                 | "if" "(" BExp ")"
                   Block "else" Block
                 | "while" "(" BExp ")" Block
  syntax StmtList ::= Stmt | Stmt StmtList
  syntax Pgm ::= StmtList

endmodule

module IMP5-RW-LITERAL

  imports IMP5-RW-LITERAL-SYNTAX
  imports INTERNAL-DOMAIN

  syntax Vars5 ::= IInt "," IInt "," IInt "," IInt "," IInt "," IInt

  configuration <k color="green"> $PGM:Pgm </k>
                <s color="red"> ( 0 , 0 , 0 , 0 , 0 , 0 ):Vars5 </s>

  // for manually defining heating/cooling rules
  syntax AExp ::= "#hole"
  syntax BExp ::= "#hole"

  // Identifiers
  rule <k> x1  => I ...</k> <s> I , _ , _ , _ , _ , _ </s>
  rule <k> x2  => I ...</k> <s> _ , I , _ , _ , _ , _ </s>
  rule <k> x3  => I ...</k> <s> _ , _ , I , _ , _ , _ </s>
  rule <k> x4  => I ...</k> <s> _ , _ , _ , I , _ , _ </s>
  rule <k> x5  => I ...</k> <s> _ , _ , _ , _ , I , _ </s>
  rule <k> ret => I ...</k> <s> _ , _ , _ , _ , _ , I </s>

  // + [seqstrict]
  rule <k> A1:AExp + A2:AExp => A1 ~> #hole + A2 ... </k>
  rule <k> (A1:IInt ~> #hole + A2) => A2 ~> A1 ~> #hole + #hole  ... </k>
  rule <k> (10 ~> 200 ~> #hole + #hole) => 210 ... </k> // the transfer example: 200 + 10

  // - [seqstrict]
  rule <k> A1:AExp - A2:AExp => A1 ~> #hole - A2 ... </k>
  rule <k> (A1:IInt ~> #hole - A2) => A2 ~> A1 ~> #hole - #hole  ... </k>
  rule <k> (10 ~> 100 ~> #hole - #hole) => 90 ... </k> // the transfer example: 100 - 10

  // > [seqstrict]
  rule <k> A1:AExp > A2:AExp => A1 ~> #hole > A2 ... </k>
  rule <k> (A1:IInt ~> #hole > A2) => A2 ~> A1 ~> #hole > #hole  ... </k>
  rule <k> (100 ~> 10 ~> #hole > #hole) => false ... </k> // the transfer example: 10 > 100

  // assignment [strict(2)]
  rule <k> X:Id2 = A:AExp; => A ~> X = #hole; ... </k>
  rule <k> (I:IInt ~> x1  = #hole;) => .K ...</k> <s> ( _ => I ) , _ , _ , _ , _ , _ </s>
  rule <k> (I:IInt ~> x2  = #hole;) => .K ...</k> <s> _ , ( _ => I ) , _ , _ , _ , _ </s>
  rule <k> (I:IInt ~> x3  = #hole;) => .K ...</k> <s> _ , _ , ( _ => I ) , _ , _ , _ </s>
  rule <k> (I:IInt ~> x4  = #hole;) => .K ...</k> <s> _ , _ , _ , ( _ => I ) , _ , _ </s>
  rule <k> (I:IInt ~> x5  = #hole;) => .K ...</k> <s> _ , _ , _ , _ , ( _ => I ) , _ </s>
  rule <k> (I:IInt ~> ret = #hole;) => .K ...</k> <s> _ , _ , _ , _ , _ , ( _ => I ) </s>

  // conditional [strict(1)]
  rule <k> if (B:BExp) S1 else S2 => B ~> if (#hole) S1 else S2 ...</k>
  rule <k> (true  ~> if (#hole)  S else _) => S ... </k>
  rule <k> (false ~> if (#hole)  _ else S) => S ... </k>

  // while loops
  rule while (B) S => if (B) {S while (B) S} else {}

  // blocks
  rule {} => .K
  rule {S} => S

  // Statements
  rule S:Stmt SL:StmtList => S ~> SL

endmodule
