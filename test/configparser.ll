target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%block = type { %blockheader, [0 x i64*] }
%blockheader = type { i64 }
%map = type { i64, i8*, i8* }
%mpz = type { i32, i32, i64* }
%set = type { i8*, i8*, i64 }
%list = type { i64, i64, i8*, i8*, i8*, i8*, i8* }
%string = type { %blockheader, [0 x i8] }

@"sym_name_Lbl'-LT-'T'-GT-'{}" = global [19 x i8] c"Lbl'-LT-'T'-GT-'{}\00"
@"sym_name_Lbl'-LT-'k'-GT-'{}" = global [19 x i8] c"Lbl'-LT-'k'-GT-'{}\00"
@"sym_name_Lbl'-LT-'state'-GT-'{}" = global [23 x i8] c"Lbl'-LT-'state'-GT-'{}\00"
@"sym_name_Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}" = global [44 x i8] c"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}\00"
@"sym_name_Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}" = global [64 x i8] c"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" = global [82 x i8] c"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" = global [82 x i8] c"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}" = global [80 x i8] c"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}" = global [80 x i8] c"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" = global [78 x i8] c"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" = global [78 x i8] c"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" = global [78 x i8] c"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}\00"
@"sym_name_Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" = global [78 x i8] c"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}\00"
@"sym_name_Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}" = global [106 x i8] c"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}\00"
@"sym_name_Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}" = global [102 x i8] c"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}\00"
@"sym_name_Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}" = global [36 x i8] c"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}\00"
@"sym_name_Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}" = global [48 x i8] c"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}\00"
@"sym_name_Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}" = global [90 x i8] c"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}\00"
@"sym_name_Lbl'Stop'Map{}" = global [15 x i8] c"Lbl'Stop'Map{}\00"
@"sym_name_Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [51 x i8] c"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds'Map'Unds'{}" = global [21 x i8] c"Lbl'Unds'Map'Unds'{}\00"
@"sym_name_Lbl'Unds'andBool'Unds'{}" = global [25 x i8] c"Lbl'Unds'andBool'Unds'{}\00"
@"sym_name_Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [61 x i8] c"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [53 x i8] c"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [57 x i8] c"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [61 x i8] c"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [53 x i8] c"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [56 x i8] c"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [60 x i8] c"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [57 x i8] c"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [58 x i8] c"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [58 x i8] c"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [58 x i8] c"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [58 x i8] c"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" = global [62 x i8] c"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}\00"
@"sym_name_Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}" = global [60 x i8] c"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}\00"
@"sym_name_Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}" = global [55 x i8] c"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}\00"
@"sym_name_Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [62 x i8] c"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [58 x i8] c"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" = global [66 x i8] c"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}\00"
@"sym_name_Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [62 x i8] c"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}" = global [60 x i8] c"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}\00"
@"sym_name_Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [54 x i8] c"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'UndsPipe'-'-GT-Unds'{}" = global [27 x i8] c"Lbl'UndsPipe'-'-GT-Unds'{}\00"
@"sym_name_Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [54 x i8] c"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" = global [58 x i8] c"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}\00"
@"sym_name_Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" = global [54 x i8] c"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" = global [58 x i8] c"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}\00"
@"sym_name_Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}" = global [54 x i8] c"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}\00"
@"sym_name_Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}" = global [40 x i8] c"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}\00"
@"sym_name_LblMap'Coln'lookup{}" = global [21 x i8] c"LblMap'Coln'lookup{}\00"
@"sym_name_LblSet'Coln'in{}" = global [17 x i8] c"LblSet'Coln'in{}\00"
@"sym_name_LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}" = global [46 x i8] c"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}\00"
@"sym_name_LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}" = global [85 x i8] c"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}\00"
@"sym_name_LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}" = global [48 x i8] c"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}\00"
@"sym_name_Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}" = global [86 x i8] c"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}\00"
@"sym_name_LblinitKCell{}" = global [15 x i8] c"LblinitKCell{}\00"
@"sym_name_LblinitStateCell{}" = global [19 x i8] c"LblinitStateCell{}\00"
@"sym_name_LblinitTCell{}" = global [15 x i8] c"LblinitTCell{}\00"
@"sym_name_Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}" = global [60 x i8] c"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}\00"
@"sym_name_LblisAExp{}" = global [12 x i8] c"LblisAExp{}\00"
@"sym_name_LblisBExp{}" = global [12 x i8] c"LblisBExp{}\00"
@"sym_name_LblisBlock{}" = global [13 x i8] c"LblisBlock{}\00"
@"sym_name_LblisBool{}" = global [12 x i8] c"LblisBool{}\00"
@"sym_name_LblisCell{}" = global [12 x i8] c"LblisCell{}\00"
@"sym_name_LblisId{}" = global [10 x i8] c"LblisId{}\00"
@"sym_name_LblisIds{}" = global [11 x i8] c"LblisIds{}\00"
@"sym_name_LblisInt{}" = global [11 x i8] c"LblisInt{}\00"
@"sym_name_LblisK{}" = global [9 x i8] c"LblisK{}\00"
@"sym_name_LblisKCell{}" = global [13 x i8] c"LblisKCell{}\00"
@"sym_name_LblisKCellOpt{}" = global [16 x i8] c"LblisKCellOpt{}\00"
@"sym_name_LblisKConfigVar{}" = global [18 x i8] c"LblisKConfigVar{}\00"
@"sym_name_LblisKItem{}" = global [13 x i8] c"LblisKItem{}\00"
@"sym_name_LblisKResult{}" = global [15 x i8] c"LblisKResult{}\00"
@"sym_name_LblisList{}" = global [12 x i8] c"LblisList{}\00"
@"sym_name_LblisMap{}" = global [11 x i8] c"LblisMap{}\00"
@"sym_name_LblisPgm{}" = global [11 x i8] c"LblisPgm{}\00"
@"sym_name_LblisSet{}" = global [11 x i8] c"LblisSet{}\00"
@"sym_name_LblisStateCell{}" = global [17 x i8] c"LblisStateCell{}\00"
@"sym_name_LblisStateCellOpt{}" = global [20 x i8] c"LblisStateCellOpt{}\00"
@"sym_name_LblisStmt{}" = global [12 x i8] c"LblisStmt{}\00"
@"sym_name_LblisString{}" = global [14 x i8] c"LblisString{}\00"
@"sym_name_LblisTCell{}" = global [13 x i8] c"LblisTCell{}\00"
@"sym_name_LblisTCellFragment{}" = global [21 x i8] c"LblisTCellFragment{}\00"
@"sym_name_Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}" = global [44 x i8] c"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}\00"
@"sym_name_LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}" = global [63 x i8] c"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}\00"
@"sym_name_LblnotBool'Unds'{}" = global [19 x i8] c"LblnotBool'Unds'{}\00"
@"sym_name_LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}" = global [95 x i8] c"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}\00"
@"sym_name_Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}" = global [68 x i8] c"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}\00"
@"sym_name_append{}" = global [9 x i8] c"append{}\00"
@"sym_name_dotk{}" = global [7 x i8] c"dotk{}\00"
@"sym_name_inj{SortAExp{},SortK{}}" = global [24 x i8] c"inj{SortAExp{},SortK{}}\00"
@"sym_name_inj{SortIds{},SortK{}}" = global [23 x i8] c"inj{SortIds{},SortK{}}\00"
@"sym_name_inj{SortKItem{},SortK{}}" = global [25 x i8] c"inj{SortKItem{},SortK{}}\00"
@"sym_name_inj{SortBExp{},SortKItem{}}" = global [28 x i8] c"inj{SortBExp{},SortKItem{}}\00"
@"sym_name_inj{SortStateCell{},SortK{}}" = global [29 x i8] c"inj{SortStateCell{},SortK{}}\00"
@"sym_name_inj{SortCell{},SortK{}}" = global [24 x i8] c"inj{SortCell{},SortK{}}\00"
@"sym_name_inj{SortKCell{},SortK{}}" = global [25 x i8] c"inj{SortKCell{},SortK{}}\00"
@"sym_name_inj{SortTCell{},SortK{}}" = global [25 x i8] c"inj{SortTCell{},SortK{}}\00"
@"sym_name_inj{SortInt{},SortK{}}" = global [23 x i8] c"inj{SortInt{},SortK{}}\00"
@"sym_name_inj{SortBool{},SortBExp{}}" = global [27 x i8] c"inj{SortBool{},SortBExp{}}\00"
@"sym_name_inj{SortKResult{},SortK{}}" = global [27 x i8] c"inj{SortKResult{},SortK{}}\00"
@"sym_name_inj{SortStmt{},SortKItem{}}" = global [28 x i8] c"inj{SortStmt{},SortKItem{}}\00"
@"sym_name_inj{SortBlock{},SortKItem{}}" = global [29 x i8] c"inj{SortBlock{},SortKItem{}}\00"
@"sym_name_inj{SortKCellOpt{},SortK{}}" = global [28 x i8] c"inj{SortKCellOpt{},SortK{}}\00"
@"sym_name_inj{SortList{},SortK{}}" = global [24 x i8] c"inj{SortList{},SortK{}}\00"
@"sym_name_inj{SortSet{},SortK{}}" = global [23 x i8] c"inj{SortSet{},SortK{}}\00"
@"sym_name_inj{SortPgm{},SortKItem{}}" = global [27 x i8] c"inj{SortPgm{},SortKItem{}}\00"
@"sym_name_inj{SortStateCellOpt{},SortK{}}" = global [32 x i8] c"inj{SortStateCellOpt{},SortK{}}\00"
@"sym_name_inj{SortMap{},SortK{}}" = global [23 x i8] c"inj{SortMap{},SortK{}}\00"
@"sym_name_inj{SortTCellFragment{},SortK{}}" = global [33 x i8] c"inj{SortTCellFragment{},SortK{}}\00"
@"sym_name_inj{SortString{},SortK{}}" = global [26 x i8] c"inj{SortString{},SortK{}}\00"
@"sym_name_inj{SortId{},SortK{}}" = global [22 x i8] c"inj{SortId{},SortK{}}\00"
@"sym_name_inj{SortKConfigVar{},SortK{}}" = global [30 x i8] c"inj{SortKConfigVar{},SortK{}}\00"
@"sym_name_kseq{}" = global [7 x i8] c"kseq{}\00"
@sort_name_SortList = global [9 x i8] c"SortList\00"
@sort_name_SortMap = global [8 x i8] c"SortMap\00"
@sort_name_SortBool = global [9 x i8] c"SortBool\00"
@bool_true = global [4 x i8] c"true"
@sort_name_SortInt = global [8 x i8] c"SortInt\00"
@sort_name_SortSet = global [8 x i8] c"SortSet\00"

declare %block* @parseConfiguration(i8*)

declare void @printConfiguration(i32, %block*)

define i32 @main(i32 %argc, i8** %argv) {
entry:
  %filename_ptr = getelementptr inbounds i8*, i8** %argv, i64 1
  %filename = load i8*, i8** %filename_ptr
  %ret = call %block* @parseConfiguration(i8* %filename)
  %0 = ptrtoint %block* %ret to i32
  ret i32 %0
}

define i32 @getTagForSymbolName(i8*) {
"Lbl'-LT-'T'-GT-'{}":
  %1 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"sym_name_Lbl'-LT-'T'-GT-'{}", i64 0, i64 0))
  %2 = icmp eq i32 %1, 0
  br i1 %2, label %exit, label %"Lbl'-LT-'k'-GT-'{}"

"Lbl'-LT-'k'-GT-'{}":                             ; preds = %"Lbl'-LT-'T'-GT-'{}"
  %3 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"sym_name_Lbl'-LT-'k'-GT-'{}", i64 0, i64 0))
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %exit, label %"Lbl'-LT-'state'-GT-'{}"

"Lbl'-LT-'state'-GT-'{}":                         ; preds = %"Lbl'-LT-'k'-GT-'{}"
  %5 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @"sym_name_Lbl'-LT-'state'-GT-'{}", i64 0, i64 0))
  %6 = icmp eq i32 %5, 0
  br i1 %6, label %exit, label %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}"

"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}":    ; preds = %"Lbl'-LT-'state'-GT-'{}"
  %7 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @"sym_name_Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}", i64 0, i64 0))
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %exit, label %"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}"

"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}": ; preds = %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}"
  %9 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([64 x i8], [64 x i8]* @"sym_name_Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}", i64 0, i64 0))
  %10 = icmp eq i32 %9, 0
  br i1 %10, label %exit, label %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"

"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}": ; preds = %"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}"
  %11 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([82 x i8], [82 x i8]* @"sym_name_Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", i64 0, i64 0))
  %12 = icmp eq i32 %11, 0
  br i1 %12, label %exit, label %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"

"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}": ; preds = %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"
  %13 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([82 x i8], [82 x i8]* @"sym_name_Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", i64 0, i64 0))
  %14 = icmp eq i32 %13, 0
  br i1 %14, label %exit, label %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}"

"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}": ; preds = %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"
  %15 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([80 x i8], [80 x i8]* @"sym_name_Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}", i64 0, i64 0))
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %exit, label %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}"

"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}"
  %17 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([80 x i8], [80 x i8]* @"sym_name_Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}", i64 0, i64 0))
  %18 = icmp eq i32 %17, 0
  br i1 %18, label %exit, label %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"

"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}"
  %19 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([78 x i8], [78 x i8]* @"sym_name_Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", i64 0, i64 0))
  %20 = icmp eq i32 %19, 0
  br i1 %20, label %exit, label %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"

"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"
  %21 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([78 x i8], [78 x i8]* @"sym_name_Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", i64 0, i64 0))
  %22 = icmp eq i32 %21, 0
  br i1 %22, label %exit, label %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"

"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"
  %23 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([78 x i8], [78 x i8]* @"sym_name_Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", i64 0, i64 0))
  %24 = icmp eq i32 %23, 0
  br i1 %24, label %exit, label %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"

"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}"
  %25 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([78 x i8], [78 x i8]* @"sym_name_Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", i64 0, i64 0))
  %26 = icmp eq i32 %25, 0
  br i1 %26, label %exit, label %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}"

"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}": ; preds = %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}"
  %27 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([106 x i8], [106 x i8]* @"sym_name_Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}", i64 0, i64 0))
  %28 = icmp eq i32 %27, 0
  br i1 %28, label %exit, label %"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}"

"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}": ; preds = %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}"
  %29 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([102 x i8], [102 x i8]* @"sym_name_Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}", i64 0, i64 0))
  %30 = icmp eq i32 %29, 0
  br i1 %30, label %exit, label %"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}"

"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}":            ; preds = %"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}"
  %31 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([36 x i8], [36 x i8]* @"sym_name_Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}", i64 0, i64 0))
  %32 = icmp eq i32 %31, 0
  br i1 %32, label %exit, label %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}"

"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}": ; preds = %"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}"
  %33 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([48 x i8], [48 x i8]* @"sym_name_Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}", i64 0, i64 0))
  %34 = icmp eq i32 %33, 0
  br i1 %34, label %exit, label %"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}"

"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}": ; preds = %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}"
  %35 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([90 x i8], [90 x i8]* @"sym_name_Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}", i64 0, i64 0))
  %36 = icmp eq i32 %35, 0
  br i1 %36, label %exit, label %"Lbl'Stop'Map{}"

"Lbl'Stop'Map{}":                                 ; preds = %"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}"
  %37 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @"sym_name_Lbl'Stop'Map{}", i64 0, i64 0))
  %38 = icmp eq i32 %37, 0
  br i1 %38, label %exit, label %"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Stop'Map{}"
  %39 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @"sym_name_Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %40 = icmp eq i32 %39, 0
  br i1 %40, label %exit, label %"Lbl'Unds'Map'Unds'{}"

"Lbl'Unds'Map'Unds'{}":                           ; preds = %"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %41 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @"sym_name_Lbl'Unds'Map'Unds'{}", i64 0, i64 0))
  %42 = icmp eq i32 %41, 0
  br i1 %42, label %exit, label %"Lbl'Unds'andBool'Unds'{}"

"Lbl'Unds'andBool'Unds'{}":                       ; preds = %"Lbl'Unds'Map'Unds'{}"
  %43 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @"sym_name_Lbl'Unds'andBool'Unds'{}", i64 0, i64 0))
  %44 = icmp eq i32 %43, 0
  br i1 %44, label %exit, label %"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'Unds'andBool'Unds'{}"
  %45 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([61 x i8], [61 x i8]* @"sym_name_Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %46 = icmp eq i32 %45, 0
  br i1 %46, label %exit, label %"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %47 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([53 x i8], [53 x i8]* @"sym_name_Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %48 = icmp eq i32 %47, 0
  br i1 %48, label %exit, label %"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %49 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([57 x i8], [57 x i8]* @"sym_name_Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %50 = icmp eq i32 %49, 0
  br i1 %50, label %exit, label %"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %51 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([61 x i8], [61 x i8]* @"sym_name_Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %52 = icmp eq i32 %51, 0
  br i1 %52, label %exit, label %"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %53 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([53 x i8], [53 x i8]* @"sym_name_Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %54 = icmp eq i32 %53, 0
  br i1 %54, label %exit, label %"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %55 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([56 x i8], [56 x i8]* @"sym_name_Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %56 = icmp eq i32 %55, 0
  br i1 %56, label %exit, label %"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %57 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([60 x i8], [60 x i8]* @"sym_name_Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %58 = icmp eq i32 %57, 0
  br i1 %58, label %exit, label %"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %59 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([57 x i8], [57 x i8]* @"sym_name_Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %60 = icmp eq i32 %59, 0
  br i1 %60, label %exit, label %"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %61 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %62 = icmp eq i32 %61, 0
  br i1 %62, label %exit, label %"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %63 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %64 = icmp eq i32 %63, 0
  br i1 %64, label %exit, label %"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %65 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %66 = icmp eq i32 %65, 0
  br i1 %66, label %exit, label %"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %67 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %68 = icmp eq i32 %67, 0
  br i1 %68, label %exit, label %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"

"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}": ; preds = %"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %69 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([62 x i8], [62 x i8]* @"sym_name_Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", i64 0, i64 0))
  %70 = icmp eq i32 %69, 0
  br i1 %70, label %exit, label %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}"

"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}": ; preds = %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"
  %71 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([60 x i8], [60 x i8]* @"sym_name_Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}", i64 0, i64 0))
  %72 = icmp eq i32 %71, 0
  br i1 %72, label %exit, label %"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}"

"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}": ; preds = %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}"
  %73 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([55 x i8], [55 x i8]* @"sym_name_Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}", i64 0, i64 0))
  %74 = icmp eq i32 %73, 0
  br i1 %74, label %exit, label %"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}"
  %75 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([62 x i8], [62 x i8]* @"sym_name_Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %76 = icmp eq i32 %75, 0
  br i1 %76, label %exit, label %"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %77 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %78 = icmp eq i32 %77, 0
  br i1 %78, label %exit, label %"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"

"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}": ; preds = %"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %79 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([66 x i8], [66 x i8]* @"sym_name_Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", i64 0, i64 0))
  %80 = icmp eq i32 %79, 0
  br i1 %80, label %exit, label %"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}"
  %81 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([62 x i8], [62 x i8]* @"sym_name_Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %82 = icmp eq i32 %81, 0
  br i1 %82, label %exit, label %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}"

"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}": ; preds = %"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %83 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([60 x i8], [60 x i8]* @"sym_name_Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}", i64 0, i64 0))
  %84 = icmp eq i32 %83, 0
  br i1 %84, label %exit, label %"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}"
  %85 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @"sym_name_Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %86 = icmp eq i32 %85, 0
  br i1 %86, label %exit, label %"Lbl'UndsPipe'-'-GT-Unds'{}"

"Lbl'UndsPipe'-'-GT-Unds'{}":                     ; preds = %"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %87 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @"sym_name_Lbl'UndsPipe'-'-GT-Unds'{}", i64 0, i64 0))
  %88 = icmp eq i32 %87, 0
  br i1 %88, label %exit, label %"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'UndsPipe'-'-GT-Unds'{}"
  %89 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @"sym_name_Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %90 = icmp eq i32 %89, 0
  br i1 %90, label %exit, label %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"

"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}": ; preds = %"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %91 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", i64 0, i64 0))
  %92 = icmp eq i32 %91, 0
  br i1 %92, label %exit, label %"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"

"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"
  %93 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @"sym_name_Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %94 = icmp eq i32 %93, 0
  br i1 %94, label %exit, label %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"

"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}": ; preds = %"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}"
  %95 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([58 x i8], [58 x i8]* @"sym_name_Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", i64 0, i64 0))
  %96 = icmp eq i32 %95, 0
  br i1 %96, label %exit, label %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}"

"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}": ; preds = %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}"
  %97 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @"sym_name_Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}", i64 0, i64 0))
  %98 = icmp eq i32 %97, 0
  br i1 %98, label %exit, label %"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}"

"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}":        ; preds = %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}"
  %99 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([40 x i8], [40 x i8]* @"sym_name_Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}", i64 0, i64 0))
  %100 = icmp eq i32 %99, 0
  br i1 %100, label %exit, label %"LblMap'Coln'lookup{}"

"LblMap'Coln'lookup{}":                           ; preds = %"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}"
  %101 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @"sym_name_LblMap'Coln'lookup{}", i64 0, i64 0))
  %102 = icmp eq i32 %101, 0
  br i1 %102, label %exit, label %"LblSet'Coln'in{}"

