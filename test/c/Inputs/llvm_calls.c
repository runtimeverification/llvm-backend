kore_symbol* v0x0000000002260730 = api.kore_symbol_new("inj");
kore_sort* v0x00000000022875c0 = api.kore_composite_sort_new("SortInternalOp");
api.kore_symbol_add_formal_argument(v0x0000000002260730,v0x00000000022875c0);
kore_sort* v0x0000000002287620 = api.kore_composite_sort_new("SortKItem");
api.kore_symbol_add_formal_argument(v0x0000000002260730,v0x0000000002287620);
kore_pattern* v0x0000000002287600 = api.kore_composite_pattern_from_symbol(v0x0000000002260730);
kore_symbol* v0x0000000002287810 = api.kore_symbol_new("Lbl'Hash'next'LSqBUndsRSqBUnds'EVM'Unds'InternalOp'Unds'MaybeOpCode");
kore_pattern* v0x0000000002287920 = api.kore_composite_pattern_from_symbol(v0x0000000002287810);
kore_symbol* v0x0000000002287a20 = api.kore_symbol_new("Lbl'Hash'lookupOpCode'LParUndsCommUndsCommUndsRParUnds'EVM'Unds'MaybeOpCode'Unds'Bytes'Unds'Int'Unds'Schedule");
kore_pattern* v0x0000000002287be0 = api.kore_composite_pattern_from_symbol(v0x0000000002287a20);
kore_symbol* v0x0000000002287ce0 = api.kore_symbol_new("Lbl'Hash'parseByteStack'LParUndsRParUnds'SERIALIZATION'Unds'Bytes'Unds'String");
kore_pattern* v0x0000000002287e00 = api.kore_composite_pattern_from_symbol(v0x0000000002287ce0);
kore_sort* v0x0000000002287f00 = api.kore_composite_sort_new("SortString");
kore_pattern* v0x00000000022880b0 = api.kore_pattern_new_token_with_len("",116746,v0x0000000002287f00);
api.kore_composite_pattern_add_argument(v0x0000000002287e00,v0x00000000022880b0);
api.kore_composite_pattern_add_argument(v0x0000000002287be0,v0x0000000002287e00);
kore_sort* vv0x0000000002287e00 = api.kore_composite_sort_new("SortInt");
kore_pattern* v0x0000000002288300 = api.kore_pattern_new_token_with_len("0",1,vv0x0000000002287e00);
api.kore_composite_pattern_add_argument(v0x0000000002287be0,v0x0000000002288300);
kore_symbol* vv0x0000000002288300 = api.kore_symbol_new("LblLONDON'Unds'EVM");
kore_pattern* vv0x00000000022880b0 = api.kore_composite_pattern_from_symbol(vv0x0000000002288300);
api.kore_composite_pattern_add_argument(v0x0000000002287be0,vv0x00000000022880b0);
api.kore_composite_pattern_add_argument(v0x0000000002287920,v0x0000000002287be0);
api.kore_composite_pattern_add_argument(v0x0000000002287600,v0x0000000002287920);
char * data_out;
size_t len_out;
api.kore_simplify(v0x0000000002287600,v0x0000000002287620,&data_out,&len_out);
/* 0x0000000002287600 |-> #next[_]_EVM_InternalOp_MaybeOpCode(
    #lookupOpCode(_,_,_)_EVM_MaybeOpCode_Bytes_Int_Schedule(
        #parseByteStack(_)_SERIALIZATION_Bytes_String(
            ""
        ),
        "0",
        LONDON_EVM()
    )
) */
