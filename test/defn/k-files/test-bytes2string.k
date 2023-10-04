module TEST-BYTES2STRING-SYNTAX
    syntax Foo ::= "foo" [token]
endmodule

module TEST-BYTES2STRING
imports TEST-BYTES2STRING-SYNTAX
imports BYTES

    rule foo => String2Bytes(Bytes2String(b"\xa9OSt\xfc\xe5\xed\xbc\x8e*\x86\x97\xc1S1g~n\xbf\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"))
endmodule