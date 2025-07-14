module LIST-SIZE

imports INT
imports MINT
imports BOOL
imports LIST

    syntax MInt{64}
    syntax MInt{256} 

    syntax List ::= "listOfMInt" [macro]
    rule listOfMInt => ListItem(2p64) (ListItem(3p64) (ListItem(4p64) .List))

    syntax Bool ::= "testListSize" [function]
    rule testListSize =>
        size(listOfMInt) ==Int MInt2Unsigned(size(listOfMInt)::MInt{64}) andBool
        size(listOfMInt) ==Int MInt2Unsigned(size(listOfMInt)::MInt{256})

endmodule