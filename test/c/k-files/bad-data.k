module BAD-DATA
  imports BYTES

  syntax WithString ::= withString ( String )
                      | f ( Int ) [function, total]
  rule f(_) => withString("I have an @ sign")

  syntax WithBytes ::= withBytes ( Bytes )
                     | g ( Int ) [function, total]
  rule g(_) => withBytes(b"The @ sign has code \x40 in ASCII")
endmodule
