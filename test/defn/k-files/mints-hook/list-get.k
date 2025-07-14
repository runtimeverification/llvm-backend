module LIST-GET

imports INT
imports MINT
imports BOOL
imports LIST
imports K-EQUAL

    syntax MInt{64}
    syntax MInt{256} 

    syntax List ::= "listOfMInt" [macro]
    rule listOfMInt => ListItem(2p64) (ListItem(3p64) (ListItem(4p64) .List))

    syntax Bool ::= "getMInt" [function]
    rule getMInt => listOfMInt[1]:KItem ==K listOfMInt[1p64]:KItem andBool
                    listOfMInt[1]:KItem ==K listOfMInt[1p256]:KItem

endmodule