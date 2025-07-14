module BYTES-GET

imports MINT
imports BYTES
imports INT
imports BOOL

    syntax MInt{64}
    syntax MInt{256}

    syntax Bytes ::= "bytesString" [macro]
    rule bytesString =>
        b"a\x00\x1b`\x00\x81`\x0b\x829\xf3`\x01`\x01Us\x8a\n\x19X\x951iBP\xd5p\x04"

    syntax Bool ::= "testBytesGet" [function]
    rule testBytesGet =>
        {bytesString[2p64]}:>MInt{64} ==MInt Int2MInt(bytesString[2]) andBool
        {bytesString[2p256]}:>MInt{256} ==MInt Int2MInt(bytesString[2])

endmodule