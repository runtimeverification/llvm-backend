module BYTES-COW-3-SYNTAX
    syntax KItem ::= "a"
endmodule

module BYTES-COW-3
    imports BYTES-COW-3-SYNTAX
    imports BYTES
    imports INT-SYNTAX

    configuration
      <test>
        <k> $PGM </k>
        <dup> .K </dup>
      </test>

    syntax KItem ::= "b" | "c" | "d" | "e"

    rule <k> a => b </k>
        <dup> _ => b"alice" </dup>
    rule <k> b => c </k>
        <dup> DUPPED ~> . => DUPPED ~> DUPPED ~> . </dup>
    rule <k> c => d </k>
        <dup> B => replaceAtBytes(B, 0:Int, b"bob__") ...</dup>
endmodule
