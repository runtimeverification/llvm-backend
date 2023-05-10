module TEST
  imports BOOL
  imports INT

  syntax Done ::= "done"
  syntax Token ::= "token1" [token] | "token2" [token]

  rule token1 => token2
  rule token2 => done
  rule false => 0
  rule 1 => token1
  rule true => false
  rule 0 => 1
endmodule
