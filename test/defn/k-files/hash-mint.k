module HASH-MINT
  imports MINT

  syntax MInt{160}
  syntax MInt{256}

  configuration <k> $PGM:K </k>
                <state> 0p256 |-> 1p256 0p160 |-> 1p160 </state>

  syntax KItem ::= foo() | bar()

  rule <k> foo() => bar() ~> State [ 0p256 ] ...</k>
       <state> State </state>
  rule <k> bar() => State [ 0p160 ] ...</k>
       <state> State </state>

endmodule