"LblSet'Coln'in{}":                               ; preds = %"LblMap'Coln'lookup{}"
  %103 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @"sym_name_LblSet'Coln'in{}", i64 0, i64 0))
  %104 = icmp eq i32 %103, 0
  br i1 %104, label %exit, label %"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}"

"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}":  ; preds = %"LblSet'Coln'in{}"
  %105 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([46 x i8], [46 x i8]* @"sym_name_LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}", i64 0, i64 0))
  %106 = icmp eq i32 %105, 0
  br i1 %106, label %exit, label %"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}"

"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}": ; preds = %"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}"
  %107 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([85 x i8], [85 x i8]* @"sym_name_LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}", i64 0, i64 0))
  %108 = icmp eq i32 %107, 0
  br i1 %108, label %exit, label %"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}"

"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}": ; preds = %"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}"
  %109 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([48 x i8], [48 x i8]* @"sym_name_LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}", i64 0, i64 0))
  %110 = icmp eq i32 %109, 0
  br i1 %110, label %exit, label %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}"

"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}": ; preds = %"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}"
  %111 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([86 x i8], [86 x i8]* @"sym_name_Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}", i64 0, i64 0))
  %112 = icmp eq i32 %111, 0
  br i1 %112, label %exit, label %"LblinitKCell{}"

"LblinitKCell{}":                                 ; preds = %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}"
  %113 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @"sym_name_LblinitKCell{}", i64 0, i64 0))
  %114 = icmp eq i32 %113, 0
  br i1 %114, label %exit, label %"LblinitStateCell{}"

"LblinitStateCell{}":                             ; preds = %"LblinitKCell{}"
  %115 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"sym_name_LblinitStateCell{}", i64 0, i64 0))
  %116 = icmp eq i32 %115, 0
  br i1 %116, label %exit, label %"LblinitTCell{}"

"LblinitTCell{}":                                 ; preds = %"LblinitStateCell{}"
  %117 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @"sym_name_LblinitTCell{}", i64 0, i64 0))
  %118 = icmp eq i32 %117, 0
  br i1 %118, label %exit, label %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}"

"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}": ; preds = %"LblinitTCell{}"
  %119 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([60 x i8], [60 x i8]* @"sym_name_Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}", i64 0, i64 0))
  %120 = icmp eq i32 %119, 0
  br i1 %120, label %exit, label %"LblisAExp{}"

"LblisAExp{}":                                    ; preds = %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}"
  %121 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisAExp{}", i64 0, i64 0))
  %122 = icmp eq i32 %121, 0
  br i1 %122, label %exit, label %"LblisBExp{}"

"LblisBExp{}":                                    ; preds = %"LblisAExp{}"
  %123 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisBExp{}", i64 0, i64 0))
  %124 = icmp eq i32 %123, 0
  br i1 %124, label %exit, label %"LblisBlock{}"

"LblisBlock{}":                                   ; preds = %"LblisBExp{}"
  %125 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"sym_name_LblisBlock{}", i64 0, i64 0))
  %126 = icmp eq i32 %125, 0
  br i1 %126, label %exit, label %"LblisBool{}"

"LblisBool{}":                                    ; preds = %"LblisBlock{}"
  %127 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisBool{}", i64 0, i64 0))
  %128 = icmp eq i32 %127, 0
  br i1 %128, label %exit, label %"LblisCell{}"

"LblisCell{}":                                    ; preds = %"LblisBool{}"
  %129 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisCell{}", i64 0, i64 0))
  %130 = icmp eq i32 %129, 0
  br i1 %130, label %exit, label %"LblisId{}"

"LblisId{}":                                      ; preds = %"LblisCell{}"
  %131 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @"sym_name_LblisId{}", i64 0, i64 0))
  %132 = icmp eq i32 %131, 0
  br i1 %132, label %exit, label %"LblisIds{}"

"LblisIds{}":                                     ; preds = %"LblisId{}"
  %133 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"sym_name_LblisIds{}", i64 0, i64 0))
  %134 = icmp eq i32 %133, 0
  br i1 %134, label %exit, label %"LblisInt{}"

"LblisInt{}":                                     ; preds = %"LblisIds{}"
  %135 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"sym_name_LblisInt{}", i64 0, i64 0))
  %136 = icmp eq i32 %135, 0
  br i1 %136, label %exit, label %"LblisK{}"

"LblisK{}":                                       ; preds = %"LblisInt{}"
  %137 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @"sym_name_LblisK{}", i64 0, i64 0))
  %138 = icmp eq i32 %137, 0
  br i1 %138, label %exit, label %"LblisKCell{}"

"LblisKCell{}":                                   ; preds = %"LblisK{}"
  %139 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"sym_name_LblisKCell{}", i64 0, i64 0))
  %140 = icmp eq i32 %139, 0
  br i1 %140, label %exit, label %"LblisKCellOpt{}"

"LblisKCellOpt{}":                                ; preds = %"LblisKCell{}"
  %141 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @"sym_name_LblisKCellOpt{}", i64 0, i64 0))
  %142 = icmp eq i32 %141, 0
  br i1 %142, label %exit, label %"LblisKConfigVar{}"

"LblisKConfigVar{}":                              ; preds = %"LblisKCellOpt{}"
  %143 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @"sym_name_LblisKConfigVar{}", i64 0, i64 0))
  %144 = icmp eq i32 %143, 0
  br i1 %144, label %exit, label %"LblisKItem{}"

"LblisKItem{}":                                   ; preds = %"LblisKConfigVar{}"
  %145 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"sym_name_LblisKItem{}", i64 0, i64 0))
  %146 = icmp eq i32 %145, 0
  br i1 %146, label %exit, label %"LblisKResult{}"

"LblisKResult{}":                                 ; preds = %"LblisKItem{}"
  %147 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @"sym_name_LblisKResult{}", i64 0, i64 0))
  %148 = icmp eq i32 %147, 0
  br i1 %148, label %exit, label %"LblisList{}"

"LblisList{}":                                    ; preds = %"LblisKResult{}"
  %149 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisList{}", i64 0, i64 0))
  %150 = icmp eq i32 %149, 0
  br i1 %150, label %exit, label %"LblisMap{}"

"LblisMap{}":                                     ; preds = %"LblisList{}"
  %151 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"sym_name_LblisMap{}", i64 0, i64 0))
  %152 = icmp eq i32 %151, 0
  br i1 %152, label %exit, label %"LblisPgm{}"

"LblisPgm{}":                                     ; preds = %"LblisMap{}"
  %153 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"sym_name_LblisPgm{}", i64 0, i64 0))
  %154 = icmp eq i32 %153, 0
  br i1 %154, label %exit, label %"LblisSet{}"

"LblisSet{}":                                     ; preds = %"LblisPgm{}"
  %155 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"sym_name_LblisSet{}", i64 0, i64 0))
  %156 = icmp eq i32 %155, 0
  br i1 %156, label %exit, label %"LblisStateCell{}"

"LblisStateCell{}":                               ; preds = %"LblisSet{}"
  %157 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @"sym_name_LblisStateCell{}", i64 0, i64 0))
  %158 = icmp eq i32 %157, 0
  br i1 %158, label %exit, label %"LblisStateCellOpt{}"

"LblisStateCellOpt{}":                            ; preds = %"LblisStateCell{}"
  %159 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([20 x i8], [20 x i8]* @"sym_name_LblisStateCellOpt{}", i64 0, i64 0))
  %160 = icmp eq i32 %159, 0
  br i1 %160, label %exit, label %"LblisStmt{}"

"LblisStmt{}":                                    ; preds = %"LblisStateCellOpt{}"
  %161 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"sym_name_LblisStmt{}", i64 0, i64 0))
  %162 = icmp eq i32 %161, 0
  br i1 %162, label %exit, label %"LblisString{}"

"LblisString{}":                                  ; preds = %"LblisStmt{}"
  %163 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @"sym_name_LblisString{}", i64 0, i64 0))
  %164 = icmp eq i32 %163, 0
  br i1 %164, label %exit, label %"LblisTCell{}"

"LblisTCell{}":                                   ; preds = %"LblisString{}"
  %165 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"sym_name_LblisTCell{}", i64 0, i64 0))
  %166 = icmp eq i32 %165, 0
  br i1 %166, label %exit, label %"LblisTCellFragment{}"

"LblisTCellFragment{}":                           ; preds = %"LblisTCell{}"
  %167 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @"sym_name_LblisTCellFragment{}", i64 0, i64 0))
  %168 = icmp eq i32 %167, 0
  br i1 %168, label %exit, label %"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}"

"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}":    ; preds = %"LblisTCellFragment{}"
  %169 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @"sym_name_Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}", i64 0, i64 0))
  %170 = icmp eq i32 %169, 0
  br i1 %170, label %exit, label %"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}"

"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}": ; preds = %"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}"
  %171 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([63 x i8], [63 x i8]* @"sym_name_LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}", i64 0, i64 0))
  %172 = icmp eq i32 %171, 0
  br i1 %172, label %exit, label %"LblnotBool'Unds'{}"

"LblnotBool'Unds'{}":                             ; preds = %"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}"
  %173 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @"sym_name_LblnotBool'Unds'{}", i64 0, i64 0))
  %174 = icmp eq i32 %173, 0
  br i1 %174, label %exit, label %"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}"

"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}": ; preds = %"LblnotBool'Unds'{}"
  %175 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([95 x i8], [95 x i8]* @"sym_name_LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}", i64 0, i64 0))
  %176 = icmp eq i32 %175, 0
  br i1 %176, label %exit, label %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}"

"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}": ; preds = %"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}"
  %177 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([68 x i8], [68 x i8]* @"sym_name_Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}", i64 0, i64 0))
  %178 = icmp eq i32 %177, 0
  br i1 %178, label %exit, label %"append{}"

"append{}":                                       ; preds = %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}"
  %179 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @"sym_name_append{}", i64 0, i64 0))
  %180 = icmp eq i32 %179, 0
  br i1 %180, label %exit, label %"dotk{}"

"dotk{}":                                         ; preds = %"append{}"
  %181 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @"sym_name_dotk{}", i64 0, i64 0))
  %182 = icmp eq i32 %181, 0
  br i1 %182, label %exit, label %"inj{SortAExp{},SortK{}}"

"inj{SortAExp{},SortK{}}":                        ; preds = %"dotk{}"
  %183 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @"sym_name_inj{SortAExp{},SortK{}}", i64 0, i64 0))
  %184 = icmp eq i32 %183, 0
  br i1 %184, label %exit, label %"inj{SortIds{},SortK{}}"

"inj{SortIds{},SortK{}}":                         ; preds = %"inj{SortAExp{},SortK{}}"
  %185 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @"sym_name_inj{SortIds{},SortK{}}", i64 0, i64 0))
  %186 = icmp eq i32 %185, 0
  br i1 %186, label %exit, label %"inj{SortKItem{},SortK{}}"

"inj{SortKItem{},SortK{}}":                       ; preds = %"inj{SortIds{},SortK{}}"
  %187 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @"sym_name_inj{SortKItem{},SortK{}}", i64 0, i64 0))
  %188 = icmp eq i32 %187, 0
  br i1 %188, label %exit, label %"inj{SortBExp{},SortKItem{}}"

"inj{SortBExp{},SortKItem{}}":                    ; preds = %"inj{SortKItem{},SortK{}}"
  %189 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @"sym_name_inj{SortBExp{},SortKItem{}}", i64 0, i64 0))
  %190 = icmp eq i32 %189, 0
  br i1 %190, label %exit, label %"inj{SortStateCell{},SortK{}}"

"inj{SortStateCell{},SortK{}}":                   ; preds = %"inj{SortBExp{},SortKItem{}}"
  %191 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @"sym_name_inj{SortStateCell{},SortK{}}", i64 0, i64 0))
  %192 = icmp eq i32 %191, 0
  br i1 %192, label %exit, label %"inj{SortCell{},SortK{}}"

"inj{SortCell{},SortK{}}":                        ; preds = %"inj{SortStateCell{},SortK{}}"
  %193 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @"sym_name_inj{SortCell{},SortK{}}", i64 0, i64 0))
  %194 = icmp eq i32 %193, 0
  br i1 %194, label %exit, label %"inj{SortKCell{},SortK{}}"

"inj{SortKCell{},SortK{}}":                       ; preds = %"inj{SortCell{},SortK{}}"
  %195 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @"sym_name_inj{SortKCell{},SortK{}}", i64 0, i64 0))
  %196 = icmp eq i32 %195, 0
  br i1 %196, label %exit, label %"inj{SortTCell{},SortK{}}"

"inj{SortTCell{},SortK{}}":                       ; preds = %"inj{SortKCell{},SortK{}}"
  %197 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @"sym_name_inj{SortTCell{},SortK{}}", i64 0, i64 0))
  %198 = icmp eq i32 %197, 0
  br i1 %198, label %exit, label %"inj{SortInt{},SortK{}}"

"inj{SortInt{},SortK{}}":                         ; preds = %"inj{SortTCell{},SortK{}}"
  %199 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @"sym_name_inj{SortInt{},SortK{}}", i64 0, i64 0))
  %200 = icmp eq i32 %199, 0
  br i1 %200, label %exit, label %"inj{SortBool{},SortBExp{}}"

"inj{SortBool{},SortBExp{}}":                     ; preds = %"inj{SortInt{},SortK{}}"
  %201 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @"sym_name_inj{SortBool{},SortBExp{}}", i64 0, i64 0))
  %202 = icmp eq i32 %201, 0
  br i1 %202, label %exit, label %"inj{SortKResult{},SortK{}}"

"inj{SortKResult{},SortK{}}":                     ; preds = %"inj{SortBool{},SortBExp{}}"
  %203 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @"sym_name_inj{SortKResult{},SortK{}}", i64 0, i64 0))
  %204 = icmp eq i32 %203, 0
  br i1 %204, label %exit, label %"inj{SortStmt{},SortKItem{}}"

"inj{SortStmt{},SortKItem{}}":                    ; preds = %"inj{SortKResult{},SortK{}}"
  %205 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @"sym_name_inj{SortStmt{},SortKItem{}}", i64 0, i64 0))
  %206 = icmp eq i32 %205, 0
  br i1 %206, label %exit, label %"inj{SortBlock{},SortKItem{}}"

"inj{SortBlock{},SortKItem{}}":                   ; preds = %"inj{SortStmt{},SortKItem{}}"
  %207 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @"sym_name_inj{SortBlock{},SortKItem{}}", i64 0, i64 0))
  %208 = icmp eq i32 %207, 0
  br i1 %208, label %exit, label %"inj{SortKCellOpt{},SortK{}}"

"inj{SortKCellOpt{},SortK{}}":                    ; preds = %"inj{SortBlock{},SortKItem{}}"
  %209 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @"sym_name_inj{SortKCellOpt{},SortK{}}", i64 0, i64 0))
  %210 = icmp eq i32 %209, 0
  br i1 %210, label %exit, label %"inj{SortList{},SortK{}}"

"inj{SortList{},SortK{}}":                        ; preds = %"inj{SortKCellOpt{},SortK{}}"
  %211 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @"sym_name_inj{SortList{},SortK{}}", i64 0, i64 0))
  %212 = icmp eq i32 %211, 0
  br i1 %212, label %exit, label %"inj{SortSet{},SortK{}}"

"inj{SortSet{},SortK{}}":                         ; preds = %"inj{SortList{},SortK{}}"
  %213 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @"sym_name_inj{SortSet{},SortK{}}", i64 0, i64 0))
  %214 = icmp eq i32 %213, 0
  br i1 %214, label %exit, label %"inj{SortPgm{},SortKItem{}}"

"inj{SortPgm{},SortKItem{}}":                     ; preds = %"inj{SortSet{},SortK{}}"
  %215 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @"sym_name_inj{SortPgm{},SortKItem{}}", i64 0, i64 0))
  %216 = icmp eq i32 %215, 0
  br i1 %216, label %exit, label %"inj{SortStateCellOpt{},SortK{}}"

"inj{SortStateCellOpt{},SortK{}}":                ; preds = %"inj{SortPgm{},SortKItem{}}"
  %217 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([32 x i8], [32 x i8]* @"sym_name_inj{SortStateCellOpt{},SortK{}}", i64 0, i64 0))
  %218 = icmp eq i32 %217, 0
  br i1 %218, label %exit, label %"inj{SortMap{},SortK{}}"

"inj{SortMap{},SortK{}}":                         ; preds = %"inj{SortStateCellOpt{},SortK{}}"
  %219 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @"sym_name_inj{SortMap{},SortK{}}", i64 0, i64 0))
  %220 = icmp eq i32 %219, 0
  br i1 %220, label %exit, label %"inj{SortTCellFragment{},SortK{}}"

"inj{SortTCellFragment{},SortK{}}":               ; preds = %"inj{SortMap{},SortK{}}"
  %221 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([33 x i8], [33 x i8]* @"sym_name_inj{SortTCellFragment{},SortK{}}", i64 0, i64 0))
  %222 = icmp eq i32 %221, 0
  br i1 %222, label %exit, label %"inj{SortString{},SortK{}}"

"inj{SortString{},SortK{}}":                      ; preds = %"inj{SortTCellFragment{},SortK{}}"
  %223 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @"sym_name_inj{SortString{},SortK{}}", i64 0, i64 0))
  %224 = icmp eq i32 %223, 0
  br i1 %224, label %exit, label %"inj{SortId{},SortK{}}"

"inj{SortId{},SortK{}}":                          ; preds = %"inj{SortString{},SortK{}}"
  %225 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @"sym_name_inj{SortId{},SortK{}}", i64 0, i64 0))
  %226 = icmp eq i32 %225, 0
  br i1 %226, label %exit, label %"inj{SortKConfigVar{},SortK{}}"

"inj{SortKConfigVar{},SortK{}}":                  ; preds = %"inj{SortId{},SortK{}}"
  %227 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([30 x i8], [30 x i8]* @"sym_name_inj{SortKConfigVar{},SortK{}}", i64 0, i64 0))
  %228 = icmp eq i32 %227, 0
  br i1 %228, label %exit, label %"kseq{}"

"kseq{}":                                         ; preds = %"inj{SortKConfigVar{},SortK{}}"
  %229 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @"sym_name_kseq{}", i64 0, i64 0))
  %230 = icmp eq i32 %229, 0
  br i1 %230, label %exit, label %stuck

