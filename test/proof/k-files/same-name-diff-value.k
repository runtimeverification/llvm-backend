module SAME-NAME-DIFF-VALUE
    imports INT
    imports K-EQUAL
    imports BOOL

    configuration <k> $PGM:Ops </k>
                  <accounts>
                    <account multiplicity="*" type="Map">
                        <acctID>   0   </acctID>
                        <balance>  0   </balance>
                    </account>
                  </accounts>

    syntax Ops ::= Op ";" Ops [symbol(seq)]
                | Op 

    syntax Op ::= "#transferFunds" Int Int Int
                | "#init" Int Int

    syntax NoOp ::= "#finish" 


    rule <k> #init ACCT VALUE ; O:Op => O </k>
         <accounts>
            ( .Bag
                  =>
                 <account>
                    <acctID> ACCT </acctID>
                    <balance> VALUE </balance>
                </account>
            )
         </accounts>
    
     rule <k> #transferFunds ACCT ACCT VALUE => .K ... </k>
         <account>
           <acctID> ACCT </acctID>
           <balance> ORIGFROM </balance>
         </account>
      requires VALUE <=Int ORIGFROM


      rule <k> #transferFunds ACCTFROM ACCTTO VALUE => .K ... </k>
         <account>
           <acctID> ACCTFROM </acctID>
           <balance> ORIGFROM => ORIGFROM -Int VALUE </balance>
           ...
         </account>
         <account>
           <acctID> ACCTTO </acctID>
           <balance> ORIGTO => ORIGTO +Int VALUE </balance>
           ...
         </account>
      requires ACCTFROM =/=K ACCTTO andBool VALUE <=Int ORIGFROM
      [preserves-definedness]

    rule <k> #transferFunds ACCTFROM _ACCTTO VALUE => .K ... </k>
         <account>
           <acctID> ACCTFROM </acctID>
           <balance> ORIGFROM </balance>
           ...
         </account>
      requires VALUE >Int ORIGFROM

endmodule