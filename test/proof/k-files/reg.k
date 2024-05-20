module REG-SYNTAX

  imports DOMAINS-SYNTAX

  syntax ControlCommand ::= "exec" | ret(Int)

  syntax Immediate ::= Int
  syntax Register ::= "r0" | "r1" | "r2" | "r3"
  syntax Operand ::= Immediate | Register

  syntax Address ::= rel(Int) | abs(Int)

  syntax Instruction ::= "load"  Register "," Operand               [strict(2)]
                       | "store" Operand "," Operand                [seqstrict]
                       | "move"  Register "," Operand               [strict(2)]
                       | "add"   Register "," Operand "," Operand   [strict(2, 3)]
                       | "mul"   Register "," Operand "," Operand   [strict(2, 3)]
                       | "div"   Register "," Operand "," Operand   [strict(2, 3)]
                       | "le"    Register "," Operand "," Operand   [strict(2, 3)]
                       | "not"   Register "," Operand               [strict(2)]
                       | "br"    Operand "," Address "," Address    [strict(1)]
                       | "jump"  Address
                       | "ret"   Operand                            [seqstrict]

endmodule

module SMAP

  imports DOMAINS

  syntax SMap ::= ".SMap"
                | store(SMap, Int, Int)

  syntax KItem ::= select(SMap, Int)

  rule [select-base]: select(store(_, A, V), A) => V
  rule [select-rec]:  select(store(M, B, _), A) => select(M, A) requires A =/=Int B

endmodule

module REG

  imports DOMAINS
  imports REG-SYNTAX
  imports SMAP

  configuration
      <T>
          <k> $PGM:ControlCommand </k>
          <pc> 0 </pc>
          <r0> 0 </r0>
          <r1> 0 </r1>
          <r2> 0 </r2>
          <r3> 0 </r3>
          <imem>
                 0 |-> le r2, r0, 0
                 1 |-> not r2, r2
                 2 |-> br r2, abs(3), abs(6)
                 3 |-> add r1, r1, r0
                 4 |-> add r0, r0, -1
                 5 |-> jump abs(0)
                 6 |-> ret r1
          </imem>
          <dmem> .SMap </dmem>
      </T>

  syntax KResult ::= Int

  // Execute the next instruction
  rule [exec-next]:
       <k> exec => I ~> exec ... </k>
       <pc> A => A +Int 1 </pc>
       <imem> A |-> I ... </imem>

  // Register lookup
  rule [lookup-r0]: <k> r0 => V ... </k> <r0> V </r0>
  rule [lookup-r1]: <k> r1 => V ... </k> <r1> V </r1>
  rule [lookup-r2]: <k> r2 => V ... </k> <r2> V </r2>
  rule [lookup-r3]: <k> r3 => V ... </k> <r3> V </r3>

  // Register set
  syntax KItem ::= setRegister(Register, KItem) [strict(2)]
  rule [set-r0]: <k> setRegister(r0, V:Int) => .K ... </k> <r0> _ => V </r0>
  rule [set-r1]: <k> setRegister(r1, V:Int) => .K ... </k> <r1> _ => V </r1>
  rule [set-r2]: <k> setRegister(r2, V:Int) => .K ... </k> <r2> _ => V </r2>
  rule [set-r3]: <k> setRegister(r3, V:Int) => .K ... </k> <r3> _ => V </r3>

  // Intruction semantics
  rule [ret-val]:
       <k> ret V:Int ~> exec => ret(V) ... </k>
       <pc> _ => 0 </pc>
       <r0> _ => 0 </r0>
       <r1> _ => 0 </r1>
       <r2> _ => 0 </r2>
       <r3> _ => 0 </r3>
       <dmem> _ => .SMap </dmem> // so that we don't need to determine SMap equality

  rule [load-rl]:
       <k> load R:Register, A:Int => setRegister(R, select(M, A)) ... </k>
       <dmem> M </dmem>

  rule [store-rl]:
       <k> store A:Int, V:Int => .K ... </k>
       <dmem> M => store(M, A, V) </dmem>

  rule [move-rl]: move R:Register, V:Int => setRegister(R, V)

  rule [add-rl]: add R:Register, V1:Int, V2:Int => setRegister(R, V1 +Int V2)
  rule [mul-rl]: mul R:Register, V1:Int, V2:Int => setRegister(R, V1 *Int V2)
  rule [div-rl]: div R:Register, V1:Int, V2:Int => setRegister(R, V1 /Int V2) requires V2 =/=Int 0

  rule [le-1-rl]: le R:Register, V1:Int, V2:Int => setRegister(R, 1) requires V1 <=Int V2
  rule [le-0-rl]: le R:Register, V1:Int, V2:Int => setRegister(R, 0) requires notBool (V1 <=Int V2)

  rule [not-0-rl]: not R:Register, V:Int => setRegister(R, 0) requires V ==Int 1
  rule [not-1-rl]: not R:Register, V:Int => setRegister(R, 1) requires V ==Int 0

  // Conditiona/unconditional branching
  rule [br-1-rl]: br I:Int, A, _ => jump A requires I ==Int 1
  rule [br-0-rl]: br I:Int, _, A => jump A requires I ==Int 0

  rule [jump-rel]:
       <k> jump rel(A) => .K ... </k>
       <pc> PC => PC +Int A </pc>

  rule [jump-abs]:
       <k> jump abs(A) => .K ... </k>
       <pc> _ => A </pc>

  syntax Int ::= exp(Int, Int) [function, total, smt-hook((^ #1 #2))]

endmodule