exit:                                             ; preds = %"kseq{}", %"inj{SortKConfigVar{},SortK{}}", %"inj{SortId{},SortK{}}", %"inj{SortString{},SortK{}}", %"inj{SortTCellFragment{},SortK{}}", %"inj{SortMap{},SortK{}}", %"inj{SortStateCellOpt{},SortK{}}", %"inj{SortPgm{},SortKItem{}}", %"inj{SortSet{},SortK{}}", %"inj{SortList{},SortK{}}", %"inj{SortKCellOpt{},SortK{}}", %"inj{SortBlock{},SortKItem{}}", %"inj{SortStmt{},SortKItem{}}", %"inj{SortKResult{},SortK{}}", %"inj{SortBool{},SortBExp{}}", %"inj{SortInt{},SortK{}}", %"inj{SortTCell{},SortK{}}", %"inj{SortKCell{},SortK{}}", %"inj{SortCell{},SortK{}}", %"inj{SortStateCell{},SortK{}}", %"inj{SortBExp{},SortKItem{}}", %"inj{SortKItem{},SortK{}}", %"inj{SortIds{},SortK{}}", %"inj{SortAExp{},SortK{}}", %"dotk{}", %"append{}", %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}", %"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}", %"LblnotBool'Unds'{}", %"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}", %"LblisTCellFragment{}", %"LblisTCell{}", %"LblisString{}", %"LblisStmt{}", %"LblisStateCellOpt{}", %"LblisStateCell{}", %"LblisSet{}", %"LblisPgm{}", %"LblisMap{}", %"LblisList{}", %"LblisKResult{}", %"LblisKItem{}", %"LblisKConfigVar{}", %"LblisKCellOpt{}", %"LblisKCell{}", %"LblisK{}", %"LblisInt{}", %"LblisIds{}", %"LblisId{}", %"LblisCell{}", %"LblisBool{}", %"LblisBlock{}", %"LblisBExp{}", %"LblisAExp{}", %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}", %"LblinitTCell{}", %"LblinitStateCell{}", %"LblinitKCell{}", %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}", %"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}", %"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}", %"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}", %"LblSet'Coln'in{}", %"LblMap'Coln'lookup{}", %"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}", %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}", %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", %"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", %"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'UndsPipe'-'-GT-Unds'{}", %"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}", %"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}", %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}", %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}", %"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}", %"Lbl'Unds'andBool'Unds'{}", %"Lbl'Unds'Map'Unds'{}", %"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}", %"Lbl'Stop'Map{}", %"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}", %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}", %"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}", %"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}", %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}", %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}", %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}", %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}", %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}", %"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}", %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}", %"Lbl'-LT-'state'-GT-'{}", %"Lbl'-LT-'k'-GT-'{}", %"Lbl'-LT-'T'-GT-'{}"
  %phi = phi i32 [ 0, %"Lbl'-LT-'T'-GT-'{}" ], [ 1, %"Lbl'-LT-'k'-GT-'{}" ], [ 2, %"Lbl'-LT-'state'-GT-'{}" ], [ 3, %"Lbl'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp{}" ], [ 4, %"Lbl'Hash'freezer'BangUndsUnds'IMP-SYNTAX'UndsUnds'BExp0'Unds'{}" ], [ 5, %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" ], [ 6, %"Lbl'Hash'freezer'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" ], [ 7, %"Lbl'Hash'freezer'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp1'Unds'{}" ], [ 8, %"Lbl'Hash'freezer'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp0'Unds'{}" ], [ 9, %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" ], [ 10, %"Lbl'Hash'freezer'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" ], [ 11, %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp0'Unds'{}" ], [ 12, %"Lbl'Hash'freezer'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp1'Unds'{}" ], [ 13, %"Lbl'Hash'freezerif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block1'Unds'{}" ], [ 14, %"Lbl'Hash'if'UndsHash'then'UndsHash'else'UndsHash'fi'Unds'K-EQUAL'UndsUnds'Bool'Unds'K'Unds'K{SortK{}}" ], [ 15, %"Lbl'LBraRBraUnds'IMP-SYNTAX'Unds'{}" ], [ 16, %"Lbl'LBraUndsRBraUnds'IMP-SYNTAX'UndsUnds'Stmt{}" ], [ 17, %"Lbl'Stop'List'LBraQuotUndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids'QuotRBraUnds'Ids{}" ], [ 18, %"Lbl'Stop'Map{}" ], [ 19, %"Lbl'Unds'-Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 20, %"Lbl'Unds'Map'Unds'{}" ], [ 21, %"Lbl'Unds'andBool'Unds'{}" ], [ 22, %"Lbl'Unds'andThenBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 23, %"Lbl'Unds'divInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 24, %"Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 25, %"Lbl'Unds'impliesBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 26, %"Lbl'Unds'modInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 27, %"Lbl'Unds'orBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 28, %"Lbl'Unds'orElseBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 29, %"Lbl'Unds'xorBool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 30, %"Lbl'Unds-GT--GT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 31, %"Lbl'Unds-GT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 32, %"Lbl'Unds-LT--LT-'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 33, %"Lbl'Unds-LT-Eqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 34, %"Lbl'Unds-LT-EqlsUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" ], [ 35, %"Lbl'UndsAndAndUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'BExp{}" ], [ 36, %"Lbl'UndsCommUndsUnds'IMP-SYNTAX'UndsUnds'Id'Unds'Ids{}" ], [ 37, %"Lbl'UndsEqlsEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 38, %"Lbl'UndsEqlsEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 39, %"Lbl'UndsEqlsSlshEqls'Bool'UndsUnds'BOOL'UndsUnds'Bool'Unds'Bool{}" ], [ 40, %"Lbl'UndsEqlsSlshEqls'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 41, %"Lbl'UndsEqlsUndsSClnUnds'IMP-SYNTAX'UndsUnds'Id'Unds'AExp{}" ], [ 42, %"Lbl'UndsPerc'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 43, %"Lbl'UndsPipe'-'-GT-Unds'{}" ], [ 44, %"Lbl'UndsPlus'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 45, %"Lbl'UndsPlusUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" ], [ 46, %"Lbl'UndsSlsh'Int'UndsUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 47, %"Lbl'UndsSlshUndsUnds'IMP-SYNTAX'UndsUnds'AExp'Unds'AExp{}" ], [ 48, %"Lbl'UndsUndsUnds'IMP-SYNTAX'UndsUnds'Stmt'Unds'Stmt{}" ], [ 49, %"Lbl-'UndsUnds'IMP-SYNTAX'UndsUnds'Int{}" ], [ 50, %"LblMap'Coln'lookup{}" ], [ 51, %"LblSet'Coln'in{}" ], [ 52, %"LblabsInt'LParUndsRParUnds'INT'UndsUnds'Int{}" ], [ 53, %"LblbitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}" ], [ 54, %"LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{}" ], [ 55, %"Lblif'LParUndsRParUnds'else'UndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block'Unds'Block{}" ], [ 56, %"LblinitKCell{}" ], [ 57, %"LblinitStateCell{}" ], [ 58, %"LblinitTCell{}" ], [ 59, %"Lblint'UndsSClnUndsUnds'IMP-SYNTAX'UndsUnds'Ids'Unds'Stmt{}" ], [ 60, %"LblisAExp{}" ], [ 61, %"LblisBExp{}" ], [ 62, %"LblisBlock{}" ], [ 63, %"LblisBool{}" ], [ 64, %"LblisCell{}" ], [ 65, %"LblisId{}" ], [ 66, %"LblisIds{}" ], [ 67, %"LblisInt{}" ], [ 68, %"LblisK{}" ], [ 69, %"LblisKCell{}" ], [ 70, %"LblisKCellOpt{}" ], [ 71, %"LblisKConfigVar{}" ], [ 72, %"LblisKItem{}" ], [ 73, %"LblisKResult{}" ], [ 74, %"LblisList{}" ], [ 75, %"LblisMap{}" ], [ 76, %"LblisPgm{}" ], [ 77, %"LblisSet{}" ], [ 78, %"LblisStateCell{}" ], [ 79, %"LblisStateCellOpt{}" ], [ 80, %"LblisStmt{}" ], [ 81, %"LblisString{}" ], [ 82, %"LblisTCell{}" ], [ 83, %"LblisTCellFragment{}" ], [ 84, %"Lblkeys'LParUndsRParUnds'MAP'UndsUnds'Map{}" ], [ 85, %"LblminInt'LParUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int{}" ], [ 86, %"LblnotBool'Unds'{}" ], [ 87, %"LblsignExtendBitRangeInt'LParUndsCommUndsCommUndsRParUnds'INT'UndsUnds'Int'Unds'Int'Unds'Int{}" ], [ 88, %"Lblwhile'LParUndsRParUndsUnds'IMP-SYNTAX'UndsUnds'BExp'Unds'Block{}" ], [ 89, %"append{}" ], [ 90, %"dotk{}" ], [ 91, %"inj{SortAExp{},SortK{}}" ], [ 92, %"inj{SortIds{},SortK{}}" ], [ 93, %"inj{SortKItem{},SortK{}}" ], [ 94, %"inj{SortBExp{},SortKItem{}}" ], [ 95, %"inj{SortStateCell{},SortK{}}" ], [ 96, %"inj{SortCell{},SortK{}}" ], [ 97, %"inj{SortKCell{},SortK{}}" ], [ 98, %"inj{SortTCell{},SortK{}}" ], [ 99, %"inj{SortInt{},SortK{}}" ], [ 100, %"inj{SortBool{},SortBExp{}}" ], [ 101, %"inj{SortKResult{},SortK{}}" ], [ 102, %"inj{SortStmt{},SortKItem{}}" ], [ 103, %"inj{SortBlock{},SortKItem{}}" ], [ 104, %"inj{SortKCellOpt{},SortK{}}" ], [ 105, %"inj{SortList{},SortK{}}" ], [ 106, %"inj{SortSet{},SortK{}}" ], [ 107, %"inj{SortPgm{},SortKItem{}}" ], [ 108, %"inj{SortStateCellOpt{},SortK{}}" ], [ 109, %"inj{SortMap{},SortK{}}" ], [ 110, %"inj{SortTCellFragment{},SortK{}}" ], [ 111, %"inj{SortString{},SortK{}}" ], [ 112, %"inj{SortId{},SortK{}}" ], [ 113, %"inj{SortKConfigVar{},SortK{}}" ], [ 114, %"kseq{}" ]
  ret i32 %phi

stuck:                                            ; preds = %"kseq{}"
  call void @abort()
  unreachable
}

declare i32 @strcmp(i8*, i8*)

; Function Attrs: noreturn
declare void @abort() #0

