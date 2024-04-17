module TEST
  imports LIST
  imports INT
  imports BOOL

 configuration <k> foo() </k> <list> baz(ListItem(3) ListItem(4)) </list> <counter> 100000 </counter>

 syntax Pgm ::= foo() | bar(KItem, List)
 syntax KItem ::= baz(List)

 rule <k> foo() => bar(K, L) </k> <list> baz(L:List) #as K:KItem </list> <counter> X => X -Int 1 </counter>
 rule <k> bar(_, _) => foo() </k> <counter> X </counter> requires X >Int 0

endmodule
