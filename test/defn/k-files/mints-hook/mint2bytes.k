module MINT2BYTES

imports INT
imports MINT
imports BYTES
imports BOOL
imports K-EQUAL

    syntax MInt{256} 

    syntax Bytes ::= "bytesString" [macro]
    // Bytes must be 256 bits (32 bytes) for this test
    rule bytesString =>
        b"\x1d)0\xdd\xcc#\xeb\x14)Q\x8bAG\xcf\xd46\xa7\xdb\x8f\xc6&\xc1=N\xb6\xa4\x81%\xc2\xd2\xf4o"

    syntax MInt{256} ::= "mintValue" [function]
    rule mintValue => 13189850602041981829950296977395610463010118185856010131061817836684537164911p256

    syntax Int ::= "intValue" [function]
    rule intValue => 13189850602041981829950296977395610463010118185856010131061817836684537164911

    syntax Bool ::= "testMInt2Bytes" [function]
    // Only supporting 256-bit MInt for this test in Big Endian format
    rule testMInt2Bytes =>
        MInt2Bytes(mintValue) ==K Int2Bytes(intValue, BE, Unsigned) andBool
        MInt2Bytes(mintValue) ==K bytesString

endmodule