define %blockheader @getBlockHeaderForSymbol(i32) {
entry:
  switch i32 %0, label %stuck [
    i32 0, label %tag0
    i32 1, label %tag1
    i32 2, label %tag2
    i32 3, label %tag3
    i32 4, label %tag4
    i32 5, label %tag5
    i32 6, label %tag6
    i32 7, label %tag7
    i32 8, label %tag8
    i32 9, label %tag9
    i32 10, label %tag10
    i32 11, label %tag11
    i32 12, label %tag12
    i32 13, label %tag13
    i32 14, label %tag14
    i32 15, label %tag15
    i32 16, label %tag16
    i32 17, label %tag17
    i32 18, label %tag18
    i32 19, label %tag19
    i32 20, label %tag20
    i32 21, label %tag21
    i32 22, label %tag22
    i32 23, label %tag23
    i32 24, label %tag24
    i32 25, label %tag25
    i32 26, label %tag26
    i32 27, label %tag27
    i32 28, label %tag28
    i32 29, label %tag29
    i32 30, label %tag30
    i32 31, label %tag31
    i32 32, label %tag32
    i32 33, label %tag33
    i32 34, label %tag34
    i32 35, label %tag35
    i32 36, label %tag36
    i32 37, label %tag37
    i32 38, label %tag38
    i32 39, label %tag39
    i32 40, label %tag40
    i32 41, label %tag41
    i32 42, label %tag42
    i32 43, label %tag43
    i32 44, label %tag44
    i32 45, label %tag45
    i32 46, label %tag46
    i32 47, label %tag47
    i32 48, label %tag48
    i32 49, label %tag49
    i32 50, label %tag50
    i32 51, label %tag51
    i32 52, label %tag52
    i32 53, label %tag53
    i32 54, label %tag54
    i32 55, label %tag55
    i32 56, label %tag56
    i32 57, label %tag57
    i32 58, label %tag58
    i32 59, label %tag59
    i32 60, label %tag60
    i32 61, label %tag61
    i32 62, label %tag62
    i32 63, label %tag63
    i32 64, label %tag64
    i32 65, label %tag65
    i32 66, label %tag66
    i32 67, label %tag67
    i32 68, label %tag68
    i32 69, label %tag69
    i32 70, label %tag70
    i32 71, label %tag71
    i32 72, label %tag72
    i32 73, label %tag73
    i32 74, label %tag74
    i32 75, label %tag75
    i32 76, label %tag76
    i32 77, label %tag77
    i32 78, label %tag78
    i32 79, label %tag79
    i32 80, label %tag80
    i32 81, label %tag81
    i32 82, label %tag82
    i32 83, label %tag83
    i32 84, label %tag84
    i32 85, label %tag85
    i32 86, label %tag86
    i32 87, label %tag87
    i32 88, label %tag88
    i32 89, label %tag89
    i32 90, label %tag90
    i32 91, label %tag91
    i32 92, label %tag92
    i32 93, label %tag93
    i32 94, label %tag94
    i32 95, label %tag95
    i32 96, label %tag96
    i32 97, label %tag97
    i32 98, label %tag98
    i32 99, label %tag99
    i32 100, label %tag100
    i32 101, label %tag101
    i32 102, label %tag102
    i32 103, label %tag103
    i32 104, label %tag104
    i32 105, label %tag105
    i32 106, label %tag106
    i32 107, label %tag107
    i32 108, label %tag108
    i32 109, label %tag109
    i32 110, label %tag110
    i32 111, label %tag111
    i32 112, label %tag112
    i32 113, label %tag113
    i32 114, label %tag114
  ]

tag0:                                             ; preds = %entry
  br label %exit

tag1:                                             ; preds = %entry
  br label %exit

tag2:                                             ; preds = %entry
  br label %exit

tag3:                                             ; preds = %entry
  br label %exit

tag4:                                             ; preds = %entry
  br label %exit

tag5:                                             ; preds = %entry
  br label %exit

tag6:                                             ; preds = %entry
  br label %exit

tag7:                                             ; preds = %entry
  br label %exit

tag8:                                             ; preds = %entry
  br label %exit

tag9:                                             ; preds = %entry
  br label %exit

tag10:                                            ; preds = %entry
  br label %exit

tag11:                                            ; preds = %entry
  br label %exit

tag12:                                            ; preds = %entry
  br label %exit

tag13:                                            ; preds = %entry
  br label %exit

tag14:                                            ; preds = %entry
  br label %exit

tag15:                                            ; preds = %entry
  br label %exit

tag16:                                            ; preds = %entry
  br label %exit

tag17:                                            ; preds = %entry
  br label %exit

tag18:                                            ; preds = %entry
  br label %exit

tag19:                                            ; preds = %entry
  br label %exit

tag20:                                            ; preds = %entry
  br label %exit

tag21:                                            ; preds = %entry
  br label %exit

tag22:                                            ; preds = %entry
  br label %exit

tag23:                                            ; preds = %entry
  br label %exit

tag24:                                            ; preds = %entry
  br label %exit

tag25:                                            ; preds = %entry
  br label %exit

tag26:                                            ; preds = %entry
  br label %exit

tag27:                                            ; preds = %entry
  br label %exit

tag28:                                            ; preds = %entry
  br label %exit

tag29:                                            ; preds = %entry
  br label %exit

tag30:                                            ; preds = %entry
  br label %exit

tag31:                                            ; preds = %entry
  br label %exit

tag32:                                            ; preds = %entry
  br label %exit

tag33:                                            ; preds = %entry
  br label %exit

tag34:                                            ; preds = %entry
  br label %exit

tag35:                                            ; preds = %entry
  br label %exit

tag36:                                            ; preds = %entry
  br label %exit

tag37:                                            ; preds = %entry
  br label %exit

tag38:                                            ; preds = %entry
  br label %exit

tag39:                                            ; preds = %entry
  br label %exit

tag40:                                            ; preds = %entry
  br label %exit

tag41:                                            ; preds = %entry
  br label %exit

tag42:                                            ; preds = %entry
  br label %exit

tag43:                                            ; preds = %entry
  br label %exit

tag44:                                            ; preds = %entry
  br label %exit

tag45:                                            ; preds = %entry
  br label %exit

tag46:                                            ; preds = %entry
  br label %exit

tag47:                                            ; preds = %entry
  br label %exit

tag48:                                            ; preds = %entry
  br label %exit

tag49:                                            ; preds = %entry
  br label %exit

tag50:                                            ; preds = %entry
  br label %exit

tag51:                                            ; preds = %entry
  br label %exit

tag52:                                            ; preds = %entry
  br label %exit

tag53:                                            ; preds = %entry
  br label %exit

tag54:                                            ; preds = %entry
  br label %exit

tag55:                                            ; preds = %entry
  br label %exit

tag56:                                            ; preds = %entry
  br label %exit

tag57:                                            ; preds = %entry
  br label %exit

tag58:                                            ; preds = %entry
  br label %exit

tag59:                                            ; preds = %entry
  br label %exit

tag60:                                            ; preds = %entry
  br label %exit

tag61:                                            ; preds = %entry
  br label %exit

tag62:                                            ; preds = %entry
  br label %exit

tag63:                                            ; preds = %entry
  br label %exit

tag64:                                            ; preds = %entry
  br label %exit

tag65:                                            ; preds = %entry
  br label %exit

tag66:                                            ; preds = %entry
  br label %exit

tag67:                                            ; preds = %entry
  br label %exit

tag68:                                            ; preds = %entry
  br label %exit

tag69:                                            ; preds = %entry
  br label %exit

tag70:                                            ; preds = %entry
  br label %exit

tag71:                                            ; preds = %entry
  br label %exit

tag72:                                            ; preds = %entry
  br label %exit

tag73:                                            ; preds = %entry
  br label %exit

tag74:                                            ; preds = %entry
  br label %exit

tag75:                                            ; preds = %entry
  br label %exit

tag76:                                            ; preds = %entry
  br label %exit

tag77:                                            ; preds = %entry
  br label %exit

tag78:                                            ; preds = %entry
  br label %exit

tag79:                                            ; preds = %entry
  br label %exit

tag80:                                            ; preds = %entry
  br label %exit

tag81:                                            ; preds = %entry
  br label %exit

tag82:                                            ; preds = %entry
  br label %exit

tag83:                                            ; preds = %entry
  br label %exit

tag84:                                            ; preds = %entry
  br label %exit

tag85:                                            ; preds = %entry
  br label %exit

tag86:                                            ; preds = %entry
  br label %exit

tag87:                                            ; preds = %entry
  br label %exit

tag88:                                            ; preds = %entry
  br label %exit

tag89:                                            ; preds = %entry
  br label %exit

tag90:                                            ; preds = %entry
  br label %exit

tag91:                                            ; preds = %entry
  br label %exit

tag92:                                            ; preds = %entry
  br label %exit

tag93:                                            ; preds = %entry
  br label %exit

tag94:                                            ; preds = %entry
  br label %exit

tag95:                                            ; preds = %entry
  br label %exit

tag96:                                            ; preds = %entry
  br label %exit

tag97:                                            ; preds = %entry
  br label %exit

tag98:                                            ; preds = %entry
  br label %exit

tag99:                                            ; preds = %entry
  br label %exit

tag100:                                           ; preds = %entry
  br label %exit

tag101:                                           ; preds = %entry
  br label %exit

tag102:                                           ; preds = %entry
  br label %exit

tag103:                                           ; preds = %entry
  br label %exit

tag104:                                           ; preds = %entry
  br label %exit

tag105:                                           ; preds = %entry
  br label %exit

tag106:                                           ; preds = %entry
  br label %exit

tag107:                                           ; preds = %entry
  br label %exit

tag108:                                           ; preds = %entry
  br label %exit

tag109:                                           ; preds = %entry
  br label %exit

tag110:                                           ; preds = %entry
  br label %exit

tag111:                                           ; preds = %entry
  br label %exit

tag112:                                           ; preds = %entry
  br label %exit

tag113:                                           ; preds = %entry
  br label %exit

tag114:                                           ; preds = %entry
  br label %exit

exit:                                             ; preds = %tag114, %tag113, %tag112, %tag111, %tag110, %tag109, %tag108, %tag107, %tag106, %tag105, %tag104, %tag103, %tag102, %tag101, %tag100, %tag99, %tag98, %tag97, %tag96, %tag95, %tag94, %tag93, %tag92, %tag91, %tag90, %tag89, %tag88, %tag87, %tag86, %tag85, %tag84, %tag83, %tag82, %tag81, %tag80, %tag79, %tag78, %tag77, %tag76, %tag75, %tag74, %tag73, %tag72, %tag71, %tag70, %tag69, %tag68, %tag67, %tag66, %tag65, %tag64, %tag63, %tag62, %tag61, %tag60, %tag59, %tag58, %tag57, %tag56, %tag55, %tag54, %tag53, %tag52, %tag51, %tag50, %tag49, %tag48, %tag47, %tag46, %tag45, %tag44, %tag43, %tag42, %tag41, %tag40, %tag39, %tag38, %tag37, %tag36, %tag35, %tag34, %tag33, %tag32, %tag31, %tag30, %tag29, %tag28, %tag27, %tag26, %tag25, %tag24, %tag23, %tag22, %tag21, %tag20, %tag19, %tag18, %tag17, %tag16, %tag15, %tag14, %tag13, %tag12, %tag11, %tag10, %tag9, %tag8, %tag7, %tag6, %tag5, %tag4, %tag3, %tag2, %tag1, %tag0
  %phi = phi %blockheader [ { i64 281487861612544 }, %tag0 ], [ { i64 562958543355905 }, %tag1 ], [ { i64 844442110001154 }, %tag2 ], [ { i64 562958543355907 }, %tag3 ], [ { i64 1125904201809924 }, %tag4 ], [ { i64 562958543355909 }, %tag5 ], [ { i64 562958543355910 }, %tag6 ], [ { i64 562958543355911 }, %tag7 ], [ { i64 562958543355912 }, %tag8 ], [ { i64 562958543355913 }, %tag9 ], [ { i64 562958543355914 }, %tag10 ], [ { i64 562958543355915 }, %tag11 ], [ { i64 562958543355916 }, %tag12 ], [ { i64 281487861612557 }, %tag13 ], [ { i64 1407392063422478 }, %tag14 ], [ { i64 1125904201809935 }, %tag15 ], [ { i64 562958543355920 }, %tag16 ], [ { i64 1125904201809937 }, %tag17 ], [ { i64 1125904201809938 }, %tag18 ], [ { i64 1688862745165843 }, %tag19 ], [ { i64 1970354901745684 }, %tag20 ], [ { i64 2251808403619861 }, %tag21 ], [ { i64 2251808403619862 }, %tag22 ], [ { i64 1688862745165847 }, %tag23 ], [ { i64 1688862745165848 }, %tag24 ], [ { i64 2251808403619865 }, %tag25 ], [ { i64 1688862745165850 }, %tag26 ], [ { i64 2251808403619867 }, %tag27 ], [ { i64 2251808403619868 }, %tag28 ], [ { i64 2251808403619869 }, %tag29 ], [ { i64 1688862745165854 }, %tag30 ], [ { i64 1688862745165855 }, %tag31 ], [ { i64 1688862745165856 }, %tag32 ], [ { i64 1688862745165857 }, %tag33 ], [ { i64 281487861612578 }, %tag34 ], [ { i64 281487861612579 }, %tag35 ], [ { i64 281487861612580 }, %tag36 ], [ { i64 2251808403619877 }, %tag37 ], [ { i64 1688862745165862 }, %tag38 ], [ { i64 2251808403619879 }, %tag39 ], [ { i64 1688862745165864 }, %tag40 ], [ { i64 281487861612585 }, %tag41 ], [ { i64 1688862745165866 }, %tag42 ], [ { i64 281487861612587 }, %tag43 ], [ { i64 1688862745165868 }, %tag44 ], [ { i64 281487861612589 }, %tag45 ], [ { i64 1688862745165870 }, %tag46 ], [ { i64 281487861612591 }, %tag47 ], [ { i64 281487861612592 }, %tag48 ], [ { i64 2533283380330545 }, %tag49 ], [ { i64 2814771241943090 }, %tag50 ], [ { i64 3096246218653747 }, %tag51 ], [ { i64 2533283380330548 }, %tag52 ], [ { i64 3377716900397109 }, %tag53 ], [ { i64 2533283380330550 }, %tag54 ], [ { i64 3659191877107767 }, %tag55 ], [ { i64 844442110001208 }, %tag56 ], [ { i64 1125904201809977 }, %tag57 ], [ { i64 844442110001210 }, %tag58 ], [ { i64 281487861612603 }, %tag59 ], [ { i64 562958543355964 }, %tag60 ], [ { i64 562958543355965 }, %tag61 ], [ { i64 562958543355966 }, %tag62 ], [ { i64 562958543355967 }, %tag63 ], [ { i64 562958543355968 }, %tag64 ], [ { i64 562958543355969 }, %tag65 ], [ { i64 562958543355970 }, %tag66 ], [ { i64 562958543355971 }, %tag67 ], [ { i64 562958543355972 }, %tag68 ], [ { i64 562958543355973 }, %tag69 ], [ { i64 562958543355974 }, %tag70 ], [ { i64 562958543355975 }, %tag71 ], [ { i64 562958543355976 }, %tag72 ], [ { i64 562958543355977 }, %tag73 ], [ { i64 562958543355978 }, %tag74 ], [ { i64 562958543355979 }, %tag75 ], [ { i64 562958543355980 }, %tag76 ], [ { i64 562958543355981 }, %tag77 ], [ { i64 562958543355982 }, %tag78 ], [ { i64 562958543355983 }, %tag79 ], [ { i64 562958543355984 }, %tag80 ], [ { i64 562958543355985 }, %tag81 ], [ { i64 562958543355986 }, %tag82 ], [ { i64 562958543355987 }, %tag83 ], [ { i64 844442110001236 }, %tag84 ], [ { i64 1688862745165909 }, %tag85 ], [ { i64 3940658263883862 }, %tag86 ], [ { i64 3377716900397143 }, %tag87 ], [ { i64 281487861612632 }, %tag88 ], [ { i64 281487861612633 }, %tag89 ], [ { i64 1125904201810010 }, %tag90 ], [ { i64 562958543355995 }, %tag91 ], [ { i64 562958543355996 }, %tag92 ], [ { i64 562958543355997 }, %tag93 ], [ { i64 562958543355998 }, %tag94 ], [ { i64 562958543355999 }, %tag95 ], [ { i64 562958543356000 }, %tag96 ], [ { i64 562958543356001 }, %tag97 ], [ { i64 562958543356002 }, %tag98 ], [ { i64 2533283380330595 }, %tag99 ], [ { i64 3940658263883876 }, %tag100 ], [ { i64 562958543356005 }, %tag101 ], [ { i64 562958543356006 }, %tag102 ], [ { i64 562958543356007 }, %tag103 ], [ { i64 562958543356008 }, %tag104 ], [ { i64 4222159010398313 }, %tag105 ], [ { i64 4503616807239786 }, %tag106 ], [ { i64 562958543356011 }, %tag107 ], [ { i64 562958543356012 }, %tag108 ], [ { i64 844442110001261 }, %tag109 ], [ { i64 562958543356014 }, %tag110 ], [ { i64 562958543356015 }, %tag111 ], [ { i64 562958543356016 }, %tag112 ], [ { i64 562958543356017 }, %tag113 ], [ { i64 281487861612658 }, %tag114 ]
  ret %blockheader %phi

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

define i1 @isSymbolAFunction(i32) {
entry:
  switch i32 %0, label %stuck [
    i32 0, label %tag0
    i32 1, label %tag1
    i32 2, label %tag2
    i32 3, label %tag3
    i32 4, label %tag4
    i32 5, label %tag5
    i32 6, label %tag6
    i32 7, label %tag7
    i32 8, label %tag8
    i32 9, label %tag9
    i32 10, label %tag10
    i32 11, label %tag11
    i32 12, label %tag12
    i32 13, label %tag13
    i32 14, label %tag14
    i32 15, label %tag15
    i32 16, label %tag16
    i32 17, label %tag17
    i32 18, label %tag18
    i32 19, label %tag19
    i32 20, label %tag20
    i32 21, label %tag21
    i32 22, label %tag22
    i32 23, label %tag23
    i32 24, label %tag24
    i32 25, label %tag25
    i32 26, label %tag26
    i32 27, label %tag27
    i32 28, label %tag28
    i32 29, label %tag29
    i32 30, label %tag30
    i32 31, label %tag31
    i32 32, label %tag32
    i32 33, label %tag33
    i32 34, label %tag34
    i32 35, label %tag35
    i32 36, label %tag36
    i32 37, label %tag37
    i32 38, label %tag38
    i32 39, label %tag39
    i32 40, label %tag40
    i32 41, label %tag41
    i32 42, label %tag42
    i32 43, label %tag43
    i32 44, label %tag44
    i32 45, label %tag45
    i32 46, label %tag46
    i32 47, label %tag47
    i32 48, label %tag48
    i32 49, label %tag49
    i32 50, label %tag50
    i32 51, label %tag51
    i32 52, label %tag52
    i32 53, label %tag53
    i32 54, label %tag54
    i32 55, label %tag55
    i32 56, label %tag56
    i32 57, label %tag57
    i32 58, label %tag58
    i32 59, label %tag59
    i32 60, label %tag60
    i32 61, label %tag61
    i32 62, label %tag62
    i32 63, label %tag63
    i32 64, label %tag64
    i32 65, label %tag65
    i32 66, label %tag66
    i32 67, label %tag67
    i32 68, label %tag68
    i32 69, label %tag69
    i32 70, label %tag70
    i32 71, label %tag71
    i32 72, label %tag72
    i32 73, label %tag73
    i32 74, label %tag74
    i32 75, label %tag75
    i32 76, label %tag76
    i32 77, label %tag77
    i32 78, label %tag78
    i32 79, label %tag79
    i32 80, label %tag80
    i32 81, label %tag81
    i32 82, label %tag82
    i32 83, label %tag83
    i32 84, label %tag84
    i32 85, label %tag85
    i32 86, label %tag86
    i32 87, label %tag87
    i32 88, label %tag88
    i32 89, label %tag89
    i32 90, label %tag90
    i32 91, label %tag91
    i32 92, label %tag92
    i32 93, label %tag93
    i32 94, label %tag94
    i32 95, label %tag95
    i32 96, label %tag96
    i32 97, label %tag97
    i32 98, label %tag98
    i32 99, label %tag99
    i32 100, label %tag100
    i32 101, label %tag101
    i32 102, label %tag102
    i32 103, label %tag103
    i32 104, label %tag104
    i32 105, label %tag105
    i32 106, label %tag106
    i32 107, label %tag107
    i32 108, label %tag108
    i32 109, label %tag109
    i32 110, label %tag110
    i32 111, label %tag111
    i32 112, label %tag112
    i32 113, label %tag113
    i32 114, label %tag114
  ]

tag0:                                             ; preds = %entry
  br label %exit

tag1:                                             ; preds = %entry
  br label %exit

tag2:                                             ; preds = %entry
  br label %exit

tag3:                                             ; preds = %entry
  br label %exit

tag4:                                             ; preds = %entry
  br label %exit

tag5:                                             ; preds = %entry
  br label %exit

tag6:                                             ; preds = %entry
  br label %exit

tag7:                                             ; preds = %entry
  br label %exit

tag8:                                             ; preds = %entry
  br label %exit

tag9:                                             ; preds = %entry
  br label %exit

tag10:                                            ; preds = %entry
  br label %exit

tag11:                                            ; preds = %entry
  br label %exit

tag12:                                            ; preds = %entry
  br label %exit

tag13:                                            ; preds = %entry
  br label %exit

tag14:                                            ; preds = %entry
  br label %exit

tag15:                                            ; preds = %entry
  br label %exit

tag16:                                            ; preds = %entry
  br label %exit

tag17:                                            ; preds = %entry
  br label %exit

tag18:                                            ; preds = %entry
  br label %exit

tag19:                                            ; preds = %entry
  br label %exit

tag20:                                            ; preds = %entry
  br label %exit

tag21:                                            ; preds = %entry
  br label %exit

tag22:                                            ; preds = %entry
  br label %exit

tag23:                                            ; preds = %entry
  br label %exit

tag24:                                            ; preds = %entry
  br label %exit

tag25:                                            ; preds = %entry
  br label %exit

tag26:                                            ; preds = %entry
  br label %exit

tag27:                                            ; preds = %entry
  br label %exit

tag28:                                            ; preds = %entry
  br label %exit

tag29:                                            ; preds = %entry
  br label %exit

tag30:                                            ; preds = %entry
  br label %exit

tag31:                                            ; preds = %entry
  br label %exit

tag32:                                            ; preds = %entry
  br label %exit

tag33:                                            ; preds = %entry
  br label %exit

tag34:                                            ; preds = %entry
  br label %exit

tag35:                                            ; preds = %entry
  br label %exit

tag36:                                            ; preds = %entry
  br label %exit

tag37:                                            ; preds = %entry
  br label %exit

tag38:                                            ; preds = %entry
  br label %exit

tag39:                                            ; preds = %entry
  br label %exit

tag40:                                            ; preds = %entry
  br label %exit

tag41:                                            ; preds = %entry
  br label %exit

tag42:                                            ; preds = %entry
  br label %exit

tag43:                                            ; preds = %entry
  br label %exit

tag44:                                            ; preds = %entry
  br label %exit

tag45:                                            ; preds = %entry
  br label %exit

tag46:                                            ; preds = %entry
  br label %exit

tag47:                                            ; preds = %entry
  br label %exit

tag48:                                            ; preds = %entry
  br label %exit

tag49:                                            ; preds = %entry
  br label %exit

tag50:                                            ; preds = %entry
  br label %exit

tag51:                                            ; preds = %entry
  br label %exit

tag52:                                            ; preds = %entry
  br label %exit

tag53:                                            ; preds = %entry
  br label %exit

tag54:                                            ; preds = %entry
  br label %exit

tag55:                                            ; preds = %entry
  br label %exit

tag56:                                            ; preds = %entry
  br label %exit

tag57:                                            ; preds = %entry
  br label %exit

tag58:                                            ; preds = %entry
  br label %exit

tag59:                                            ; preds = %entry
  br label %exit

tag60:                                            ; preds = %entry
  br label %exit

tag61:                                            ; preds = %entry
  br label %exit

tag62:                                            ; preds = %entry
  br label %exit

tag63:                                            ; preds = %entry
  br label %exit

tag64:                                            ; preds = %entry
  br label %exit

tag65:                                            ; preds = %entry
  br label %exit

tag66:                                            ; preds = %entry
  br label %exit

tag67:                                            ; preds = %entry
  br label %exit

tag68:                                            ; preds = %entry
  br label %exit

tag69:                                            ; preds = %entry
  br label %exit

tag70:                                            ; preds = %entry
  br label %exit

tag71:                                            ; preds = %entry
  br label %exit

tag72:                                            ; preds = %entry
  br label %exit

tag73:                                            ; preds = %entry
  br label %exit

tag74:                                            ; preds = %entry
  br label %exit

tag75:                                            ; preds = %entry
  br label %exit

tag76:                                            ; preds = %entry
  br label %exit

tag77:                                            ; preds = %entry
  br label %exit

tag78:                                            ; preds = %entry
  br label %exit

tag79:                                            ; preds = %entry
  br label %exit

tag80:                                            ; preds = %entry
  br label %exit

tag81:                                            ; preds = %entry
  br label %exit

tag82:                                            ; preds = %entry
  br label %exit

tag83:                                            ; preds = %entry
  br label %exit

tag84:                                            ; preds = %entry
  br label %exit

tag85:                                            ; preds = %entry
  br label %exit

tag86:                                            ; preds = %entry
  br label %exit

tag87:                                            ; preds = %entry
  br label %exit

tag88:                                            ; preds = %entry
  br label %exit

tag89:                                            ; preds = %entry
  br label %exit

tag90:                                            ; preds = %entry
  br label %exit

tag91:                                            ; preds = %entry
  br label %exit

tag92:                                            ; preds = %entry
  br label %exit

tag93:                                            ; preds = %entry
  br label %exit

tag94:                                            ; preds = %entry
  br label %exit

tag95:                                            ; preds = %entry
  br label %exit

tag96:                                            ; preds = %entry
  br label %exit

tag97:                                            ; preds = %entry
  br label %exit

tag98:                                            ; preds = %entry
  br label %exit

tag99:                                            ; preds = %entry
  br label %exit

tag100:                                           ; preds = %entry
  br label %exit

tag101:                                           ; preds = %entry
  br label %exit

tag102:                                           ; preds = %entry
  br label %exit

tag103:                                           ; preds = %entry
  br label %exit

tag104:                                           ; preds = %entry
  br label %exit

tag105:                                           ; preds = %entry
  br label %exit

tag106:                                           ; preds = %entry
  br label %exit

tag107:                                           ; preds = %entry
  br label %exit

tag108:                                           ; preds = %entry
  br label %exit

tag109:                                           ; preds = %entry
  br label %exit

tag110:                                           ; preds = %entry
  br label %exit

tag111:                                           ; preds = %entry
  br label %exit

tag112:                                           ; preds = %entry
  br label %exit

tag113:                                           ; preds = %entry
  br label %exit

tag114:                                           ; preds = %entry
  br label %exit

exit:                                             ; preds = %tag114, %tag113, %tag112, %tag111, %tag110, %tag109, %tag108, %tag107, %tag106, %tag105, %tag104, %tag103, %tag102, %tag101, %tag100, %tag99, %tag98, %tag97, %tag96, %tag95, %tag94, %tag93, %tag92, %tag91, %tag90, %tag89, %tag88, %tag87, %tag86, %tag85, %tag84, %tag83, %tag82, %tag81, %tag80, %tag79, %tag78, %tag77, %tag76, %tag75, %tag74, %tag73, %tag72, %tag71, %tag70, %tag69, %tag68, %tag67, %tag66, %tag65, %tag64, %tag63, %tag62, %tag61, %tag60, %tag59, %tag58, %tag57, %tag56, %tag55, %tag54, %tag53, %tag52, %tag51, %tag50, %tag49, %tag48, %tag47, %tag46, %tag45, %tag44, %tag43, %tag42, %tag41, %tag40, %tag39, %tag38, %tag37, %tag36, %tag35, %tag34, %tag33, %tag32, %tag31, %tag30, %tag29, %tag28, %tag27, %tag26, %tag25, %tag24, %tag23, %tag22, %tag21, %tag20, %tag19, %tag18, %tag17, %tag16, %tag15, %tag14, %tag13, %tag12, %tag11, %tag10, %tag9, %tag8, %tag7, %tag6, %tag5, %tag4, %tag3, %tag2, %tag1, %tag0
  %phi = phi i1 [ false, %tag0 ], [ false, %tag1 ], [ false, %tag2 ], [ false, %tag3 ], [ false, %tag4 ], [ false, %tag5 ], [ false, %tag6 ], [ false, %tag7 ], [ false, %tag8 ], [ false, %tag9 ], [ false, %tag10 ], [ false, %tag11 ], [ false, %tag12 ], [ false, %tag13 ], [ true, %tag14 ], [ false, %tag15 ], [ false, %tag16 ], [ false, %tag17 ], [ true, %tag18 ], [ true, %tag19 ], [ true, %tag20 ], [ true, %tag21 ], [ true, %tag22 ], [ true, %tag23 ], [ true, %tag24 ], [ true, %tag25 ], [ true, %tag26 ], [ true, %tag27 ], [ true, %tag28 ], [ true, %tag29 ], [ true, %tag30 ], [ true, %tag31 ], [ true, %tag32 ], [ true, %tag33 ], [ false, %tag34 ], [ false, %tag35 ], [ false, %tag36 ], [ true, %tag37 ], [ true, %tag38 ], [ true, %tag39 ], [ true, %tag40 ], [ false, %tag41 ], [ true, %tag42 ], [ true, %tag43 ], [ true, %tag44 ], [ false, %tag45 ], [ true, %tag46 ], [ false, %tag47 ], [ false, %tag48 ], [ false, %tag49 ], [ true, %tag50 ], [ true, %tag51 ], [ true, %tag52 ], [ true, %tag53 ], [ true, %tag54 ], [ false, %tag55 ], [ true, %tag56 ], [ true, %tag57 ], [ true, %tag58 ], [ false, %tag59 ], [ true, %tag60 ], [ true, %tag61 ], [ true, %tag62 ], [ true, %tag63 ], [ true, %tag64 ], [ true, %tag65 ], [ true, %tag66 ], [ true, %tag67 ], [ true, %tag68 ], [ true, %tag69 ], [ true, %tag70 ], [ true, %tag71 ], [ true, %tag72 ], [ true, %tag73 ], [ true, %tag74 ], [ true, %tag75 ], [ true, %tag76 ], [ true, %tag77 ], [ true, %tag78 ], [ true, %tag79 ], [ true, %tag80 ], [ true, %tag81 ], [ true, %tag82 ], [ true, %tag83 ], [ true, %tag84 ], [ true, %tag85 ], [ true, %tag86 ], [ true, %tag87 ], [ false, %tag88 ], [ true, %tag89 ], [ false, %tag90 ], [ false, %tag91 ], [ false, %tag92 ], [ false, %tag93 ], [ false, %tag94 ], [ false, %tag95 ], [ false, %tag96 ], [ false, %tag97 ], [ false, %tag98 ], [ false, %tag99 ], [ false, %tag100 ], [ false, %tag101 ], [ false, %tag102 ], [ false, %tag103 ], [ false, %tag104 ], [ false, %tag105 ], [ false, %tag106 ], [ false, %tag107 ], [ false, %tag108 ], [ false, %tag109 ], [ false, %tag110 ], [ false, %tag111 ], [ false, %tag112 ], [ false, %tag113 ], [ false, %tag114 ]
  ret i1 %phi

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

define void @storeSymbolChildren(%block*, [0 x i8*]*) {
entry:
  %2 = getelementptr inbounds %block, %block* %0, i64 0, i32 0, i32 0
  %3 = load i64, i64* %2
  %4 = trunc i64 %3 to i32
  switch i32 %4, label %stuck [
    i32 0, label %tag0
    i32 1, label %tag1
    i32 2, label %tag2
    i32 3, label %tag3
    i32 5, label %tag5
    i32 6, label %tag6
    i32 7, label %tag7
    i32 8, label %tag8
    i32 9, label %tag9
    i32 10, label %tag10
    i32 11, label %tag11
    i32 12, label %tag12
    i32 13, label %tag13
    i32 14, label %tag14
    i32 16, label %tag16
    i32 19, label %tag19
    i32 20, label %tag20
    i32 21, label %tag21
    i32 22, label %tag22
    i32 23, label %tag23
    i32 24, label %tag24
    i32 25, label %tag25
    i32 26, label %tag26
    i32 27, label %tag27
    i32 28, label %tag28
    i32 29, label %tag29
    i32 30, label %tag30
    i32 31, label %tag31
    i32 32, label %tag32
    i32 33, label %tag33
    i32 34, label %tag34
    i32 35, label %tag35
    i32 36, label %tag36
    i32 37, label %tag37
    i32 38, label %tag38
    i32 39, label %tag39
    i32 40, label %tag40
    i32 41, label %tag41
    i32 42, label %tag42
    i32 43, label %tag43
    i32 44, label %tag44
    i32 45, label %tag45
    i32 46, label %tag46
    i32 47, label %tag47
    i32 48, label %tag48
    i32 49, label %tag49
    i32 50, label %tag50
    i32 51, label %tag51
    i32 52, label %tag52
    i32 53, label %tag53
    i32 54, label %tag54
    i32 55, label %tag55
    i32 56, label %tag56
    i32 58, label %tag58
    i32 59, label %tag59
    i32 60, label %tag60
    i32 61, label %tag61
    i32 62, label %tag62
    i32 63, label %tag63
    i32 64, label %tag64
    i32 65, label %tag65
    i32 66, label %tag66
    i32 67, label %tag67
    i32 68, label %tag68
    i32 69, label %tag69
    i32 70, label %tag70
    i32 71, label %tag71
    i32 72, label %tag72
    i32 73, label %tag73
    i32 74, label %tag74
    i32 75, label %tag75
    i32 76, label %tag76
    i32 77, label %tag77
    i32 78, label %tag78
    i32 79, label %tag79
    i32 80, label %tag80
    i32 81, label %tag81
    i32 82, label %tag82
    i32 83, label %tag83
    i32 84, label %tag84
    i32 85, label %tag85
    i32 86, label %tag86
    i32 87, label %tag87
    i32 88, label %tag88
    i32 89, label %tag89
    i32 91, label %tag91
    i32 92, label %tag92
    i32 93, label %tag93
    i32 94, label %tag94
    i32 95, label %tag95
    i32 96, label %tag96
    i32 97, label %tag97
    i32 98, label %tag98
    i32 99, label %tag99
    i32 100, label %tag100
    i32 101, label %tag101
    i32 102, label %tag102
    i32 103, label %tag103
    i32 104, label %tag104
    i32 105, label %tag105
    i32 106, label %tag106
    i32 107, label %tag107
    i32 108, label %tag108
    i32 109, label %tag109
    i32 110, label %tag110
    i32 111, label %tag111
    i32 112, label %tag112
    i32 113, label %tag113
    i32 114, label %tag114
  ]

tag0:                                             ; preds = %entry
  %5 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %6 = load i8*, i8** %5
  %7 = bitcast i8* %6 to %block*
  %8 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %9 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %8, i64 0, i32 2
  store %block* %7, %block** %9
  %10 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %11 = load i8*, i8** %10
  %12 = bitcast i8* %11 to %block*
  %13 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %14 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %13, i64 0, i32 3
  store %block* %12, %block** %14
  ret void

tag1:                                             ; preds = %entry
  %15 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %16 = load i8*, i8** %15
  %17 = bitcast i8* %16 to %block*
  %18 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %19 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %18, i64 0, i32 2
  store %block* %17, %block** %19
  ret void

tag2:                                             ; preds = %entry
  %20 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %21 = load i8*, i8** %20
  %22 = bitcast i8* %21 to %map*
  %23 = load %map, %map* %22
  tail call void @free(i8* %21)
  %24 = bitcast %block* %0 to { %blockheader, [0 x i64], %map }*
  %25 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %24, i64 0, i32 2
  store %map %23, %map* %25
  ret void

tag3:                                             ; preds = %entry
  %26 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %27 = load i8*, i8** %26
  %28 = bitcast i8* %27 to %block*
  %29 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %30 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %29, i64 0, i32 2
  store %block* %28, %block** %30
  ret void

tag5:                                             ; preds = %entry
  %31 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %32 = load i8*, i8** %31
  %33 = bitcast i8* %32 to %block*
  %34 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %35 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %34, i64 0, i32 2
  store %block* %33, %block** %35
  ret void

tag6:                                             ; preds = %entry
  %36 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %37 = load i8*, i8** %36
  %38 = bitcast i8* %37 to %block*
  %39 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %40 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %39, i64 0, i32 2
  store %block* %38, %block** %40
  ret void

tag7:                                             ; preds = %entry
  %41 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %42 = load i8*, i8** %41
  %43 = bitcast i8* %42 to %block*
  %44 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %45 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %44, i64 0, i32 2
  store %block* %43, %block** %45
  ret void

tag8:                                             ; preds = %entry
  %46 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %47 = load i8*, i8** %46
  %48 = bitcast i8* %47 to %block*
  %49 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %50 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %49, i64 0, i32 2
  store %block* %48, %block** %50
  ret void

tag9:                                             ; preds = %entry
  %51 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %52 = load i8*, i8** %51
  %53 = bitcast i8* %52 to %block*
  %54 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %55 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %54, i64 0, i32 2
  store %block* %53, %block** %55
  ret void

tag10:                                            ; preds = %entry
  %56 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %57 = load i8*, i8** %56
  %58 = bitcast i8* %57 to %block*
  %59 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %60 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %59, i64 0, i32 2
  store %block* %58, %block** %60
  ret void

tag11:                                            ; preds = %entry
  %61 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %62 = load i8*, i8** %61
  %63 = bitcast i8* %62 to %block*
  %64 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %65 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %64, i64 0, i32 2
  store %block* %63, %block** %65
  ret void

tag12:                                            ; preds = %entry
  %66 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %67 = load i8*, i8** %66
  %68 = bitcast i8* %67 to %block*
  %69 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %70 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %69, i64 0, i32 2
  store %block* %68, %block** %70
  ret void

tag13:                                            ; preds = %entry
  %71 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %72 = load i8*, i8** %71
  %73 = bitcast i8* %72 to %block*
  %74 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %75 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %74, i64 0, i32 2
  store %block* %73, %block** %75
  %76 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %77 = load i8*, i8** %76
  %78 = bitcast i8* %77 to %block*
  %79 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %80 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %79, i64 0, i32 3
  store %block* %78, %block** %80
  ret void

tag14:                                            ; preds = %entry
  %81 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %82 = load i8*, i8** %81
  %83 = bitcast i8* %82 to i1*
  %84 = load i1, i1* %83
  tail call void @free(i8* %82)
  %85 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, %block*, %block* }*
  %86 = getelementptr inbounds { %blockheader, [0 x i64], i1, %block*, %block* }, { %blockheader, [0 x i64], i1, %block*, %block* }* %85, i64 0, i32 2
  store i1 %84, i1* %86
  %87 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %88 = load i8*, i8** %87
  %89 = bitcast i8* %88 to %block*
  %90 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, %block*, %block* }*
  %91 = getelementptr inbounds { %blockheader, [0 x i64], i1, %block*, %block* }, { %blockheader, [0 x i64], i1, %block*, %block* }* %90, i64 0, i32 3
  store %block* %89, %block** %91
  %92 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %93 = load i8*, i8** %92
  %94 = bitcast i8* %93 to %block*
  %95 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, %block*, %block* }*
  %96 = getelementptr inbounds { %blockheader, [0 x i64], i1, %block*, %block* }, { %blockheader, [0 x i64], i1, %block*, %block* }* %95, i64 0, i32 4
  store %block* %94, %block** %96
  ret void

