module TEST
  imports INT
  imports STRING

  syntax Split ::= "S(" String ")"
                 | "I(" Int    ")"
                 | String
                 | Int

  syntax Split ::= extract( Split ) [function]
  rule extract( S( S ) ) => S
  rule extract( I( I ) ) => I

  syntax Pgm ::= Split

  configuration <k> $PGM:Split </k>

  rule (S( _ ) #Or I( _ )) #as X => extract( X )
endmodule
