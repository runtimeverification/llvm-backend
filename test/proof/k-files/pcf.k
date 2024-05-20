module PCF-SYNTAX

imports DOMAINS-SYNTAX

  syntax Variable ::= Id
  syntax Abstraction ::= lambda(Variable, Term)

  syntax Int ::= "-" Int [function, total]

  syntax Base ::= if(Term, Term, Term) [strict(1)]
                | add(Term, Term)
                | mul(Term, Term)
                | div(Term, Term)      [seqstrict]
                | le(Term, Term)       [seqstrict]
                | not(Term)            [strict]

  syntax Value ::= Int | Bool | Abstraction

  syntax Term ::= Variable
                | Value
                | Base
                | app(Term, Term)      [seqstrict]
                | fix(Variable, Term)

  syntax KItem ::= addFreezer1(Term) | addFreezer2(Int)
  syntax KItem ::= mulFreezer1(Term) | mulFreezer2(Int)

endmodule

module PCF

  imports DOMAINS
  imports PCF-SYNTAX

  syntax KResult ::= Value

  rule [neg-rl]: - V:Int => 0 -Int V

  // substitution that COULD potentially have variable capturing
  // but since we are dealing with closed terms this is fine
  syntax Term ::= substitute(Term, Variable, Term) [function, total]

  rule [subs-int]: substitute(V:Int, _, _) => V
  rule [subs-bool]: substitute(B:Bool, _, _) => B

  rule [subs-var1]: substitute(X:Variable, X, T) => T
  rule [subs-var2]: substitute(Y:Variable, X, _) => Y requires X =/=K Y

  rule [subs-if]: substitute(if(T1, T2, T3), X, T) => if(substitute(T1, X, T), substitute(T2, X, T), substitute(T3, X, T))
  rule [subs-add]: substitute(add(T1, T2), X, T) => add(substitute(T1, X, T), substitute(T2, X, T))
  rule [subs-mul]: substitute(mul(T1, T2), X, T) => mul(substitute(T1, X, T), substitute(T2, X, T))
  rule [subs-div]: substitute(div(T1, T2), X, T) => div(substitute(T1, X, T), substitute(T2, X, T))
  rule [subs-le]: substitute(le(T1, T2), X, T) => le(substitute(T1, X, T), substitute(T2, X, T))
  rule [subs-not]: substitute(not(T'), X, T) => not(substitute(T', X, T))

  rule [subst-app]: substitute(app(T1, T2), X, T) => app(substitute(T1, X, T), substitute(T2, X, T))

  rule [subst-la1]: substitute(lambda(X, T'), X, _) => lambda(X, T')
  rule [subst-la2]: substitute(lambda(Y, T'), X, T) => lambda(Y, substitute(T', X, T)) requires X =/=K Y

  rule [subst-fix1]: substitute(fix(X, T'), X, _) => fix(X, T')
  rule [subst-fix2]: substitute(fix(Y, T'), X, T) => fix(Y, substitute(T', X, T)) requires X =/=K Y

  rule [if-t-rl]: if(true, T, _) => T
  rule [if-f-rl]: if(false, _, T) => T
  rule [le-rl]: le(V1:Int, V2:Int) => V1 <=Int V2
  rule [not-rl]: not(V:Bool) => notBool V

  // manually handling strictness since we need to describe the loop invariant
  rule [add-val]:   add(V1:Int, V2:Int) => V1 +Int V2
  rule [add-heat1]: add(V1:Term, V2:Term) => V1 ~> addFreezer1(V2) requires notBool isKResult(V1)
  rule [add-cool1]: V1:Int ~> addFreezer1(V2) => add(V1, V2)
  rule [add-head2]: add(V1:Int, V2:Term) => V2 ~> addFreezer2(V1) requires notBool isKResult(V2)
  rule [add-cool2]: V2:Int ~> addFreezer2(V1) => add(V1, V2)

  rule [mul-val]:   mul(V1:Int, V2:Int) => V1 *Int V2
  rule [mul-heat1]: mul(V1:Term, V2:Term) => V1 ~> mulFreezer1(V2) requires notBool isKResult(V1)
  rule [mul-cool1]: V1:Int ~> mulFreezer1(V2) => mul(V1, V2)
  rule [mul-heat2]: mul(V1:Int, V2:Term) => V2 ~> mulFreezer2(V1) requires notBool isKResult(V2)
  rule [mul-cool2]: V2:Int ~> mulFreezer2(V1) => mul(V1, V2)

  rule [div-rl]: div(V1:Int, V2:Int) => V1 /Int V2 requires V2 =/=Int 0

  rule [app-rl]: app(lambda(X, T), V:Value) => substitute(T, X, V)
  rule [fix-rl]: fix(F, T) => substitute(T, F, fix(F, T))

  // for verification
  syntax Int ::= exp(Int, Int) [function, total, smt-hook((^ #1 #2))]

endmodule
