module LIST-UPDATE

imports INT
imports MINT
imports BOOL
imports LIST
imports K-EQUAL

    syntax MInt{64}

    syntax List ::= "listOfMInt" [macro]
    rule listOfMInt => ListItem(2p64) (ListItem(3p64) (ListItem(4p64) .List))

    syntax Bool ::= "testListUpdate" [function]
    // This function only support MInt{64} for now, use MInt{256} can cause undefined behavior
    rule testListUpdate => listOfMInt[2p64 <- 10p64] ==K listOfMInt[2 <- 10p64]

endmodule