tag16:                                            ; preds = %entry
  %97 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %98 = load i8*, i8** %97
  %99 = bitcast i8* %98 to %block*
  %100 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %101 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %100, i64 0, i32 2
  store %block* %99, %block** %101
  ret void

tag19:                                            ; preds = %entry
  %102 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %103 = load i8*, i8** %102
  %104 = bitcast i8* %103 to %mpz*
  %105 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %106 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %105, i64 0, i32 2
  store %mpz* %104, %mpz** %106
  %107 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %108 = load i8*, i8** %107
  %109 = bitcast i8* %108 to %mpz*
  %110 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %111 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %110, i64 0, i32 3
  store %mpz* %109, %mpz** %111
  ret void

tag20:                                            ; preds = %entry
  %112 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %113 = load i8*, i8** %112
  %114 = bitcast i8* %113 to %map*
  %115 = load %map, %map* %114
  tail call void @free(i8* %113)
  %116 = bitcast %block* %0 to { %blockheader, [0 x i64], %map, %map }*
  %117 = getelementptr inbounds { %blockheader, [0 x i64], %map, %map }, { %blockheader, [0 x i64], %map, %map }* %116, i64 0, i32 2
  store %map %115, %map* %117
  %118 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %119 = load i8*, i8** %118
  %120 = bitcast i8* %119 to %map*
  %121 = load %map, %map* %120
  tail call void @free(i8* %119)
  %122 = bitcast %block* %0 to { %blockheader, [0 x i64], %map, %map }*
  %123 = getelementptr inbounds { %blockheader, [0 x i64], %map, %map }, { %blockheader, [0 x i64], %map, %map }* %122, i64 0, i32 3
  store %map %121, %map* %123
  ret void

tag21:                                            ; preds = %entry
  %124 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %125 = load i8*, i8** %124
  %126 = bitcast i8* %125 to i1*
  %127 = load i1, i1* %126
  tail call void @free(i8* %125)
  %128 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %129 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %128, i64 0, i32 2
  store i1 %127, i1* %129
  %130 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %131 = load i8*, i8** %130
  %132 = bitcast i8* %131 to i1*
  %133 = load i1, i1* %132
  tail call void @free(i8* %131)
  %134 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %135 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %134, i64 0, i32 3
  store i1 %133, i1* %135
  ret void

tag22:                                            ; preds = %entry
  %136 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %137 = load i8*, i8** %136
  %138 = bitcast i8* %137 to i1*
  %139 = load i1, i1* %138
  tail call void @free(i8* %137)
  %140 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %141 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %140, i64 0, i32 2
  store i1 %139, i1* %141
  %142 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %143 = load i8*, i8** %142
  %144 = bitcast i8* %143 to i1*
  %145 = load i1, i1* %144
  tail call void @free(i8* %143)
  %146 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %147 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %146, i64 0, i32 3
  store i1 %145, i1* %147
  ret void

tag23:                                            ; preds = %entry
  %148 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %149 = load i8*, i8** %148
  %150 = bitcast i8* %149 to %mpz*
  %151 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %152 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %151, i64 0, i32 2
  store %mpz* %150, %mpz** %152
  %153 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %154 = load i8*, i8** %153
  %155 = bitcast i8* %154 to %mpz*
  %156 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %157 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %156, i64 0, i32 3
  store %mpz* %155, %mpz** %157
  ret void

tag24:                                            ; preds = %entry
  %158 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %159 = load i8*, i8** %158
  %160 = bitcast i8* %159 to %mpz*
  %161 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %162 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %161, i64 0, i32 2
  store %mpz* %160, %mpz** %162
  %163 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %164 = load i8*, i8** %163
  %165 = bitcast i8* %164 to %mpz*
  %166 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %167 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %166, i64 0, i32 3
  store %mpz* %165, %mpz** %167
  ret void

tag25:                                            ; preds = %entry
  %168 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %169 = load i8*, i8** %168
  %170 = bitcast i8* %169 to i1*
  %171 = load i1, i1* %170
  tail call void @free(i8* %169)
  %172 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %173 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %172, i64 0, i32 2
  store i1 %171, i1* %173
  %174 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %175 = load i8*, i8** %174
  %176 = bitcast i8* %175 to i1*
  %177 = load i1, i1* %176
  tail call void @free(i8* %175)
  %178 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %179 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %178, i64 0, i32 3
  store i1 %177, i1* %179
  ret void

tag26:                                            ; preds = %entry
  %180 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %181 = load i8*, i8** %180
  %182 = bitcast i8* %181 to %mpz*
  %183 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %184 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %183, i64 0, i32 2
  store %mpz* %182, %mpz** %184
  %185 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %186 = load i8*, i8** %185
  %187 = bitcast i8* %186 to %mpz*
  %188 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %189 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %188, i64 0, i32 3
  store %mpz* %187, %mpz** %189
  ret void

tag27:                                            ; preds = %entry
  %190 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %191 = load i8*, i8** %190
  %192 = bitcast i8* %191 to i1*
  %193 = load i1, i1* %192
  tail call void @free(i8* %191)
  %194 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %195 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %194, i64 0, i32 2
  store i1 %193, i1* %195
  %196 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %197 = load i8*, i8** %196
  %198 = bitcast i8* %197 to i1*
  %199 = load i1, i1* %198
  tail call void @free(i8* %197)
  %200 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %201 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %200, i64 0, i32 3
  store i1 %199, i1* %201
  ret void

tag28:                                            ; preds = %entry
  %202 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %203 = load i8*, i8** %202
  %204 = bitcast i8* %203 to i1*
  %205 = load i1, i1* %204
  tail call void @free(i8* %203)
  %206 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %207 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %206, i64 0, i32 2
  store i1 %205, i1* %207
  %208 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %209 = load i8*, i8** %208
  %210 = bitcast i8* %209 to i1*
  %211 = load i1, i1* %210
  tail call void @free(i8* %209)
  %212 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %213 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %212, i64 0, i32 3
  store i1 %211, i1* %213
  ret void

tag29:                                            ; preds = %entry
  %214 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %215 = load i8*, i8** %214
  %216 = bitcast i8* %215 to i1*
  %217 = load i1, i1* %216
  tail call void @free(i8* %215)
  %218 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %219 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %218, i64 0, i32 2
  store i1 %217, i1* %219
  %220 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %221 = load i8*, i8** %220
  %222 = bitcast i8* %221 to i1*
  %223 = load i1, i1* %222
  tail call void @free(i8* %221)
  %224 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %225 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %224, i64 0, i32 3
  store i1 %223, i1* %225
  ret void

tag30:                                            ; preds = %entry
  %226 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %227 = load i8*, i8** %226
  %228 = bitcast i8* %227 to %mpz*
  %229 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %230 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %229, i64 0, i32 2
  store %mpz* %228, %mpz** %230
  %231 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %232 = load i8*, i8** %231
  %233 = bitcast i8* %232 to %mpz*
  %234 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %235 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %234, i64 0, i32 3
  store %mpz* %233, %mpz** %235
  ret void

tag31:                                            ; preds = %entry
  %236 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %237 = load i8*, i8** %236
  %238 = bitcast i8* %237 to %mpz*
  %239 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %240 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %239, i64 0, i32 2
  store %mpz* %238, %mpz** %240
  %241 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %242 = load i8*, i8** %241
  %243 = bitcast i8* %242 to %mpz*
  %244 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %245 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %244, i64 0, i32 3
  store %mpz* %243, %mpz** %245
  ret void

tag32:                                            ; preds = %entry
  %246 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %247 = load i8*, i8** %246
  %248 = bitcast i8* %247 to %mpz*
  %249 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %250 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %249, i64 0, i32 2
  store %mpz* %248, %mpz** %250
  %251 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %252 = load i8*, i8** %251
  %253 = bitcast i8* %252 to %mpz*
  %254 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %255 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %254, i64 0, i32 3
  store %mpz* %253, %mpz** %255
  ret void

tag33:                                            ; preds = %entry
  %256 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %257 = load i8*, i8** %256
  %258 = bitcast i8* %257 to %mpz*
  %259 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %260 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %259, i64 0, i32 2
  store %mpz* %258, %mpz** %260
  %261 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %262 = load i8*, i8** %261
  %263 = bitcast i8* %262 to %mpz*
  %264 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %265 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %264, i64 0, i32 3
  store %mpz* %263, %mpz** %265
  ret void

tag34:                                            ; preds = %entry
  %266 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %267 = load i8*, i8** %266
  %268 = bitcast i8* %267 to %block*
  %269 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %270 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %269, i64 0, i32 2
  store %block* %268, %block** %270
  %271 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %272 = load i8*, i8** %271
  %273 = bitcast i8* %272 to %block*
  %274 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %275 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %274, i64 0, i32 3
  store %block* %273, %block** %275
  ret void

tag35:                                            ; preds = %entry
  %276 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %277 = load i8*, i8** %276
  %278 = bitcast i8* %277 to %block*
  %279 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %280 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %279, i64 0, i32 2
  store %block* %278, %block** %280
  %281 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %282 = load i8*, i8** %281
  %283 = bitcast i8* %282 to %block*
  %284 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %285 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %284, i64 0, i32 3
  store %block* %283, %block** %285
  ret void

tag36:                                            ; preds = %entry
  %286 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %287 = load i8*, i8** %286
  %288 = bitcast i8* %287 to %block*
  %289 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %290 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %289, i64 0, i32 2
  store %block* %288, %block** %290
  %291 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %292 = load i8*, i8** %291
  %293 = bitcast i8* %292 to %block*
  %294 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %295 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %294, i64 0, i32 3
  store %block* %293, %block** %295
  ret void

tag37:                                            ; preds = %entry
  %296 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %297 = load i8*, i8** %296
  %298 = bitcast i8* %297 to i1*
  %299 = load i1, i1* %298
  tail call void @free(i8* %297)
  %300 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %301 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %300, i64 0, i32 2
  store i1 %299, i1* %301
  %302 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %303 = load i8*, i8** %302
  %304 = bitcast i8* %303 to i1*
  %305 = load i1, i1* %304
  tail call void @free(i8* %303)
  %306 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %307 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %306, i64 0, i32 3
  store i1 %305, i1* %307
  ret void

tag38:                                            ; preds = %entry
  %308 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %309 = load i8*, i8** %308
  %310 = bitcast i8* %309 to %mpz*
  %311 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %312 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %311, i64 0, i32 2
  store %mpz* %310, %mpz** %312
  %313 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %314 = load i8*, i8** %313
  %315 = bitcast i8* %314 to %mpz*
  %316 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %317 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %316, i64 0, i32 3
  store %mpz* %315, %mpz** %317
  ret void

tag39:                                            ; preds = %entry
  %318 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %319 = load i8*, i8** %318
  %320 = bitcast i8* %319 to i1*
  %321 = load i1, i1* %320
  tail call void @free(i8* %319)
  %322 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %323 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %322, i64 0, i32 2
  store i1 %321, i1* %323
  %324 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %325 = load i8*, i8** %324
  %326 = bitcast i8* %325 to i1*
  %327 = load i1, i1* %326
  tail call void @free(i8* %325)
  %328 = bitcast %block* %0 to { %blockheader, [0 x i64], i1, i1 }*
  %329 = getelementptr inbounds { %blockheader, [0 x i64], i1, i1 }, { %blockheader, [0 x i64], i1, i1 }* %328, i64 0, i32 3
  store i1 %327, i1* %329
  ret void

tag40:                                            ; preds = %entry
  %330 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %331 = load i8*, i8** %330
  %332 = bitcast i8* %331 to %mpz*
  %333 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %334 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %333, i64 0, i32 2
  store %mpz* %332, %mpz** %334
  %335 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %336 = load i8*, i8** %335
  %337 = bitcast i8* %336 to %mpz*
  %338 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %339 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %338, i64 0, i32 3
  store %mpz* %337, %mpz** %339
  ret void

tag41:                                            ; preds = %entry
  %340 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %341 = load i8*, i8** %340
  %342 = bitcast i8* %341 to %block*
  %343 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %344 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %343, i64 0, i32 2
  store %block* %342, %block** %344
  %345 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %346 = load i8*, i8** %345
  %347 = bitcast i8* %346 to %block*
  %348 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %349 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %348, i64 0, i32 3
  store %block* %347, %block** %349
  ret void

tag42:                                            ; preds = %entry
  %350 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %351 = load i8*, i8** %350
  %352 = bitcast i8* %351 to %mpz*
  %353 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %354 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %353, i64 0, i32 2
  store %mpz* %352, %mpz** %354
  %355 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %356 = load i8*, i8** %355
  %357 = bitcast i8* %356 to %mpz*
  %358 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %359 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %358, i64 0, i32 3
  store %mpz* %357, %mpz** %359
  ret void

tag43:                                            ; preds = %entry
  %360 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %361 = load i8*, i8** %360
  %362 = bitcast i8* %361 to %block*
  %363 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %364 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %363, i64 0, i32 2
  store %block* %362, %block** %364
  %365 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %366 = load i8*, i8** %365
  %367 = bitcast i8* %366 to %block*
  %368 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %369 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %368, i64 0, i32 3
  store %block* %367, %block** %369
  ret void

tag44:                                            ; preds = %entry
  %370 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %371 = load i8*, i8** %370
  %372 = bitcast i8* %371 to %mpz*
  %373 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %374 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %373, i64 0, i32 2
  store %mpz* %372, %mpz** %374
  %375 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %376 = load i8*, i8** %375
  %377 = bitcast i8* %376 to %mpz*
  %378 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %379 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %378, i64 0, i32 3
  store %mpz* %377, %mpz** %379
  ret void

tag45:                                            ; preds = %entry
  %380 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %381 = load i8*, i8** %380
  %382 = bitcast i8* %381 to %block*
  %383 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %384 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %383, i64 0, i32 2
  store %block* %382, %block** %384
  %385 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %386 = load i8*, i8** %385
  %387 = bitcast i8* %386 to %block*
  %388 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %389 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %388, i64 0, i32 3
  store %block* %387, %block** %389
  ret void

tag46:                                            ; preds = %entry
  %390 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %391 = load i8*, i8** %390
  %392 = bitcast i8* %391 to %mpz*
  %393 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %394 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %393, i64 0, i32 2
  store %mpz* %392, %mpz** %394
  %395 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %396 = load i8*, i8** %395
  %397 = bitcast i8* %396 to %mpz*
  %398 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %399 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %398, i64 0, i32 3
  store %mpz* %397, %mpz** %399
  ret void

tag47:                                            ; preds = %entry
  %400 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %401 = load i8*, i8** %400
  %402 = bitcast i8* %401 to %block*
  %403 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %404 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %403, i64 0, i32 2
  store %block* %402, %block** %404
  %405 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %406 = load i8*, i8** %405
  %407 = bitcast i8* %406 to %block*
  %408 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %409 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %408, i64 0, i32 3
  store %block* %407, %block** %409
  ret void

tag48:                                            ; preds = %entry
  %410 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %411 = load i8*, i8** %410
  %412 = bitcast i8* %411 to %block*
  %413 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %414 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %413, i64 0, i32 2
  store %block* %412, %block** %414
  %415 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %416 = load i8*, i8** %415
  %417 = bitcast i8* %416 to %block*
  %418 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %419 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %418, i64 0, i32 3
  store %block* %417, %block** %419
  ret void

