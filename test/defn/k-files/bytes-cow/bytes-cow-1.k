module BYTES-COW-1-SYNTAX
    syntax KItem  ::= "a"
endmodule

module BYTES-COW-1
    imports BYTES-COW-1-SYNTAX
    imports BYTES
    imports INT-SYNTAX

    configuration
      <test>
        <k> $PGM </k>
        <mem> .Bytes </mem>
        <stuff> .Bytes </stuff>
      </test>

    syntax KItem ::= "b" | "c" | "d" | "e"

    rule <k> a => b </k>
        <mem> _ => b"alice" </mem>
    rule <k> b => c </k>
        <mem> B </mem>
        <stuff> _ => B </stuff>
    rule <k> c => d </k>
        <mem> B => replaceAtBytes(B, 0:Int, b"bob__") </mem>
endmodule
