module TEST
  imports LIST
  imports INT

  syntax KItem ::= bar(K, List)
  syntax KItem ::= foo(List) [function]
  syntax KItem ::= baz(KItem, K) [symbol]

  rule foo(ListItem(0) ListItem(1 #as X)) => bar(X, .List)
  rule foo(ListItem(0 #as X)) => bar(X, .List)
  rule foo(L ListItem(2) ListItem(3) ListItem(4 #as X)) => bar(X, L)
  rule foo(ListItem(0) L ListItem(2 #as X)) => bar(X, L)
  rule foo(.List) => bar(.K, .List)
  rule foo(ListItem(1 #as X) L) => bar(X +Int 2, L)
endmodule
