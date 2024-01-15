module BYTES-COW-4-SYNTAX
    syntax KItem  ::= "a"
endmodule

module BYTES-COW-4
    imports BYTES-COW-4-SYNTAX
    imports BYTES
    imports INT-SYNTAX

    syntax Foo ::= Bytes | "empty"

    configuration
      <test>
        <k> $PGM </k>
        <mem> empty </mem>
        <stuff> empty </stuff>
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
