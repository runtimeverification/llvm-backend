module BYTES2MINT

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

    syntax Bool ::= "testBytes2MInt" [function]

    // Only supporting 256-bit MInt for this test in Big Endian format
    rule testBytes2MInt =>
        Bytes2MInt(bytesString) ==K Int2MInt(Bytes2Int(bytesString, BE, Unsigned))

endmodule