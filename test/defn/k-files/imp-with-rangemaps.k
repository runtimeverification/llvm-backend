// Copyright (c) 2014-2019 K Team. All Rights Reserved.

// Modified IMP definition that adds range maps.

module IMP-WITH-RANGEMAPS-SYNTAX
  imports RANGEMAP
  imports BOOL-SYNTAX
  imports INT-SYNTAX
  imports ID

  syntax AExp  ::= Int | Id
                 | "rm_size" "(" RmExp ")"       [klabel(rm_size(_)), symbol, strict]
                 | "lookup" "(" RmExp "," AExp ")" [klabel(lookup(_,_)), symbol, seqstrict]
                 | AExp "/" AExp              [klabel(_/_), symbol, left, seqstrict, color(pink)]
                 > AExp "+" AExp              [klabel(_+_), symbol, left, seqstrict, color(pink)]
                 | "(" AExp ")"               [bracket]
  syntax BExp  ::= Bool
                 | "contains_key" "(" RmExp "," AExp ")" [klabel(contains_key(_,_)), symbol, seqstrict]
                 | AExp "<=" AExp             [klabel(_<=_), symbol, seqstrict, latex({#1}\leq{#2}), color(pink)]
                 | "!" BExp                   [klabel(!_), symbol, strict, color(pink)]
                 > BExp "&&" BExp             [klabel(_&&_), symbol, left, strict(1), color(pink)]
                 | "(" BExp ")"               [bracket]
  syntax RmExp ::= RangeMap | Id
                 | "empty" "(" ")"            [klabel(empty()), symbol]
                 | "element" "(" AExp "," AExp "," AExp ")" [klabel(element(_,_,_)), symbol, seqstrict]
                 | "update" "(" RmExp "," AExp "," AExp "," AExp ")" [klabel(update(_,_,_,_)), symbol, seqstrict]
                 | "remove" "(" RmExp "," AExp "," AExp ")" [klabel(remove(_,_,_)), symbol, seqstrict]
                 | "updaterangemap" "(" RmExp "," RmExp ")" [klabel(updatermap(_,_)), symbol, seqstrict]
                 | "difference" "(" RmExp "," RmExp ")" [klabel(difference(_,_)), symbol, seqstrict]
  syntax Block ::= "{" "}"                    [klabel({}), symbol]
                 | "{" Stmt "}"               [klabel({_}), symbol, format(%1%i%n%2%d%n%3)]
  syntax Stmt  ::= Block
                 | Id "=" AExp ";"            [klabel(_=a_;), symbol, strict(2), color(pink), format(%1 %2 %3%4)]
                 | Id "=" RmExp ";"            [klabel(_=r_;), symbol, strict(2), color(pink), format(%1 %2 %3%4)]
                 | "if" "(" BExp ")"
                   Block "else" Block         [klabel(if(_)_else_), symbol, strict(1), colors(yellow, white, white, yellow), format(%1 %2%3%4 %5 %6 %7)]
                 | "while" "(" BExp ")" Block [klabel(while(_)_), symbol, colors(yellow,white,white), format(%1 %2%3%4 %5)]
                 > Stmt Stmt                  [klabel(__), symbol, left, format(%1%n%2)]

  syntax Pgm ::= "int" Ids ";" "rangemap" Ids ";" Stmt  [klabel(int_;rangemap_;_), symbol, format(%1 %2%3%n%4 %5%6%n%7), colors(yellow,pink,yellow,pink)]
  syntax Ids ::= List{Id,","}                 [klabel(_,_), symbol, format(%1%2 %3)]
endmodule

module IMP-WITH-RANGEMAPS
  imports IMP-WITH-RANGEMAPS-SYNTAX
  imports BOOL
  imports INT
  imports MAP
  imports SET
  imports BASIC-K

  syntax KResult ::= Int | Bool | RangeMap
  syntax KItem   ::= Pgm

  configuration <T color="yellow">
                  <k color="green"> $PGM:Pgm </k>
                  <state color="red"> .Map </state>
                </T>

  rule <k> X:Id => STATE[X] ... </k> <state> STATE </state> requires X in_keys(STATE)

  rule <k> empty() => .RangeMap ... </k>
  rule <k> element(S:Int, E:Int, V:Int) => [ S , E ) r|-> V  ... </k>
  rule <k> update(Rm:RangeMap, S:Int, E:Int, V:Int) => Rm [ [S, E) <- V ] ... </k>
  rule <k> remove(Rm:RangeMap, S:Int, E:Int) => Rm [ [S, E) <- undef ] ... </k>
  rule <k> updaterangemap(Rm1:RangeMap, Rm2:RangeMap) => updateRangeMap(Rm1, Rm2) ... </k>
  rule <k> difference(Rm1:RangeMap, Rm2:RangeMap) => Rm1 -RangeMap Rm2 ... </k>
  rule <k> contains_key(Rm:RangeMap, K:Int) => K in_keys ( Rm ) ... </k>
  rule <k> rm_size(Rm:RangeMap) => size ( Rm ) ... </k>
  rule <k> lookup(Rm:RangeMap, K:Int) => Rm [ K ] ... </k>

  rule <k> I1 / I2 => I1 /Int I2 ... </k>  requires I2 =/=Int 0
  rule <k> I1 + I2 => I1 +Int I2 ... </k>

  rule <k> I1 <= I2   => I1 <=Int I2 ... </k>
  rule <k> ! T        => notBool T   ... </k>
  rule <k> true && B  => B           ... </k>
  rule <k> false && _ => false       ... </k>

  rule <k> {}  => . ... </k>
  rule <k> {S} => S ... </k>

  rule <k> X = I:Int; => . ... </k> <state> STATE => STATE [ X <- I ] </state>
  rule <k> X = Rm:RangeMap; => . ... </k> <state> STATE => STATE [ X <- Rm ] </state>

  rule <k> S1:Stmt S2:Stmt => S1 ~> S2 ... </k>

  rule <k> if (true)  S else _ => S ... </k>
  rule <k> if (false) _ else S => S ... </k>

  rule [while]: <k> while (B) S => if (B) {S while (B) S} else {} ... </k>

  rule <k> int (X , Xs => Xs) ; rangemap _ ; _ ... </k> <state> STATE => STATE [ X <- 0 ] </state>
    requires notBool (X in keys(STATE))

  rule <k> int .Ids ; rangemap (X , Xs => Xs) ; _ ... </k> <state> STATE => STATE [ X <- .RangeMap ] </state>
    requires notBool (X in keys(STATE))

  rule <k> int .Ids ; rangemap .Ids ; S => S ... </k>
endmodule