tag49:                                            ; preds = %entry
  %420 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %421 = load i8*, i8** %420
  %422 = bitcast i8* %421 to %mpz*
  %423 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz* }*
  %424 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %423, i64 0, i32 2
  store %mpz* %422, %mpz** %424
  ret void

tag50:                                            ; preds = %entry
  %425 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %426 = load i8*, i8** %425
  %427 = bitcast i8* %426 to %map*
  %428 = load %map, %map* %427
  tail call void @free(i8* %426)
  %429 = bitcast %block* %0 to { %blockheader, [0 x i64], %map, %block* }*
  %430 = getelementptr inbounds { %blockheader, [0 x i64], %map, %block* }, { %blockheader, [0 x i64], %map, %block* }* %429, i64 0, i32 2
  store %map %428, %map* %430
  %431 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %432 = load i8*, i8** %431
  %433 = bitcast i8* %432 to %block*
  %434 = bitcast %block* %0 to { %blockheader, [0 x i64], %map, %block* }*
  %435 = getelementptr inbounds { %blockheader, [0 x i64], %map, %block* }, { %blockheader, [0 x i64], %map, %block* }* %434, i64 0, i32 3
  store %block* %433, %block** %435
  ret void

tag51:                                            ; preds = %entry
  %436 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %437 = load i8*, i8** %436
  %438 = bitcast i8* %437 to %block*
  %439 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %set }*
  %440 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %set }, { %blockheader, [0 x i64], %block*, %set }* %439, i64 0, i32 2
  store %block* %438, %block** %440
  %441 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %442 = load i8*, i8** %441
  %443 = bitcast i8* %442 to %set*
  %444 = load %set, %set* %443
  tail call void @free(i8* %442)
  %445 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %set }*
  %446 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %set }, { %blockheader, [0 x i64], %block*, %set }* %445, i64 0, i32 3
  store %set %444, %set* %446
  ret void

tag52:                                            ; preds = %entry
  %447 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %448 = load i8*, i8** %447
  %449 = bitcast i8* %448 to %mpz*
  %450 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz* }*
  %451 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %450, i64 0, i32 2
  store %mpz* %449, %mpz** %451
  ret void

tag53:                                            ; preds = %entry
  %452 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %453 = load i8*, i8** %452
  %454 = bitcast i8* %453 to %mpz*
  %455 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %456 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %455, i64 0, i32 2
  store %mpz* %454, %mpz** %456
  %457 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %458 = load i8*, i8** %457
  %459 = bitcast i8* %458 to %mpz*
  %460 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %461 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %460, i64 0, i32 3
  store %mpz* %459, %mpz** %461
  %462 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %463 = load i8*, i8** %462
  %464 = bitcast i8* %463 to %mpz*
  %465 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %466 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %465, i64 0, i32 4
  store %mpz* %464, %mpz** %466
  ret void

tag54:                                            ; preds = %entry
  %467 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %468 = load i8*, i8** %467
  %469 = bitcast i8* %468 to %mpz*
  %470 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz* }*
  %471 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %470, i64 0, i32 2
  store %mpz* %469, %mpz** %471
  ret void

tag55:                                            ; preds = %entry
  %472 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %473 = load i8*, i8** %472
  %474 = bitcast i8* %473 to %block*
  %475 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %476 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %475, i64 0, i32 2
  store %block* %474, %block** %476
  %477 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %478 = load i8*, i8** %477
  %479 = bitcast i8* %478 to %block*
  %480 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %481 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %480, i64 0, i32 3
  store %block* %479, %block** %481
  %482 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %483 = load i8*, i8** %482
  %484 = bitcast i8* %483 to %block*
  %485 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block*, %block* }*
  %486 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block*, %block* }, { %blockheader, [0 x i64], %block*, %block*, %block* }* %485, i64 0, i32 4
  store %block* %484, %block** %486
  ret void

tag56:                                            ; preds = %entry
  %487 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %488 = load i8*, i8** %487
  %489 = bitcast i8* %488 to %map*
  %490 = load %map, %map* %489
  tail call void @free(i8* %488)
  %491 = bitcast %block* %0 to { %blockheader, [0 x i64], %map }*
  %492 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %491, i64 0, i32 2
  store %map %490, %map* %492
  ret void

tag58:                                            ; preds = %entry
  %493 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %494 = load i8*, i8** %493
  %495 = bitcast i8* %494 to %map*
  %496 = load %map, %map* %495
  tail call void @free(i8* %494)
  %497 = bitcast %block* %0 to { %blockheader, [0 x i64], %map }*
  %498 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %497, i64 0, i32 2
  store %map %496, %map* %498
  ret void

tag59:                                            ; preds = %entry
  %499 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %500 = load i8*, i8** %499
  %501 = bitcast i8* %500 to %block*
  %502 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %503 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %502, i64 0, i32 2
  store %block* %501, %block** %503
  %504 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %505 = load i8*, i8** %504
  %506 = bitcast i8* %505 to %block*
  %507 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %508 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %507, i64 0, i32 3
  store %block* %506, %block** %508
  ret void

tag60:                                            ; preds = %entry
  %509 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %510 = load i8*, i8** %509
  %511 = bitcast i8* %510 to %block*
  %512 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %513 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %512, i64 0, i32 2
  store %block* %511, %block** %513
  ret void

tag61:                                            ; preds = %entry
  %514 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %515 = load i8*, i8** %514
  %516 = bitcast i8* %515 to %block*
  %517 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %518 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %517, i64 0, i32 2
  store %block* %516, %block** %518
  ret void

tag62:                                            ; preds = %entry
  %519 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %520 = load i8*, i8** %519
  %521 = bitcast i8* %520 to %block*
  %522 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %523 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %522, i64 0, i32 2
  store %block* %521, %block** %523
  ret void

tag63:                                            ; preds = %entry
  %524 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %525 = load i8*, i8** %524
  %526 = bitcast i8* %525 to %block*
  %527 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %528 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %527, i64 0, i32 2
  store %block* %526, %block** %528
  ret void

tag64:                                            ; preds = %entry
  %529 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %530 = load i8*, i8** %529
  %531 = bitcast i8* %530 to %block*
  %532 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %533 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %532, i64 0, i32 2
  store %block* %531, %block** %533
  ret void

tag65:                                            ; preds = %entry
  %534 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %535 = load i8*, i8** %534
  %536 = bitcast i8* %535 to %block*
  %537 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %538 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %537, i64 0, i32 2
  store %block* %536, %block** %538
  ret void

tag66:                                            ; preds = %entry
  %539 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %540 = load i8*, i8** %539
  %541 = bitcast i8* %540 to %block*
  %542 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %543 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %542, i64 0, i32 2
  store %block* %541, %block** %543
  ret void

tag67:                                            ; preds = %entry
  %544 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %545 = load i8*, i8** %544
  %546 = bitcast i8* %545 to %block*
  %547 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %548 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %547, i64 0, i32 2
  store %block* %546, %block** %548
  ret void

tag68:                                            ; preds = %entry
  %549 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %550 = load i8*, i8** %549
  %551 = bitcast i8* %550 to %block*
  %552 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %553 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %552, i64 0, i32 2
  store %block* %551, %block** %553
  ret void

tag69:                                            ; preds = %entry
  %554 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %555 = load i8*, i8** %554
  %556 = bitcast i8* %555 to %block*
  %557 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %558 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %557, i64 0, i32 2
  store %block* %556, %block** %558
  ret void

tag70:                                            ; preds = %entry
  %559 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %560 = load i8*, i8** %559
  %561 = bitcast i8* %560 to %block*
  %562 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %563 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %562, i64 0, i32 2
  store %block* %561, %block** %563
  ret void

tag71:                                            ; preds = %entry
  %564 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %565 = load i8*, i8** %564
  %566 = bitcast i8* %565 to %block*
  %567 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %568 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %567, i64 0, i32 2
  store %block* %566, %block** %568
  ret void

tag72:                                            ; preds = %entry
  %569 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %570 = load i8*, i8** %569
  %571 = bitcast i8* %570 to %block*
  %572 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %573 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %572, i64 0, i32 2
  store %block* %571, %block** %573
  ret void

tag73:                                            ; preds = %entry
  %574 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %575 = load i8*, i8** %574
  %576 = bitcast i8* %575 to %block*
  %577 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %578 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %577, i64 0, i32 2
  store %block* %576, %block** %578
  ret void

tag74:                                            ; preds = %entry
  %579 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %580 = load i8*, i8** %579
  %581 = bitcast i8* %580 to %block*
  %582 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %583 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %582, i64 0, i32 2
  store %block* %581, %block** %583
  ret void

tag75:                                            ; preds = %entry
  %584 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %585 = load i8*, i8** %584
  %586 = bitcast i8* %585 to %block*
  %587 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %588 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %587, i64 0, i32 2
  store %block* %586, %block** %588
  ret void

tag76:                                            ; preds = %entry
  %589 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %590 = load i8*, i8** %589
  %591 = bitcast i8* %590 to %block*
  %592 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %593 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %592, i64 0, i32 2
  store %block* %591, %block** %593
  ret void

tag77:                                            ; preds = %entry
  %594 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %595 = load i8*, i8** %594
  %596 = bitcast i8* %595 to %block*
  %597 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %598 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %597, i64 0, i32 2
  store %block* %596, %block** %598
  ret void

tag78:                                            ; preds = %entry
  %599 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %600 = load i8*, i8** %599
  %601 = bitcast i8* %600 to %block*
  %602 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %603 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %602, i64 0, i32 2
  store %block* %601, %block** %603
  ret void

tag79:                                            ; preds = %entry
  %604 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %605 = load i8*, i8** %604
  %606 = bitcast i8* %605 to %block*
  %607 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %608 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %607, i64 0, i32 2
  store %block* %606, %block** %608
  ret void

tag80:                                            ; preds = %entry
  %609 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %610 = load i8*, i8** %609
  %611 = bitcast i8* %610 to %block*
  %612 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %613 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %612, i64 0, i32 2
  store %block* %611, %block** %613
  ret void

tag81:                                            ; preds = %entry
  %614 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %615 = load i8*, i8** %614
  %616 = bitcast i8* %615 to %block*
  %617 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %618 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %617, i64 0, i32 2
  store %block* %616, %block** %618
  ret void

tag82:                                            ; preds = %entry
  %619 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %620 = load i8*, i8** %619
  %621 = bitcast i8* %620 to %block*
  %622 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %623 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %622, i64 0, i32 2
  store %block* %621, %block** %623
  ret void

tag83:                                            ; preds = %entry
  %624 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %625 = load i8*, i8** %624
  %626 = bitcast i8* %625 to %block*
  %627 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %628 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %627, i64 0, i32 2
  store %block* %626, %block** %628
  ret void

tag84:                                            ; preds = %entry
  %629 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %630 = load i8*, i8** %629
  %631 = bitcast i8* %630 to %map*
  %632 = load %map, %map* %631
  tail call void @free(i8* %630)
  %633 = bitcast %block* %0 to { %blockheader, [0 x i64], %map }*
  %634 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %633, i64 0, i32 2
  store %map %632, %map* %634
  ret void

tag85:                                            ; preds = %entry
  %635 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %636 = load i8*, i8** %635
  %637 = bitcast i8* %636 to %mpz*
  %638 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %639 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %638, i64 0, i32 2
  store %mpz* %637, %mpz** %639
  %640 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %641 = load i8*, i8** %640
  %642 = bitcast i8* %641 to %mpz*
  %643 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz* }*
  %644 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz* }* %643, i64 0, i32 3
  store %mpz* %642, %mpz** %644
  ret void

tag86:                                            ; preds = %entry
  %645 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %646 = load i8*, i8** %645
  %647 = bitcast i8* %646 to i1*
  %648 = load i1, i1* %647
  tail call void @free(i8* %646)
  %649 = bitcast %block* %0 to { %blockheader, [0 x i64], i1 }*
  %650 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %649, i64 0, i32 2
  store i1 %648, i1* %650
  ret void

tag87:                                            ; preds = %entry
  %651 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %652 = load i8*, i8** %651
  %653 = bitcast i8* %652 to %mpz*
  %654 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %655 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %654, i64 0, i32 2
  store %mpz* %653, %mpz** %655
  %656 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %657 = load i8*, i8** %656
  %658 = bitcast i8* %657 to %mpz*
  %659 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %660 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %659, i64 0, i32 3
  store %mpz* %658, %mpz** %660
  %661 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %662 = load i8*, i8** %661
  %663 = bitcast i8* %662 to %mpz*
  %664 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }*
  %665 = getelementptr inbounds { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }, { %blockheader, [0 x i64], %mpz*, %mpz*, %mpz* }* %664, i64 0, i32 4
  store %mpz* %663, %mpz** %665
  ret void

tag88:                                            ; preds = %entry
  %666 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %667 = load i8*, i8** %666
  %668 = bitcast i8* %667 to %block*
  %669 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %670 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %669, i64 0, i32 2
  store %block* %668, %block** %670
  %671 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %672 = load i8*, i8** %671
  %673 = bitcast i8* %672 to %block*
  %674 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %675 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %674, i64 0, i32 3
  store %block* %673, %block** %675
  ret void

tag89:                                            ; preds = %entry
  %676 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %677 = load i8*, i8** %676
  %678 = bitcast i8* %677 to %block*
  %679 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %680 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %679, i64 0, i32 2
  store %block* %678, %block** %680
  %681 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %682 = load i8*, i8** %681
  %683 = bitcast i8* %682 to %block*
  %684 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %685 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %684, i64 0, i32 3
  store %block* %683, %block** %685
  ret void

tag91:                                            ; preds = %entry
  %686 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %687 = load i8*, i8** %686
  %688 = bitcast i8* %687 to %block*
  %689 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %690 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %689, i64 0, i32 2
  store %block* %688, %block** %690
  ret void

tag92:                                            ; preds = %entry
  %691 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %692 = load i8*, i8** %691
  %693 = bitcast i8* %692 to %block*
  %694 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %695 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %694, i64 0, i32 2
  store %block* %693, %block** %695
  ret void

tag93:                                            ; preds = %entry
  %696 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %697 = load i8*, i8** %696
  %698 = bitcast i8* %697 to %block*
  %699 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %700 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %699, i64 0, i32 2
  store %block* %698, %block** %700
  ret void

tag94:                                            ; preds = %entry
  %701 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %702 = load i8*, i8** %701
  %703 = bitcast i8* %702 to %block*
  %704 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %705 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %704, i64 0, i32 2
  store %block* %703, %block** %705
  ret void

tag95:                                            ; preds = %entry
  %706 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %707 = load i8*, i8** %706
  %708 = bitcast i8* %707 to %block*
  %709 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %710 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %709, i64 0, i32 2
  store %block* %708, %block** %710
  ret void

tag96:                                            ; preds = %entry
  %711 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %712 = load i8*, i8** %711
  %713 = bitcast i8* %712 to %block*
  %714 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %715 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %714, i64 0, i32 2
  store %block* %713, %block** %715
  ret void

tag97:                                            ; preds = %entry
  %716 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %717 = load i8*, i8** %716
  %718 = bitcast i8* %717 to %block*
  %719 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %720 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %719, i64 0, i32 2
  store %block* %718, %block** %720
  ret void

tag98:                                            ; preds = %entry
  %721 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %722 = load i8*, i8** %721
  %723 = bitcast i8* %722 to %block*
  %724 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %725 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %724, i64 0, i32 2
  store %block* %723, %block** %725
  ret void

tag99:                                            ; preds = %entry
  %726 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %727 = load i8*, i8** %726
  %728 = bitcast i8* %727 to %mpz*
  %729 = bitcast %block* %0 to { %blockheader, [0 x i64], %mpz* }*
  %730 = getelementptr inbounds { %blockheader, [0 x i64], %mpz* }, { %blockheader, [0 x i64], %mpz* }* %729, i64 0, i32 2
  store %mpz* %728, %mpz** %730
  ret void

tag100:                                           ; preds = %entry
  %731 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %732 = load i8*, i8** %731
  %733 = bitcast i8* %732 to i1*
  %734 = load i1, i1* %733
  tail call void @free(i8* %732)
  %735 = bitcast %block* %0 to { %blockheader, [0 x i64], i1 }*
  %736 = getelementptr inbounds { %blockheader, [0 x i64], i1 }, { %blockheader, [0 x i64], i1 }* %735, i64 0, i32 2
  store i1 %734, i1* %736
  ret void

tag101:                                           ; preds = %entry
  %737 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %738 = load i8*, i8** %737
  %739 = bitcast i8* %738 to %block*
  %740 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %741 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %740, i64 0, i32 2
  store %block* %739, %block** %741
  ret void

tag102:                                           ; preds = %entry
  %742 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %743 = load i8*, i8** %742
  %744 = bitcast i8* %743 to %block*
  %745 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %746 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %745, i64 0, i32 2
  store %block* %744, %block** %746
  ret void

tag103:                                           ; preds = %entry
  %747 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %748 = load i8*, i8** %747
  %749 = bitcast i8* %748 to %block*
  %750 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %751 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %750, i64 0, i32 2
  store %block* %749, %block** %751
  ret void

tag104:                                           ; preds = %entry
  %752 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %753 = load i8*, i8** %752
  %754 = bitcast i8* %753 to %block*
  %755 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %756 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %755, i64 0, i32 2
  store %block* %754, %block** %756
  ret void

tag105:                                           ; preds = %entry
  %757 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %758 = load i8*, i8** %757
  %759 = bitcast i8* %758 to %list*
  %760 = load %list, %list* %759
  tail call void @free(i8* %758)
  %761 = bitcast %block* %0 to { %blockheader, [0 x i64], %list }*
  %762 = getelementptr inbounds { %blockheader, [0 x i64], %list }, { %blockheader, [0 x i64], %list }* %761, i64 0, i32 2
  store %list %760, %list* %762
  ret void

tag106:                                           ; preds = %entry
  %763 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %764 = load i8*, i8** %763
  %765 = bitcast i8* %764 to %set*
  %766 = load %set, %set* %765
  tail call void @free(i8* %764)
  %767 = bitcast %block* %0 to { %blockheader, [0 x i64], %set }*
  %768 = getelementptr inbounds { %blockheader, [0 x i64], %set }, { %blockheader, [0 x i64], %set }* %767, i64 0, i32 2
  store %set %766, %set* %768
  ret void

tag107:                                           ; preds = %entry
  %769 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %770 = load i8*, i8** %769
  %771 = bitcast i8* %770 to %block*
  %772 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %773 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %772, i64 0, i32 2
  store %block* %771, %block** %773
  ret void

tag108:                                           ; preds = %entry
  %774 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %775 = load i8*, i8** %774
  %776 = bitcast i8* %775 to %block*
  %777 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %778 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %777, i64 0, i32 2
  store %block* %776, %block** %778
  ret void

tag109:                                           ; preds = %entry
  %779 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %780 = load i8*, i8** %779
  %781 = bitcast i8* %780 to %map*
  %782 = load %map, %map* %781
  tail call void @free(i8* %780)
  %783 = bitcast %block* %0 to { %blockheader, [0 x i64], %map }*
  %784 = getelementptr inbounds { %blockheader, [0 x i64], %map }, { %blockheader, [0 x i64], %map }* %783, i64 0, i32 2
  store %map %782, %map* %784
  ret void

tag110:                                           ; preds = %entry
  %785 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %786 = load i8*, i8** %785
  %787 = bitcast i8* %786 to %block*
  %788 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %789 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %788, i64 0, i32 2
  store %block* %787, %block** %789
  ret void

tag111:                                           ; preds = %entry
  %790 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %791 = load i8*, i8** %790
  %792 = bitcast i8* %791 to %block*
  %793 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %794 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %793, i64 0, i32 2
  store %block* %792, %block** %794
  ret void

tag112:                                           ; preds = %entry
  %795 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %796 = load i8*, i8** %795
  %797 = bitcast i8* %796 to %block*
  %798 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %799 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %798, i64 0, i32 2
  store %block* %797, %block** %799
  ret void

tag113:                                           ; preds = %entry
  %800 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %801 = load i8*, i8** %800
  %802 = bitcast i8* %801 to %block*
  %803 = bitcast %block* %0 to { %blockheader, [0 x i64], %block* }*
  %804 = getelementptr inbounds { %blockheader, [0 x i64], %block* }, { %blockheader, [0 x i64], %block* }* %803, i64 0, i32 2
  store %block* %802, %block** %804
  ret void

tag114:                                           ; preds = %entry
  %805 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %806 = load i8*, i8** %805
  %807 = bitcast i8* %806 to %block*
  %808 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %809 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %808, i64 0, i32 2
  store %block* %807, %block** %809
  %810 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %811 = load i8*, i8** %810
  %812 = bitcast i8* %811 to %block*
  %813 = bitcast %block* %0 to { %blockheader, [0 x i64], %block*, %block* }*
  %814 = getelementptr inbounds { %blockheader, [0 x i64], %block*, %block* }, { %blockheader, [0 x i64], %block*, %block* }* %813, i64 0, i32 3
  store %block* %812, %block** %814
  ret void

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare void @free(i8*)

