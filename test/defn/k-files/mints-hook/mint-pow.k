module MINT-POW

imports INT
imports MINT
imports BOOL
imports K-EQUAL

    syntax MInt{64}
    syntax MInt{256} 

    // MInt exponenciation always assume unsigned results, except for the
    // exponentiation by zero, which is always 1.
    syntax Bool ::= "testMIntPow" [function]
    rule testMIntPow =>
        MInt2Unsigned(2p256 ^MInt 255p256) ==Int 2 ^Int 255 andBool
        MInt2Unsigned(2p64 ^MInt 63p64) ==Int 2 ^Int 63

    syntax Bool ::= "testMIntPowZero" [function]
    rule testMIntPowZero =>
        MInt2Unsigned(2p256 ^MInt 0p256) ==Int 1 andBool
        MInt2Unsigned(2p64 ^MInt 0p64) ==Int 1

    syntax Bool ::= "testMIntPowOne" [function]
    rule testMIntPowOne =>
        MInt2Unsigned(2p256 ^MInt 1p256) ==Int 2 andBool
        MInt2Unsigned(2p64 ^MInt 1p64) ==Int 2

    // test negative exponentiation == 0
    syntax Bool ::= "testMIntPowNegative" [function]
    rule testMIntPowNegative =>
        MInt2Signed(2p256 ^MInt -10p256) ==Int 0 andBool
        MInt2Signed(2p64 ^MInt -10p64) ==Int 0

    
    syntax Bool ::= "testMIntPowAll" [function]
    rule testMIntPowAll =>
        testMIntPow andBool
        testMIntPowZero andBool
        testMIntPowOne andBool
        testMIntPowNegative
    

endmodule