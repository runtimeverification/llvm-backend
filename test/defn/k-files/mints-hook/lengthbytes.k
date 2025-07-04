module LENGTHBYTES

imports INT
imports MINT
imports BYTES
imports BOOL
imports K-EQUAL

    syntax MInt{64}
    syntax MInt{256} 

    syntax Bytes ::= "bytesString" [macro]
    rule bytesString =>
        b"\x1d)0\xdd\xcc#\xeb\x14)Q\x8bAG\xcf\xd46\xa7\xdb\x8f\xc6&\xc1=N\xb6\xa4\x81%\xc2\xd2\xf4o"

    syntax Bool ::= "testLengthBytes" [function]
    rule testLengthBytes =>
        lengthBytes(bytesString):Int ==Int MInt2Unsigned(lengthBytes(bytesString):MInt{64}) andBool
        lengthBytes(bytesString):Int ==Int MInt2Unsigned(lengthBytes(bytesString):MInt{256})

endmodule