define i8* @evaluateFunctionSymbol(i32, [0 x i8*]*) {
entry:
  switch i32 %0, label %stuck [
    i32 14, label %tag14
    i32 18, label %tag18
    i32 19, label %tag19
    i32 20, label %tag20
    i32 21, label %tag21
    i32 22, label %tag22
    i32 23, label %tag23
    i32 24, label %tag24
    i32 25, label %tag25
    i32 26, label %tag26
    i32 27, label %tag27
    i32 28, label %tag28
    i32 29, label %tag29
    i32 30, label %tag30
    i32 31, label %tag31
    i32 32, label %tag32
    i32 33, label %tag33
    i32 37, label %tag37
    i32 38, label %tag38
    i32 39, label %tag39
    i32 40, label %tag40
    i32 42, label %tag42
    i32 43, label %tag43
    i32 44, label %tag44
    i32 46, label %tag46
    i32 50, label %tag50
    i32 51, label %tag51
    i32 52, label %tag52
    i32 53, label %tag53
    i32 54, label %tag54
    i32 56, label %tag56
    i32 57, label %tag57
    i32 58, label %tag58
    i32 60, label %tag60
    i32 61, label %tag61
    i32 62, label %tag62
    i32 63, label %tag63
    i32 64, label %tag64
    i32 65, label %tag65
    i32 66, label %tag66
    i32 67, label %tag67
    i32 68, label %tag68
    i32 69, label %tag69
    i32 70, label %tag70
    i32 71, label %tag71
    i32 72, label %tag72
    i32 73, label %tag73
    i32 74, label %tag74
    i32 75, label %tag75
    i32 76, label %tag76
    i32 77, label %tag77
    i32 78, label %tag78
    i32 79, label %tag79
    i32 80, label %tag80
    i32 81, label %tag81
    i32 82, label %tag82
    i32 83, label %tag83
    i32 84, label %tag84
    i32 85, label %tag85
    i32 86, label %tag86
    i32 87, label %tag87
    i32 89, label %tag89
  ]

tag14:                                            ; preds = %entry
  %2 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %3 = load i8*, i8** %2
  %4 = bitcast i8* %3 to i1*
  %5 = load i1, i1* %4
  tail call void @free(i8* %3)
  %6 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %7 = load i8*, i8** %6
  %8 = bitcast i8* %7 to %block*
  %9 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %10 = load i8*, i8** %9
  %11 = bitcast i8* %10 to %block*
  br i1 %5, label %then, label %else

then:                                             ; preds = %tag14
  br label %hook_KEQUAL_ite

else:                                             ; preds = %tag14
  br label %hook_KEQUAL_ite

hook_KEQUAL_ite:                                  ; preds = %else, %then
  %phi = phi %block* [ %8, %then ], [ %11, %else ]
  %12 = bitcast %block* %phi to i8*
  br label %exit

tag18:                                            ; preds = %entry
  %13 = call %map @hook_MAP_unit()
  %malloccall = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall to %map*
  store %map %13, %map* %14
  %15 = bitcast %map* %14 to i8*
  br label %exit

tag19:                                            ; preds = %entry
  %16 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %17 = load i8*, i8** %16
  %18 = bitcast i8* %17 to %mpz*
  %19 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %20 = load i8*, i8** %19
  %21 = bitcast i8* %20 to %mpz*
  %22 = call %mpz* @hook_INT_sub(%mpz* %18, %mpz* %21)
  %23 = bitcast %mpz* %22 to i8*
  br label %exit

tag20:                                            ; preds = %entry
  %24 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %25 = load i8*, i8** %24
  %26 = bitcast i8* %25 to %map*
  %27 = load %map, %map* %26
  tail call void @free(i8* %25)
  %28 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %29 = load i8*, i8** %28
  %30 = bitcast i8* %29 to %map*
  %31 = load %map, %map* %30
  tail call void @free(i8* %29)
  %32 = alloca %map
  store %map %27, %map* %32
  %33 = alloca %map
  store %map %31, %map* %33
  %34 = call %map @hook_MAP_concat(%map* %32, %map* %33)
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %35 = bitcast i8* %malloccall1 to %map*
  store %map %34, %map* %35
  %36 = bitcast %map* %35 to i8*
  br label %exit

tag21:                                            ; preds = %entry
  %37 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %38 = load i8*, i8** %37
  %39 = bitcast i8* %38 to i1*
  %40 = load i1, i1* %39
  tail call void @free(i8* %38)
  %41 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %42 = load i8*, i8** %41
  %43 = bitcast i8* %42 to i1*
  %44 = load i1, i1* %43
  tail call void @free(i8* %42)
  br i1 %40, label %then2, label %hook_BOOL_and

then2:                                            ; preds = %tag21
  br label %hook_BOOL_and

hook_BOOL_and:                                    ; preds = %then2, %tag21
  %phi3 = phi i1 [ %44, %then2 ], [ %40, %tag21 ]
  %malloccall4 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %45 = bitcast i8* %malloccall4 to i1*
  store i1 %phi3, i1* %45
  %46 = bitcast i1* %45 to i8*
  br label %exit

tag22:                                            ; preds = %entry
  %47 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %48 = load i8*, i8** %47
  %49 = bitcast i8* %48 to i1*
  %50 = load i1, i1* %49
  tail call void @free(i8* %48)
  %51 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %52 = load i8*, i8** %51
  %53 = bitcast i8* %52 to i1*
  %54 = load i1, i1* %53
  tail call void @free(i8* %52)
  br i1 %50, label %then5, label %hook_BOOL_and6

then5:                                            ; preds = %tag22
  br label %hook_BOOL_and6

hook_BOOL_and6:                                   ; preds = %then5, %tag22
  %phi7 = phi i1 [ %54, %then5 ], [ %50, %tag22 ]
  %malloccall8 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %55 = bitcast i8* %malloccall8 to i1*
  store i1 %phi7, i1* %55
  %56 = bitcast i1* %55 to i8*
  br label %exit

tag23:                                            ; preds = %entry
  %57 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %58 = load i8*, i8** %57
  %59 = bitcast i8* %58 to %mpz*
  %60 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %61 = load i8*, i8** %60
  %62 = bitcast i8* %61 to %mpz*
  %63 = call %mpz* @hook_INT_ediv(%mpz* %59, %mpz* %62)
  %64 = bitcast %mpz* %63 to i8*
  br label %exit

tag24:                                            ; preds = %entry
  %65 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %66 = load i8*, i8** %65
  %67 = bitcast i8* %66 to %mpz*
  %68 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %69 = load i8*, i8** %68
  %70 = bitcast i8* %69 to %mpz*
  %71 = call i1 @"eval_Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{SortInt{},SortInt{}}"(%mpz* %67, %mpz* %70)
  %malloccall9 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %72 = bitcast i8* %malloccall9 to i1*
  store i1 %71, i1* %72
  %73 = bitcast i1* %72 to i8*
  br label %exit

tag25:                                            ; preds = %entry
  %74 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %75 = load i8*, i8** %74
  %76 = bitcast i8* %75 to i1*
  %77 = load i1, i1* %76
  tail call void @free(i8* %75)
  %78 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %79 = load i8*, i8** %78
  %80 = bitcast i8* %79 to i1*
  %81 = load i1, i1* %80
  tail call void @free(i8* %79)
  br i1 %77, label %then10, label %hook_BOOL_implies

then10:                                           ; preds = %tag25
  br label %hook_BOOL_implies

hook_BOOL_implies:                                ; preds = %then10, %tag25
  %phi11 = phi i1 [ %81, %then10 ], [ true, %tag25 ]
  %malloccall12 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %82 = bitcast i8* %malloccall12 to i1*
  store i1 %phi11, i1* %82
  %83 = bitcast i1* %82 to i8*
  br label %exit

tag26:                                            ; preds = %entry
  %84 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %85 = load i8*, i8** %84
  %86 = bitcast i8* %85 to %mpz*
  %87 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %88 = load i8*, i8** %87
  %89 = bitcast i8* %88 to %mpz*
  %90 = call %mpz* @hook_INT_emod(%mpz* %86, %mpz* %89)
  %91 = bitcast %mpz* %90 to i8*
  br label %exit

tag27:                                            ; preds = %entry
  %92 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %93 = load i8*, i8** %92
  %94 = bitcast i8* %93 to i1*
  %95 = load i1, i1* %94
  tail call void @free(i8* %93)
  %96 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %97 = load i8*, i8** %96
  %98 = bitcast i8* %97 to i1*
  %99 = load i1, i1* %98
  tail call void @free(i8* %97)
  br i1 %95, label %hook_BOOL_or, label %else13

else13:                                           ; preds = %tag27
  br label %hook_BOOL_or

hook_BOOL_or:                                     ; preds = %else13, %tag27
  %phi14 = phi i1 [ %99, %else13 ], [ %95, %tag27 ]
  %malloccall15 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %100 = bitcast i8* %malloccall15 to i1*
  store i1 %phi14, i1* %100
  %101 = bitcast i1* %100 to i8*
  br label %exit

tag28:                                            ; preds = %entry
  %102 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %103 = load i8*, i8** %102
  %104 = bitcast i8* %103 to i1*
  %105 = load i1, i1* %104
  tail call void @free(i8* %103)
  %106 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %107 = load i8*, i8** %106
  %108 = bitcast i8* %107 to i1*
  %109 = load i1, i1* %108
  tail call void @free(i8* %107)
  br i1 %105, label %hook_BOOL_or17, label %else16

else16:                                           ; preds = %tag28
  br label %hook_BOOL_or17

hook_BOOL_or17:                                   ; preds = %else16, %tag28
  %phi18 = phi i1 [ %109, %else16 ], [ %105, %tag28 ]
  %malloccall19 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %110 = bitcast i8* %malloccall19 to i1*
  store i1 %phi18, i1* %110
  %111 = bitcast i1* %110 to i8*
  br label %exit

tag29:                                            ; preds = %entry
  %112 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %113 = load i8*, i8** %112
  %114 = bitcast i8* %113 to i1*
  %115 = load i1, i1* %114
  tail call void @free(i8* %113)
  %116 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %117 = load i8*, i8** %116
  %118 = bitcast i8* %117 to i1*
  %119 = load i1, i1* %118
  tail call void @free(i8* %117)
  %hook_BOOL_ne = xor i1 %115, %119
  %malloccall20 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %120 = bitcast i8* %malloccall20 to i1*
  store i1 %hook_BOOL_ne, i1* %120
  %121 = bitcast i1* %120 to i8*
  br label %exit

tag30:                                            ; preds = %entry
  %122 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %123 = load i8*, i8** %122
  %124 = bitcast i8* %123 to %mpz*
  %125 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %126 = load i8*, i8** %125
  %127 = bitcast i8* %126 to %mpz*
  %128 = call %mpz* @hook_INT_shr(%mpz* %124, %mpz* %127)
  %129 = bitcast %mpz* %128 to i8*
  br label %exit

tag31:                                            ; preds = %entry
  %130 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %131 = load i8*, i8** %130
  %132 = bitcast i8* %131 to %mpz*
  %133 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %134 = load i8*, i8** %133
  %135 = bitcast i8* %134 to %mpz*
  %136 = call i1 @hook_INT_ge(%mpz* %132, %mpz* %135)
  %malloccall21 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %137 = bitcast i8* %malloccall21 to i1*
  store i1 %136, i1* %137
  %138 = bitcast i1* %137 to i8*
  br label %exit

tag32:                                            ; preds = %entry
  %139 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %140 = load i8*, i8** %139
  %141 = bitcast i8* %140 to %mpz*
  %142 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %143 = load i8*, i8** %142
  %144 = bitcast i8* %143 to %mpz*
  %145 = call %mpz* @hook_INT_shl(%mpz* %141, %mpz* %144)
  %146 = bitcast %mpz* %145 to i8*
  br label %exit

tag33:                                            ; preds = %entry
  %147 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %148 = load i8*, i8** %147
  %149 = bitcast i8* %148 to %mpz*
  %150 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %151 = load i8*, i8** %150
  %152 = bitcast i8* %151 to %mpz*
  %153 = call i1 @hook_INT_le(%mpz* %149, %mpz* %152)
  %malloccall22 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %154 = bitcast i8* %malloccall22 to i1*
  store i1 %153, i1* %154
  %155 = bitcast i1* %154 to i8*
  br label %exit

tag37:                                            ; preds = %entry
  %156 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %157 = load i8*, i8** %156
  %158 = bitcast i8* %157 to i1*
  %159 = load i1, i1* %158
  tail call void @free(i8* %157)
  %160 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %161 = load i8*, i8** %160
  %162 = bitcast i8* %161 to i1*
  %163 = load i1, i1* %162
  tail call void @free(i8* %161)
  %hook_BOOL_eq = icmp eq i1 %159, %163
  %malloccall23 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %164 = bitcast i8* %malloccall23 to i1*
  store i1 %hook_BOOL_eq, i1* %164
  %165 = bitcast i1* %164 to i8*
  br label %exit

tag38:                                            ; preds = %entry
  %166 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %167 = load i8*, i8** %166
  %168 = bitcast i8* %167 to %mpz*
  %169 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %170 = load i8*, i8** %169
  %171 = bitcast i8* %170 to %mpz*
  %172 = call i1 @hook_INT_eq(%mpz* %168, %mpz* %171)
  %malloccall24 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %173 = bitcast i8* %malloccall24 to i1*
  store i1 %172, i1* %173
  %174 = bitcast i1* %173 to i8*
  br label %exit

tag39:                                            ; preds = %entry
  %175 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %176 = load i8*, i8** %175
  %177 = bitcast i8* %176 to i1*
  %178 = load i1, i1* %177
  tail call void @free(i8* %176)
  %179 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %180 = load i8*, i8** %179
  %181 = bitcast i8* %180 to i1*
  %182 = load i1, i1* %181
  tail call void @free(i8* %180)
  %hook_BOOL_ne25 = xor i1 %178, %182
  %malloccall26 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %183 = bitcast i8* %malloccall26 to i1*
  store i1 %hook_BOOL_ne25, i1* %183
  %184 = bitcast i1* %183 to i8*
  br label %exit

tag40:                                            ; preds = %entry
  %185 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %186 = load i8*, i8** %185
  %187 = bitcast i8* %186 to %mpz*
  %188 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %189 = load i8*, i8** %188
  %190 = bitcast i8* %189 to %mpz*
  %191 = call i1 @hook_INT_ne(%mpz* %187, %mpz* %190)
  %malloccall27 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %192 = bitcast i8* %malloccall27 to i1*
  store i1 %191, i1* %192
  %193 = bitcast i1* %192 to i8*
  br label %exit

tag42:                                            ; preds = %entry
  %194 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %195 = load i8*, i8** %194
  %196 = bitcast i8* %195 to %mpz*
  %197 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %198 = load i8*, i8** %197
  %199 = bitcast i8* %198 to %mpz*
  %200 = call %mpz* @hook_INT_tmod(%mpz* %196, %mpz* %199)
  %201 = bitcast %mpz* %200 to i8*
  br label %exit

tag43:                                            ; preds = %entry
  %202 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %203 = load i8*, i8** %202
  %204 = bitcast i8* %203 to %block*
  %205 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %206 = load i8*, i8** %205
  %207 = bitcast i8* %206 to %block*
  %208 = call %map @hook_MAP_element(%block* %204, %block* %207)
  %malloccall28 = tail call i8* @malloc(i64 ptrtoint (%map* getelementptr (%map, %map* null, i32 1) to i64))
  %209 = bitcast i8* %malloccall28 to %map*
  store %map %208, %map* %209
  %210 = bitcast %map* %209 to i8*
  br label %exit

tag44:                                            ; preds = %entry
  %211 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %212 = load i8*, i8** %211
  %213 = bitcast i8* %212 to %mpz*
  %214 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %215 = load i8*, i8** %214
  %216 = bitcast i8* %215 to %mpz*
  %217 = call %mpz* @hook_INT_add(%mpz* %213, %mpz* %216)
  %218 = bitcast %mpz* %217 to i8*
  br label %exit

tag46:                                            ; preds = %entry
  %219 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %220 = load i8*, i8** %219
  %221 = bitcast i8* %220 to %mpz*
  %222 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %223 = load i8*, i8** %222
  %224 = bitcast i8* %223 to %mpz*
  %225 = call %mpz* @hook_INT_tdiv(%mpz* %221, %mpz* %224)
  %226 = bitcast %mpz* %225 to i8*
  br label %exit

tag50:                                            ; preds = %entry
  %227 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %228 = load i8*, i8** %227
  %229 = bitcast i8* %228 to %map*
  %230 = load %map, %map* %229
  tail call void @free(i8* %228)
  %231 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %232 = load i8*, i8** %231
  %233 = bitcast i8* %232 to %block*
  %234 = alloca %map
  store %map %230, %map* %234
  %235 = call %block* @hook_MAP_lookup(%map* %234, %block* %233)
  %236 = bitcast %block* %235 to i8*
  br label %exit

tag51:                                            ; preds = %entry
  %237 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %238 = load i8*, i8** %237
  %239 = bitcast i8* %238 to %block*
  %240 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %241 = load i8*, i8** %240
  %242 = bitcast i8* %241 to %set*
  %243 = load %set, %set* %242
  tail call void @free(i8* %241)
  %244 = alloca %set
  store %set %243, %set* %244
  %245 = call i1 @hook_SET_in(%block* %239, %set* %244)
  %malloccall29 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %246 = bitcast i8* %malloccall29 to i1*
  store i1 %245, i1* %246
  %247 = bitcast i1* %246 to i8*
  br label %exit

tag52:                                            ; preds = %entry
  %248 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %249 = load i8*, i8** %248
  %250 = bitcast i8* %249 to %mpz*
  %251 = call %mpz* @hook_INT_abs(%mpz* %250)
  %252 = bitcast %mpz* %251 to i8*
  br label %exit

tag53:                                            ; preds = %entry
  %253 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %254 = load i8*, i8** %253
  %255 = bitcast i8* %254 to %mpz*
  %256 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %257 = load i8*, i8** %256
  %258 = bitcast i8* %257 to %mpz*
  %259 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %260 = load i8*, i8** %259
  %261 = bitcast i8* %260 to %mpz*
  %262 = call %mpz* @hook_INT_bitRange(%mpz* %255, %mpz* %258, %mpz* %261)
  %263 = bitcast %mpz* %262 to i8*
  br label %exit

tag54:                                            ; preds = %entry
  %264 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %265 = load i8*, i8** %264
  %266 = bitcast i8* %265 to %mpz*
  %267 = call %mpz* @"eval_LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{SortInt{}}"(%mpz* %266)
  %268 = bitcast %mpz* %267 to i8*
  br label %exit

tag56:                                            ; preds = %entry
  %269 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %270 = load i8*, i8** %269
  %271 = bitcast i8* %270 to %map*
  %272 = load %map, %map* %271
  tail call void @free(i8* %270)
  %273 = alloca %map
  store %map %272, %map* %273
  %274 = call %block* @"eval_LblinitKCell{SortMap{}}"(%map* %273)
  %275 = bitcast %block* %274 to i8*
  br label %exit

tag57:                                            ; preds = %entry
  %276 = call %block* @"eval_LblinitStateCell{}"()
  %277 = bitcast %block* %276 to i8*
  br label %exit

tag58:                                            ; preds = %entry
  %278 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %279 = load i8*, i8** %278
  %280 = bitcast i8* %279 to %map*
  %281 = load %map, %map* %280
  tail call void @free(i8* %279)
  %282 = alloca %map
  store %map %281, %map* %282
  %283 = call %block* @"eval_LblinitTCell{SortMap{}}"(%map* %282)
  %284 = bitcast %block* %283 to i8*
  br label %exit

tag60:                                            ; preds = %entry
  %285 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %286 = load i8*, i8** %285
  %287 = bitcast i8* %286 to %block*
  %288 = call i1 @"eval_LblisAExp{SortK{}}"(%block* %287)
  %malloccall30 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %289 = bitcast i8* %malloccall30 to i1*
  store i1 %288, i1* %289
  %290 = bitcast i1* %289 to i8*
  br label %exit

tag61:                                            ; preds = %entry
  %291 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %292 = load i8*, i8** %291
  %293 = bitcast i8* %292 to %block*
  %294 = call i1 @"eval_LblisBExp{SortK{}}"(%block* %293)
  %malloccall31 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %295 = bitcast i8* %malloccall31 to i1*
  store i1 %294, i1* %295
  %296 = bitcast i1* %295 to i8*
  br label %exit

tag62:                                            ; preds = %entry
  %297 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %298 = load i8*, i8** %297
  %299 = bitcast i8* %298 to %block*
  %300 = call i1 @"eval_LblisBlock{SortK{}}"(%block* %299)
  %malloccall32 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %301 = bitcast i8* %malloccall32 to i1*
  store i1 %300, i1* %301
  %302 = bitcast i1* %301 to i8*
  br label %exit

tag63:                                            ; preds = %entry
  %303 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %304 = load i8*, i8** %303
  %305 = bitcast i8* %304 to %block*
  %306 = call i1 @"eval_LblisBool{SortK{}}"(%block* %305)
  %malloccall33 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %307 = bitcast i8* %malloccall33 to i1*
  store i1 %306, i1* %307
  %308 = bitcast i1* %307 to i8*
  br label %exit

tag64:                                            ; preds = %entry
  %309 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %310 = load i8*, i8** %309
  %311 = bitcast i8* %310 to %block*
  %312 = call i1 @"eval_LblisCell{SortK{}}"(%block* %311)
  %malloccall34 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %313 = bitcast i8* %malloccall34 to i1*
  store i1 %312, i1* %313
  %314 = bitcast i1* %313 to i8*
  br label %exit

tag65:                                            ; preds = %entry
  %315 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %316 = load i8*, i8** %315
  %317 = bitcast i8* %316 to %block*
  %318 = call i1 @"eval_LblisId{SortK{}}"(%block* %317)
  %malloccall35 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %319 = bitcast i8* %malloccall35 to i1*
  store i1 %318, i1* %319
  %320 = bitcast i1* %319 to i8*
  br label %exit

tag66:                                            ; preds = %entry
  %321 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %322 = load i8*, i8** %321
  %323 = bitcast i8* %322 to %block*
  %324 = call i1 @"eval_LblisIds{SortK{}}"(%block* %323)
  %malloccall36 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %325 = bitcast i8* %malloccall36 to i1*
  store i1 %324, i1* %325
  %326 = bitcast i1* %325 to i8*
  br label %exit

tag67:                                            ; preds = %entry
  %327 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %328 = load i8*, i8** %327
  %329 = bitcast i8* %328 to %block*
  %330 = call i1 @"eval_LblisInt{SortK{}}"(%block* %329)
  %malloccall37 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %331 = bitcast i8* %malloccall37 to i1*
  store i1 %330, i1* %331
  %332 = bitcast i1* %331 to i8*
  br label %exit

tag68:                                            ; preds = %entry
  %333 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %334 = load i8*, i8** %333
  %335 = bitcast i8* %334 to %block*
  %336 = call i1 @"eval_LblisK{SortK{}}"(%block* %335)
  %malloccall38 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %337 = bitcast i8* %malloccall38 to i1*
  store i1 %336, i1* %337
  %338 = bitcast i1* %337 to i8*
  br label %exit

tag69:                                            ; preds = %entry
  %339 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %340 = load i8*, i8** %339
  %341 = bitcast i8* %340 to %block*
  %342 = call i1 @"eval_LblisKCell{SortK{}}"(%block* %341)
  %malloccall39 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %343 = bitcast i8* %malloccall39 to i1*
  store i1 %342, i1* %343
  %344 = bitcast i1* %343 to i8*
  br label %exit

tag70:                                            ; preds = %entry
  %345 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %346 = load i8*, i8** %345
  %347 = bitcast i8* %346 to %block*
  %348 = call i1 @"eval_LblisKCellOpt{SortK{}}"(%block* %347)
  %malloccall40 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %349 = bitcast i8* %malloccall40 to i1*
  store i1 %348, i1* %349
  %350 = bitcast i1* %349 to i8*
  br label %exit

tag71:                                            ; preds = %entry
  %351 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %352 = load i8*, i8** %351
  %353 = bitcast i8* %352 to %block*
  %354 = call i1 @"eval_LblisKConfigVar{SortK{}}"(%block* %353)
  %malloccall41 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %355 = bitcast i8* %malloccall41 to i1*
  store i1 %354, i1* %355
  %356 = bitcast i1* %355 to i8*
  br label %exit

tag72:                                            ; preds = %entry
  %357 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %358 = load i8*, i8** %357
  %359 = bitcast i8* %358 to %block*
  %360 = call i1 @"eval_LblisKItem{SortK{}}"(%block* %359)
  %malloccall42 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %361 = bitcast i8* %malloccall42 to i1*
  store i1 %360, i1* %361
  %362 = bitcast i1* %361 to i8*
  br label %exit

tag73:                                            ; preds = %entry
  %363 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %364 = load i8*, i8** %363
  %365 = bitcast i8* %364 to %block*
  %366 = call i1 @"eval_LblisKResult{SortK{}}"(%block* %365)
  %malloccall43 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %367 = bitcast i8* %malloccall43 to i1*
  store i1 %366, i1* %367
  %368 = bitcast i1* %367 to i8*
  br label %exit

tag74:                                            ; preds = %entry
  %369 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %370 = load i8*, i8** %369
  %371 = bitcast i8* %370 to %block*
  %372 = call i1 @"eval_LblisList{SortK{}}"(%block* %371)
  %malloccall44 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %373 = bitcast i8* %malloccall44 to i1*
  store i1 %372, i1* %373
  %374 = bitcast i1* %373 to i8*
  br label %exit

tag75:                                            ; preds = %entry
  %375 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %376 = load i8*, i8** %375
  %377 = bitcast i8* %376 to %block*
  %378 = call i1 @"eval_LblisMap{SortK{}}"(%block* %377)
  %malloccall45 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %379 = bitcast i8* %malloccall45 to i1*
  store i1 %378, i1* %379
  %380 = bitcast i1* %379 to i8*
  br label %exit

tag76:                                            ; preds = %entry
  %381 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %382 = load i8*, i8** %381
  %383 = bitcast i8* %382 to %block*
  %384 = call i1 @"eval_LblisPgm{SortK{}}"(%block* %383)
  %malloccall46 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %385 = bitcast i8* %malloccall46 to i1*
  store i1 %384, i1* %385
  %386 = bitcast i1* %385 to i8*
  br label %exit

tag77:                                            ; preds = %entry
  %387 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %388 = load i8*, i8** %387
  %389 = bitcast i8* %388 to %block*
  %390 = call i1 @"eval_LblisSet{SortK{}}"(%block* %389)
  %malloccall47 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %391 = bitcast i8* %malloccall47 to i1*
  store i1 %390, i1* %391
  %392 = bitcast i1* %391 to i8*
  br label %exit

tag78:                                            ; preds = %entry
  %393 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %394 = load i8*, i8** %393
  %395 = bitcast i8* %394 to %block*
  %396 = call i1 @"eval_LblisStateCell{SortK{}}"(%block* %395)
  %malloccall48 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %397 = bitcast i8* %malloccall48 to i1*
  store i1 %396, i1* %397
  %398 = bitcast i1* %397 to i8*
  br label %exit

tag79:                                            ; preds = %entry
  %399 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %400 = load i8*, i8** %399
  %401 = bitcast i8* %400 to %block*
  %402 = call i1 @"eval_LblisStateCellOpt{SortK{}}"(%block* %401)
  %malloccall49 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %403 = bitcast i8* %malloccall49 to i1*
  store i1 %402, i1* %403
  %404 = bitcast i1* %403 to i8*
  br label %exit

tag80:                                            ; preds = %entry
  %405 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %406 = load i8*, i8** %405
  %407 = bitcast i8* %406 to %block*
  %408 = call i1 @"eval_LblisStmt{SortK{}}"(%block* %407)
  %malloccall50 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %409 = bitcast i8* %malloccall50 to i1*
  store i1 %408, i1* %409
  %410 = bitcast i1* %409 to i8*
  br label %exit

tag81:                                            ; preds = %entry
  %411 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %412 = load i8*, i8** %411
  %413 = bitcast i8* %412 to %block*
  %414 = call i1 @"eval_LblisString{SortK{}}"(%block* %413)
  %malloccall51 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %415 = bitcast i8* %malloccall51 to i1*
  store i1 %414, i1* %415
  %416 = bitcast i1* %415 to i8*
  br label %exit

tag82:                                            ; preds = %entry
  %417 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %418 = load i8*, i8** %417
  %419 = bitcast i8* %418 to %block*
  %420 = call i1 @"eval_LblisTCell{SortK{}}"(%block* %419)
  %malloccall52 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %421 = bitcast i8* %malloccall52 to i1*
  store i1 %420, i1* %421
  %422 = bitcast i1* %421 to i8*
  br label %exit

tag83:                                            ; preds = %entry
  %423 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %424 = load i8*, i8** %423
  %425 = bitcast i8* %424 to %block*
  %426 = call i1 @"eval_LblisTCellFragment{SortK{}}"(%block* %425)
  %malloccall53 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %427 = bitcast i8* %malloccall53 to i1*
  store i1 %426, i1* %427
  %428 = bitcast i1* %427 to i8*
  br label %exit

tag84:                                            ; preds = %entry
  %429 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %430 = load i8*, i8** %429
  %431 = bitcast i8* %430 to %map*
  %432 = load %map, %map* %431
  tail call void @free(i8* %430)
  %433 = alloca %map
  store %map %432, %map* %433
  %434 = call %set @hook_MAP_keys(%map* %433)
  %malloccall54 = tail call i8* @malloc(i64 ptrtoint (%set* getelementptr (%set, %set* null, i32 1) to i64))
  %435 = bitcast i8* %malloccall54 to %set*
  store %set %434, %set* %435
  %436 = bitcast %set* %435 to i8*
  br label %exit

tag85:                                            ; preds = %entry
  %437 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %438 = load i8*, i8** %437
  %439 = bitcast i8* %438 to %mpz*
  %440 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %441 = load i8*, i8** %440
  %442 = bitcast i8* %441 to %mpz*
  %443 = call %mpz* @hook_INT_min(%mpz* %439, %mpz* %442)
  %444 = bitcast %mpz* %443 to i8*
  br label %exit

tag86:                                            ; preds = %entry
  %445 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %446 = load i8*, i8** %445
  %447 = bitcast i8* %446 to i1*
  %448 = load i1, i1* %447
  tail call void @free(i8* %446)
  %hook_BOOL_not = xor i1 %448, true
  %malloccall55 = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %449 = bitcast i8* %malloccall55 to i1*
  store i1 %hook_BOOL_not, i1* %449
  %450 = bitcast i1* %449 to i8*
  br label %exit

tag87:                                            ; preds = %entry
  %451 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %452 = load i8*, i8** %451
  %453 = bitcast i8* %452 to %mpz*
  %454 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %455 = load i8*, i8** %454
  %456 = bitcast i8* %455 to %mpz*
  %457 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 2
  %458 = load i8*, i8** %457
  %459 = bitcast i8* %458 to %mpz*
  %460 = call %mpz* @hook_INT_signExtendBitRange(%mpz* %453, %mpz* %456, %mpz* %459)
  %461 = bitcast %mpz* %460 to i8*
  br label %exit

tag89:                                            ; preds = %entry
  %462 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 0
  %463 = load i8*, i8** %462
  %464 = bitcast i8* %463 to %block*
  %465 = getelementptr [0 x i8*], [0 x i8*]* %1, i64 0, i64 1
  %466 = load i8*, i8** %465
  %467 = bitcast i8* %466 to %block*
  %468 = call %block* @"eval_append{SortK{},SortK{}}"(%block* %464, %block* %467)
  %469 = bitcast %block* %468 to i8*
  br label %exit

exit:                                             ; preds = %tag89, %tag87, %tag86, %tag85, %tag84, %tag83, %tag82, %tag81, %tag80, %tag79, %tag78, %tag77, %tag76, %tag75, %tag74, %tag73, %tag72, %tag71, %tag70, %tag69, %tag68, %tag67, %tag66, %tag65, %tag64, %tag63, %tag62, %tag61, %tag60, %tag58, %tag57, %tag56, %tag54, %tag53, %tag52, %tag51, %tag50, %tag46, %tag44, %tag43, %tag42, %tag40, %tag39, %tag38, %tag37, %tag33, %tag32, %tag31, %tag30, %tag29, %hook_BOOL_or17, %hook_BOOL_or, %tag26, %hook_BOOL_implies, %tag24, %tag23, %hook_BOOL_and6, %hook_BOOL_and, %tag20, %tag19, %tag18, %hook_KEQUAL_ite
  %phi56 = phi i8* [ %12, %hook_KEQUAL_ite ], [ %15, %tag18 ], [ %23, %tag19 ], [ %36, %tag20 ], [ %46, %hook_BOOL_and ], [ %56, %hook_BOOL_and6 ], [ %64, %tag23 ], [ %73, %tag24 ], [ %83, %hook_BOOL_implies ], [ %91, %tag26 ], [ %101, %hook_BOOL_or ], [ %111, %hook_BOOL_or17 ], [ %121, %tag29 ], [ %129, %tag30 ], [ %138, %tag31 ], [ %146, %tag32 ], [ %155, %tag33 ], [ %165, %tag37 ], [ %174, %tag38 ], [ %184, %tag39 ], [ %193, %tag40 ], [ %201, %tag42 ], [ %210, %tag43 ], [ %218, %tag44 ], [ %226, %tag46 ], [ %236, %tag50 ], [ %247, %tag51 ], [ %252, %tag52 ], [ %263, %tag53 ], [ %268, %tag54 ], [ %275, %tag56 ], [ %277, %tag57 ], [ %284, %tag58 ], [ %290, %tag60 ], [ %296, %tag61 ], [ %302, %tag62 ], [ %308, %tag63 ], [ %314, %tag64 ], [ %320, %tag65 ], [ %326, %tag66 ], [ %332, %tag67 ], [ %338, %tag68 ], [ %344, %tag69 ], [ %350, %tag70 ], [ %356, %tag71 ], [ %362, %tag72 ], [ %368, %tag73 ], [ %374, %tag74 ], [ %380, %tag75 ], [ %386, %tag76 ], [ %392, %tag77 ], [ %398, %tag78 ], [ %404, %tag79 ], [ %410, %tag80 ], [ %416, %tag81 ], [ %422, %tag82 ], [ %428, %tag83 ], [ %436, %tag84 ], [ %444, %tag85 ], [ %450, %tag86 ], [ %461, %tag87 ], [ %469, %tag89 ]
  ret i8* %phi56

stuck:                                            ; preds = %entry
  call void @abort()
  unreachable
}

