module IO-SYNTAX
  imports DOMAINS
  imports K-IO
  imports STRING

  syntax Stmt ::= "close" "(" Id ")" 
            | "seek" "(" Id "," Int ")" 
            | "seekEnd" "(" Id "," Int ")" 
            | "putc" "(" Id "," Int ")" 
            | "write" "(" Id "," String ")" 
            | "lock" "(" Id "," Int ")" 
            | "unlock" "(" Id "," Int ")" 
            | "log" "(" String "," String ")" 

  syntax IOExp ::= IOVal
                | "open" "(" String "," String ")" 
                | "tell" "(" Id ")" 
                | "getc" "(" Id ")" 
                | "read" "(" Id "," Int ")" 

  syntax IOVal ::= IOString | IOInt

  syntax Stmt ::= IOExp | Stmt Stmt [left]
                  | Id "=" IOExp ";" [strict(2)]

  syntax Pgm ::= "int" Ids ";" Stmt | Stmt
  syntax Ids ::= List{Id,","}

  syntax KResult ::= IOVal
endmodule

module IO
  imports IO-SYNTAX
  configuration <T color="yellow">
                  <k color="green"> $PGM:Pgm </k>
                  <state color="red"> .Map </state>
                </T>

  rule open(S:String, S':String) => #open(S, S') 
  rule log(S:String, S':String) => #logToFile(S, S') 

  rule <k> tell(X:Id) => #tell(I:Int) ...</k> <state>... X |-> I ...</state>
  rule <k> getc(X:Id) => #getc(I:Int) ...</k> <state>... X |-> I ...</state>
  rule <k> read(X:Id, I:Int) => #read(I':Int, I) ...</k> <state>... X |-> I' ...</state>
  rule <k> close(X:Id) => #close(I:Int) ...</k> <state>... X |-> I ...</state>
  rule <k> seek(X:Id, I:Int) => #seek(I':Int, I) ...</k> <state>... X |-> I' ...</state>
  rule <k> seekEnd(X:Id, I:Int) => #seekEnd(I':Int, I) ...</k> <state>... X |-> I' ...</state>
  rule <k> putc(X:Id, I:Int) => #putc(I':Int, I) ...</k> <state>... X |-> I' ...</state>
  rule <k> write(X:Id, S:String) => #write(I:Int, S) ...</k> <state>... X |-> I ...</state>
  rule <k> lock(X:Id, I:Int) => #lock(I':Int, I) ...</k> <state>... X |-> I' ...</state>
  rule <k> unlock(X:Id, I:Int) => #unlock(I':Int, I) ...</k> <state>... X |-> I' ...</state>

  rule <k> X = I:IOVal; => . ...</k> <state>... X |-> (_ => I) ...</state>
  rule <k> X:Id => I ...</k> <state>... X |-> I ...</state>
  rule S1:Stmt S2:Stmt => S1 ~> S2  [structural]

  rule <k> int (X,Xs => Xs);_ </k> <state> Rho:Map (.Map => X|->0) </state>
    requires notBool (X in keys(Rho))
  rule int .Ids; S => S  [structural]

endmodule
