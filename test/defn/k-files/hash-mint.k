module HASH-MINT
  imports MINT

  syntax MInt{160}
  syntax MInt{256}
  syntax MInt{128}

  configuration <k> $PGM:K </k>
                <state> 0p256 |-> 1p256 0p160 |-> 1p160 0p128 |-> 1p128 </state>

  syntax KItem ::= foo() | bar() | baz()

  rule <k> foo() => bar() ~> State [ 0p256 ] ...</k>
       <state> State </state>
  rule <k> bar() => baz() ~> State [ 0p160 ] ...</k>
       <state> State </state>
  rule <k> baz() => State [ 0p128 ] ...</k>
       <state> State </state>

endmodule