declare %map @hook_MAP_unit()

declare noalias i8* @malloc(i64)

declare %mpz* @hook_INT_sub(%mpz*, %mpz*)

declare %map @hook_MAP_concat(%map*, %map*)

declare %mpz* @hook_INT_ediv(%mpz*, %mpz*)

declare i1 @"eval_Lbl'Unds'dividesInt'UndsUnds'INT'UndsUnds'Int'Unds'Int{SortInt{},SortInt{}}"(%mpz*, %mpz*)

declare %mpz* @hook_INT_emod(%mpz*, %mpz*)

declare %mpz* @hook_INT_shr(%mpz*, %mpz*)

declare i1 @hook_INT_ge(%mpz*, %mpz*)

declare %mpz* @hook_INT_shl(%mpz*, %mpz*)

declare i1 @hook_INT_le(%mpz*, %mpz*)

declare i1 @hook_INT_eq(%mpz*, %mpz*)

declare i1 @hook_INT_ne(%mpz*, %mpz*)

declare %mpz* @hook_INT_tmod(%mpz*, %mpz*)

declare %map @hook_MAP_element(%block*, %block*)

declare %mpz* @hook_INT_add(%mpz*, %mpz*)

declare %mpz* @hook_INT_tdiv(%mpz*, %mpz*)

declare %block* @hook_MAP_lookup(%map*, %block*)

declare i1 @hook_SET_in(%block*, %set*)

declare %mpz* @hook_INT_abs(%mpz*)

declare %mpz* @hook_INT_bitRange(%mpz*, %mpz*, %mpz*)

declare %mpz* @"eval_LblfreshInt'LParUndsRParUnds'INT'UndsUnds'Int{SortInt{}}"(%mpz*)

declare %block* @"eval_LblinitKCell{SortMap{}}"(%map*)

declare %block* @"eval_LblinitStateCell{}"()

declare %block* @"eval_LblinitTCell{SortMap{}}"(%map*)

declare i1 @"eval_LblisAExp{SortK{}}"(%block*)

declare i1 @"eval_LblisBExp{SortK{}}"(%block*)

declare i1 @"eval_LblisBlock{SortK{}}"(%block*)

declare i1 @"eval_LblisBool{SortK{}}"(%block*)

declare i1 @"eval_LblisCell{SortK{}}"(%block*)

declare i1 @"eval_LblisId{SortK{}}"(%block*)

declare i1 @"eval_LblisIds{SortK{}}"(%block*)

declare i1 @"eval_LblisInt{SortK{}}"(%block*)

declare i1 @"eval_LblisK{SortK{}}"(%block*)

declare i1 @"eval_LblisKCell{SortK{}}"(%block*)

declare i1 @"eval_LblisKCellOpt{SortK{}}"(%block*)

declare i1 @"eval_LblisKConfigVar{SortK{}}"(%block*)

declare i1 @"eval_LblisKItem{SortK{}}"(%block*)

declare i1 @"eval_LblisKResult{SortK{}}"(%block*)

declare i1 @"eval_LblisList{SortK{}}"(%block*)

declare i1 @"eval_LblisMap{SortK{}}"(%block*)

declare i1 @"eval_LblisPgm{SortK{}}"(%block*)

declare i1 @"eval_LblisSet{SortK{}}"(%block*)

declare i1 @"eval_LblisStateCell{SortK{}}"(%block*)

declare i1 @"eval_LblisStateCellOpt{SortK{}}"(%block*)

declare i1 @"eval_LblisStmt{SortK{}}"(%block*)

declare i1 @"eval_LblisString{SortK{}}"(%block*)

declare i1 @"eval_LblisTCell{SortK{}}"(%block*)

declare i1 @"eval_LblisTCellFragment{SortK{}}"(%block*)

declare %set @hook_MAP_keys(%map*)

declare %mpz* @hook_INT_min(%mpz*, %mpz*)

declare %mpz* @hook_INT_signExtendBitRange(%mpz*, %mpz*, %mpz*)

declare %block* @"eval_append{SortK{},SortK{}}"(%block*, %block*)

define i8* @getToken(i8*, i64, i8*) {
is_SortList:
  %3 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @sort_name_SortList, i64 0, i64 0))
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %SortList, label %is_SortMap

SortList:                                         ; preds = %is_SortList
  call void @abort()
  unreachable

is_SortMap:                                       ; preds = %is_SortList
  %5 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sort_name_SortMap, i64 0, i64 0))
  %6 = icmp eq i32 %5, 0
  br i1 %6, label %SortMap, label %is_SortBool

SortMap:                                          ; preds = %is_SortMap
  call void @abort()
  unreachable

is_SortBool:                                      ; preds = %is_SortMap
  %7 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @sort_name_SortBool, i64 0, i64 0))
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %SortBool, label %is_SortInt

SortBool:                                         ; preds = %is_SortBool
  %9 = call i1 @string_equal(i8* %2, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @bool_true, i64 0, i64 0), i64 %1, i64 4)
  %malloccall = tail call i8* @malloc(i64 ptrtoint (i1* getelementptr (i1, i1* null, i32 1) to i64))
  %10 = bitcast i8* %malloccall to i1*
  store i1 %9, i1* %10
  %11 = bitcast i1* %10 to i8*
  br label %exit

is_SortInt:                                       ; preds = %is_SortBool
  %12 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sort_name_SortInt, i64 0, i64 0))
  %13 = icmp eq i32 %12, 0
  br i1 %13, label %SortInt, label %is_SortSet

SortInt:                                          ; preds = %is_SortInt
  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%mpz* getelementptr (%mpz, %mpz* null, i32 1) to i64))
  %14 = bitcast i8* %malloccall1 to %mpz*
  %15 = call i32 @__gmpz_init_set_str(%mpz* %14, i8* %2, i32 10)
  %16 = icmp eq i32 %15, 0
  %17 = bitcast %mpz* %14 to i8*
  br i1 %16, label %exit, label %invalid_int

invalid_int:                                      ; preds = %SortInt
  call void @abort()
  unreachable

is_SortSet:                                       ; preds = %is_SortInt
  %18 = call i32 @strcmp(i8* %0, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sort_name_SortSet, i64 0, i64 0))
  %19 = icmp eq i32 %18, 0
  br i1 %19, label %SortSet, label %symbol

SortSet:                                          ; preds = %is_SortSet
  call void @abort()
  unreachable

symbol:                                           ; preds = %is_SortSet
  %20 = add i64 %1, ptrtoint (%string* getelementptr (%string, %string* null, i32 1) to i64)
  %malloccall2 = tail call i8* @malloc(i64 %20)
  %21 = bitcast i8* %malloccall2 to %string*
  %22 = getelementptr inbounds %string, %string* %21, i64 0, i32 0, i32 0
  store i64 %1, i64* %22
  %23 = getelementptr inbounds %string, %string* %21, i64 0, i32 1, i64 0
  %24 = call i8* @memcpy(i8* %23, i8* %2, i64 %1)
  %25 = bitcast %string* %21 to i8*
  br label %exit

exit:                                             ; preds = %symbol, %SortInt, %SortBool
  %phi = phi i8* [ %11, %SortBool ], [ %17, %SortInt ], [ %25, %symbol ]
  ret i8* %phi
}

declare i1 @string_equal(i8*, i8*, i64, i64)

declare i32 @__gmpz_init_set_str(%mpz*, i8*, i32)

declare i8* @memcpy(i8*, i8*, i64)

attributes #0 = { noreturn }
