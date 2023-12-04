module WASM-MAPS
  imports MAP-INT-TO-VAL
  // imports MAP-INT-TO-INT

  configuration
        <wasm>
          <instrs> $PGM </instrs>
          <locals> .MapIntToVal </locals>
          // <locals> .MapIntToInt </locals>
        </wasm>

  syntax Instr ::= "init_local"  Int Int
  // -----------------------------------
  rule <instrs> init_local INDEX VALUE => . ... </instrs>
       <locals> LOCALS => LOCALS {{ INDEX <- wrap(VALUE) }} </locals>

endmodule

module MAP-INT-TO-INT
  imports WRAPPED-INT

  syntax MapIntToInt [hook(MAP.Map)]
  syntax MapIntToInt ::= MapIntToInt MapIntToInt
         [ left, function, hook(MAP.concat), klabel(_MapIntToInt_),
           symbol, assoc, comm, unit(.MapIntToInt), element(_Int2Int|->_)
         ]
  syntax MapIntToInt ::= ".MapIntToInt"
         [function, total, hook(MAP.unit),klabel(.MapIntToInt), symbol]
  syntax MapIntToInt ::= WrappedInt "Int2Int|->" WrappedInt
         [function, total, hook(MAP.element), klabel(_Int2Int|->_), symbol]

  syntax MapIntToInt ::= MapIntToInt "[" key: WrappedInt "<-" value: WrappedInt "]" [function, total, klabel(MapInt2Int:update), symbol, hook(MAP.update), prefer]

  syntax priorities _Int2Int|->_ > _MapIntToInt_ .MapIntToInt
  syntax non-assoc _Int2Int|->_

  syntax MapIntToInt ::= MapIntToInt "{{" key: Int "<-" value: Int "}}"
                 [ function, total, klabel(MapIntToInt:primitiveUpdate), symbol,
                   prefer
                 ]
  rule M:MapIntToInt {{ Key:Int <- Value:Int }}
      => M[wrap(Key) <- wrap(Value)]

endmodule

module MAP-INT-TO-VAL
  imports WRAPPED-INT

  syntax Val ::= WrappedInt
 // -----------------------

  syntax MapIntToVal [hook(MAP.Map)]
  syntax MapIntToVal ::= MapIntToVal MapIntToVal
         [ left, function, hook(MAP.concat), klabel(_MapIntToVal_),
           symbol, assoc, comm, unit(.MapIntToVal), element(_Int2Val|->_)
         ]
  syntax MapIntToVal ::= ".MapIntToVal"
         [function, total, hook(MAP.unit),klabel(.MapIntToVal), symbol]
  syntax MapIntToVal ::= WrappedInt "Int2Val|->" Val
         [function, total, hook(MAP.element), klabel(_Int2Val|->_), symbol]

  syntax MapIntToVal ::= MapIntToVal "[" key: WrappedInt "<-" value: Val "]" [function, total, klabel(MapInt2Val:update), symbol, hook(MAP.update), prefer]

  syntax priorities _Int2Val|->_ > _MapIntToVal_ .MapIntToVal
  syntax non-assoc _Int2Val|->_

  syntax MapIntToVal ::= MapIntToVal "{{" key: Int "<-" value: Val "}}"
                 [ function, total, klabel(MapIntToVal:primitiveUpdate), symbol,
                   prefer
                 ]
  rule M:MapIntToVal {{ Key:Int <- Value:Val }}
      => M[wrap(Key) <- Value]

endmodule

module WRAPPED-INT
  imports INT

  syntax WrappedInt ::= wrap(Int)  [symbol, klabel(wrapInt)]
  // -------------------------------------------------------

  syntax Int ::= unwrap(WrappedInt)  [function, total, injective, symbol, klabel(unwrapInt)]
  // ---------------------------------------------------------------------------------------
  rule unwrap(wrap(A:Int)) => A
endmodule
