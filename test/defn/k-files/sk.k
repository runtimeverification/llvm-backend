module LAMBDA-SYNTAX
  imports DOMAINS-SYNTAX
  syntax Exp ::= Id
               | "lambda" Id "." Exp
               | Exp Exp              [seqstrict, klabel(app), app]
               | "(" Exp ")"          [bracket]

  syntax Exp ::= Int | Bool
               | "-" Int
               | Exp "*" Exp          [left]
               | Exp "/" Exp
               > Exp "+" Exp          [left]
               > Exp "<=" Exp

  syntax left app

  syntax Exp ::= "if" Exp "then" Exp "else" Exp

  syntax Exp ::= "let" Id "=" Exp "in" Exp
  rule let X = E in E'::Exp => (lambda X . E') E                         [macro]

  syntax Exp ::= "letrec" Id Id "=" Exp "in" Exp
               | "mu" Id "." Exp
  rule letrec F::Id X = E in E' => let F = mu F . lambda X . E in E'     [macro]

  syntax Id ::= "$f" | "$x" | "$y"
  rule mu X . E => (lambda $f . (
                      (lambda $x . ($f (lambda $y . ($x $x $y))))
                      (lambda $x . ($f (lambda $y . ($x $x $y))))
                    )) (lambda X . E)                                   [macro]
endmodule


module LAMBDA
  imports LAMBDA-SYNTAX
  imports DOMAINS

  syntax BuiltVal ::= Id | Int | Bool
  syntax Val ::= BuiltVal
  syntax Exp ::= Val
  syntax KResult ::= Val

  syntax BuiltTernOp ::= "SS"
  syntax BuiltBinOp ::= "KK"
  syntax BuiltVal ::= BuiltTernOp
  syntax BuiltVal ::= BuiltBinOp
  syntax Val ::= BuiltBinOp Val [klabel(app), app]
  syntax BuiltBinOp ::= BuiltTernOp Val [klabel(app), app]

  syntax left app

  rule lambda X . X => SS KK KK                                         [macro]
  rule lambda X . Y:Id => KK Y                                          [macro, owise]
  rule lambda X . (E1 E2) => SS (lambda X . E1) (lambda X . E2)         [macro-rec]
  rule lambda X . B:BuiltVal => KK B                                    [macro, owise]

  rule KK E1 E2 => E1
  rule SS E1 E2 E3 => E1 E3 (E2 E3)

  syntax BuiltBinOp ::= "mul" | "div" | "add" |  "leq"

  rule - I => 0 -Int I                                                  [macro]

  rule E1 * E2  => mul E1 E2                                            [macro]
  rule E1 / E2  => div E1 E2                                            [macro]
  rule E1 + E2  => add E1 E2                                            [macro]
  rule E1 <= E2 => leq E1 E2                                            [macro]

  rule mul I1:Int I2:Int => I1 *Int I2
  rule div I1:Int I2:Int => I1 /Int I2 requires I2 =/=Int 0
  rule add I1:Int I2:Int => I1 +Int I2
  rule leq I1:Int I2:Int => I1 <=Int I2

  syntax BuiltTernOp ::= "cond"

  rule if E1 then E2 else E3 => cond E1 E2 E3                           [macro]

  rule cond true  E _ => E [priority(25)]
  rule cond false _ E => E [priority(25)]

endmodule
