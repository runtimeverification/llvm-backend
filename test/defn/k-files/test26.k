module TEST
  imports STRING
  imports INT

  syntax KItem ::= log(String, KItem) [function, hook(IO.logTerm)]
  syntax String ::= longString(Int) [function]

  rule 0 => log("foo", longString(17))
  rule longString(I) => longString(I -Int 1) +String longString(I -Int 1) [owise]
  rule longString(0) => "f"
endmodule
