module BYTES-UPDATE

imports INT
imports MINT
imports BYTES
imports BOOL
imports K-EQUAL

    syntax MInt{64}
    syntax MInt{256} 

    syntax Bytes ::= "bytesString" [macro]
    rule bytesString =>
        b"a\x00\x1b`\x00\x81`\x0b\x829\xf3`\x01`\x01Us\x8a\n\x19X\x951iBP\xd5p\x04"

    syntax Bool ::= "testBytesUpdate" [function]
    rule testBytesUpdate =>
        bytesString[2p64 <- 10p64] ==K bytesString[2 <- 10] andBool
        bytesString[2p256 <- 10p256] ==K bytesString[2 <- 10]
    
endmodule