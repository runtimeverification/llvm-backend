module LAMBDA-EXPLICIT-SUBST-SYNTAX

  imports DOMAINS

  syntax Term ::= Value
                | Term Term [seqstrict, left]
                | "(" Term ")" [bracket]

  syntax Value ::= "lambda" Int Term
                 | Int

endmodule

module LAMBDA-EXPLICIT-SUBST

  imports LAMBDA-EXPLICIT-SUBST-SYNTAX

  syntax KResult ::= Value

  // capture-free substitution
  syntax Term ::= substitute(Term, Int, Term) [function, total]
  syntax Bool ::= isFreeIn(Int, Term) [function, total]
  syntax Int ::= maxVarIn(Term) [function, total]
               | max(Int, Int) [function, total]

  rule [subst-apply] : (lambda X T) S:Value => substitute(T, X, S)

  rule [subst-x1] : substitute(X, X, T) => T
  rule [subst-x2] : substitute(X, Y, _) => X requires X =/=Int Y
  rule [subst-app] : substitute(T S, X, R) => substitute(T, X, R) substitute(S, X, R)
  rule [subst-la1] : substitute(lambda X T, X, _) => lambda X T
  rule [subst-la2] : substitute(lambda X T, Y, S) => lambda X substitute(T, Y, S) requires X =/=Int Y andBool notBool isFreeIn(X, S)
  rule [subst-la3] : substitute(lambda X T, Y, S) => lambda X substitute(T, Y, substitute(S, X, maxVarIn(S) +Int 1)) requires X =/=Int Y andBool isFreeIn(X, S)

  rule [free-x1] : isFreeIn(X, X) => true
  rule [free-x2] : isFreeIn(X, Y) => false requires X =/=Int Y
  rule [free-app] : isFreeIn(X, T S) => isFreeIn(X, T) andBool isFreeIn(X, S)
  rule [free-la1] : isFreeIn(X, lambda X _) => false
  rule [free-la2] : isFreeIn(X, lambda Y T) => isFreeIn(X, T) requires X =/=Int Y

  rule [in-x] : maxVarIn(X) => X
  rule [in-app] : maxVarIn(T S) => max(maxVarIn(T), maxVarIn(S))
  rule [in-lam] : maxVarIn(lambda X T) => max(X, maxVarIn(T))

  rule [max-1] : max(A, B) => A requires A >=Int B
  rule [max-2] : max(A, B) => B requires B >=Int A

endmodule
