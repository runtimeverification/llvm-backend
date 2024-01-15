module BYTES-COW-5-SYNTAX
    syntax KItem  ::= "a"
endmodule

module BYTES-COW-5
    imports BYTES-COW-5-SYNTAX
    imports BYTES
    imports COLLECTIONS
    imports INT-SYNTAX

    configuration
      <test>
        <k> $PGM </k>
        <mem> .List </mem>
        <stuff> .List </stuff>
      </test>

    syntax KItem ::= "b" | "c" | "d" | "e"

    rule <k> a => b </k>
        <mem> _ => ListItem(SetItem(b"key" |-> b"val")) </mem>
    rule <k> b => c </k>
        <mem> B </mem>
        <stuff> _ => B </stuff>
    rule <k> c => d </k>
        <mem>
          ListItem(SetItem(K |-> V)) =>
            #let NewK = replaceAtBytes(K, 0, b"N_K") #in
            #let NewV = replaceAtBytes(V, 0, b"N_V") #in
              ListItem(SetItem(NewK |-> NewV))
         </mem>
endmodule
