module TEST
  syntax KItem ::= bar()
  syntax Done ::= "done"
  syntax KItem ::= foo(KItem)

  rule bar() #as T => foo(T)
  rule foo(bar()) => done
endmodule
