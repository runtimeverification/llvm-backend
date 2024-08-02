module CELL-VALUE-SYNTAX

  imports DOMAINS-SYNTAX
  imports COLLECTIONS

  syntax KItem ::= "#init"
                 | #newMsg(Int, Int, String)
                 | #delMsg(Int)
                 | #inMessages(Int)

endmodule

module CELL-VALUE

  imports DOMAINS
  imports CELL-VALUE-SYNTAX

  configuration
    <k> $PGM:KItem </k>
    <messages>
      <message multiplicity="*" type="Map">
        <msgID> 0  </msgID>
        <value> 0  </value>
        <to>    "" </to>
      </message>
    </messages>

  syntax Bool ::= findMsg(Int, MessagesCell) [function]

  rule [find-msg-f]:
       findMsg( _ID, <messages> .Bag </messages> ) => false

  rule [find-msg-t]:
       findMsg( ID,
                <messages>
                  <message> <msgID> ID </msgID> ... </message>
                  _MSGS
                </messages>
              )
              => true

  rule [find-msg-r]:
       findMsg( ID,
                <messages>
                  <message> <msgID> ID' </msgID> ... </message>
                  MSGS
                </messages>
              )
              => findMsg(ID, <messages> MSGS </messages>)
    requires ID =/=Int ID'


  rule [init-rl]:
    <k> #init =>
             #newMsg(0, 100, "fooAccount")
          ~> #newMsg(1, 50, "barAccount")
          ~> #newMsg(2, 200, "oofAccount")
          ~> #delMsg(1)
          ~> #inMessages(1)
        ...
    </k>

  rule [newmsg-rl]:
    <k> #newMsg(ID, VAL, REC) => .K ... </k>
    <messages>
       ( .Bag
      => <message>
           <msgID> ID  </msgID>
           <value> VAL </value>
           <to>    REC </to>
           ...
         </message>
       )
     ...
     </messages>

  rule [delmsg-rl]:
    <k> #delMsg(ID) => .K ... </k>
    <messages>
       ( <message>
           <msgID> ID  </msgID>
           ...
         </message>
         => .Bag
       )
     ...
     </messages>

  rule [find-rl]:
    <k> #inMessages(ID) => findMsg(ID, <messages> MSGS </messages>) ... </k>
    <messages> MSGS </messages>

endmodule
