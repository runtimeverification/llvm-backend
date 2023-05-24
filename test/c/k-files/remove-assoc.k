module REMOVE-ASSOC
  imports DOMAINS

  syntax Foo ::= run(List) [klabel(run), symbol]
               | #run(List, Int)
               | done(Int) [klabel(done), symbol]

  rule run(L) => #run(L, 0)
  rule #run(.List, S) => done(S)
  rule #run(ListItem(L) Rest, S) => #run(Rest, L +Int S)
endmodule
