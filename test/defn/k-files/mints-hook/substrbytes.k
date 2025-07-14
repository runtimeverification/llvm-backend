module SUBSTRBYTES

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


    syntax Bool ::= "testSubstrBytes" [function]
    rule testSubstrBytes => 
        substrBytes(bytesString, 0, 5) ==K substrBytes(bytesString, 0p64, 5p64) andBool
        substrBytes(bytesString, 0, 5) ==K substrBytes(bytesString, 0p256, 5p256)

endmodule