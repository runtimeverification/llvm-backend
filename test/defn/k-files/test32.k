module TOKENS 
  syntax UpperName 
  syntax LowerName 
  syntax DollarName 
  syntax Numbers 
  syntax BacktickName 
  syntax KoreString 
  syntax KModuleName  ::=
    UpperName 
  syntax KSort  ::=
    UpperName 
  syntax ModuleName  ::=
    KModuleName 
  | UpperName 
  | LowerName 
  syntax SymbolName  ::=
    UpperName 
  | LowerName 
  syntax SortName  ::=
    KSort 
  | UpperName 
  | LowerName 
  syntax VariableName  ::=
    UpperName 
  | LowerName 
endmodule

module TOKENS-SYNTAX 
  imports public TOKENS
/*  syntax UpperName  ::=
    r"[A-Z][A-Za-z\\-0-9'\\#]*" [autoReject, token]
  syntax LowerName  ::=
    r"[a-z][A-Za-z\\-0-9'\\#]*" [autoReject, token]
  | "left" [token]
  syntax Numbers  ::=
    r"[\\+-]?[0-9]+" [token]
  syntax DollarName  ::=
    r"(\\$)([A-Z][A-Za-z\\-0-9]*)" [token]
  syntax BacktickName  ::=
    r"`(\\\\`|\\\\\\\\|[^`\\\\\\n\\r])+`" [token]
  syntax KoreString  ::=
    r"[\\\"](([^\\\"\n\r\\\\])|([\\\\][nrtf\\\"\\\\])|([\\\\][x][0-9a-fA-F]{2})|([\\\\][u][0-9a-fA-F]{4})|([\\\\][U][0-9a-fA-F]{8}))*[\\\"]" [token]*/
endmodule

module KORE-COMMON 
  imports public TOKENS
  syntax LowerName  ::=
    "function" [token]
  | "functional" [token]
  | "constructor" [token]
  | "injective" [token]
  | "klabel" [token]
  syntax Sort  ::=
    SortName 
  | SortName "{" Sorts "}" [klabel(nameParam), symbol]
  syntax Sorts 
  syntax Symbol  ::=
    SymbolName "{" Sorts "}" [klabel(symbolSorts), symbol]
  syntax Variable  ::=
    VariableName ":" Sort [klabel(varType), symbol]
  syntax Pattern  ::=
    Variable 
  | Symbol "(" Patterns ")" [klabel(application), symbol]
  | "\\and" "{" Sort "}" "(" Pattern "," Pattern ")" [klabel(and)]
  | "\\not" "{" Sort "}" "(" Pattern ")" [klabel(not)]
  | "\\or" "{" Sort "}" "(" Pattern "," Pattern ")" [klabel(or)]
  | "\\implies" "{" Sort "}" "(" Pattern "," Pattern ")" [klabel(implies)]
  | "\\iff" "{" Sort "}" "(" Pattern "," Pattern ")" [klabel(iff)]
  | "\\forall" "{" Sort "}" "(" Variable "," Pattern ")" [klabel(forall)]
  | "\\exists" "{" Sort "}" "(" Variable "," Pattern ")" [klabel(exists)]
  | "\\ceil" "{" Sort "," Sort "}" "(" Pattern ")" [klabel(ceil)]
  | "\\floor" "{" Sort "," Sort "}" "(" Pattern ")" [klabel(floor)]
  | "\\equals" "{" Sort "," Sort "}" "(" Pattern "," Pattern ")" [klabel(equals)]
  | "\\in" "{" Sort "," Sort "}" "(" Pattern "," Pattern ")" [klabel(in)]
  | "\\top" "{" Sort "}" "(" ")" [klabel(top)]
  | "\\bottom" "{" Sort "}" "(" ")" [klabel(bottom)]
  | "\\next" "{" Sort "}" "(" Pattern ")" [klabel(next)]
  | "\\rewrites" "{" Sort "}" "(" Pattern "," Pattern ")" [klabel(rewrites)]
  | "\\dv" "{" Sort "}" "(" KoreString ")" [klabel(dv), symbol]
  syntax Patterns 
  syntax Attribute  ::=
    "[" Patterns "]" [klabel(koreAttributesLbl), symbol]
  syntax KoreDeclaration  ::=
    "hook-sort" Sort Attribute 
  | "hook-symbol" Symbol "(" Sorts ")" ":" Sort Attribute 
  | "sort" Sort Attribute [klabel(sortDeclaration), symbol]
  | "symbol" Symbol "(" Sorts ")" ":" Sort Attribute [klabel(symbolDeclaration), symbol]
  | "axiom" "{" Sorts "}" Pattern Attribute [klabel(axiomDeclaration), symbol]
  syntax Declaration  ::=
    KoreDeclaration 
  syntax Declarations 
  syntax Module 
  syntax Modules 
  syntax KoreDefinition 
  syntax Definition  ::=
    KoreDefinition 
endmodule

module KORE-SYNTAX 
  imports public KORE-COMMON
  imports public TOKENS-SYNTAX
  syntax EmptySorts  ::=
    "" [klabel(dotSorts)]
  syntax NeSorts  ::=
    Sort "," NeSorts [klabel(consSorts)]
  | Sort EmptySorts [klabel(consSorts)]
  syntax Sorts  ::=
    NeSorts 
  | EmptySorts 
  syntax KoreDeclaration  ::=
    "import" ModuleName Attribute [klabel(koreImportLbl), symbol]
  syntax EmptyPatterns  ::=
    "" [klabel(dotPatterns)]
  syntax NePatterns  ::=
    Pattern "," NePatterns [klabel(consPatterns)]
  | Pattern EmptyPatterns [klabel(consPatterns)]
  syntax Patterns  ::=
    NePatterns 
  | EmptyPatterns 
  syntax EmptyDeclarations  ::=
    "" [klabel(dotDeclarations)]
  syntax NeDeclarations  ::=
    Declaration NeDeclarations [klabel(consDeclarations)]
  | Declaration EmptyDeclarations [klabel(consDeclarations)]
  syntax Declarations  ::=
    NeDeclarations 
  | EmptyDeclarations 
  syntax EmptyModules  ::=
    "" [klabel(dotModules)]
  syntax NeModules  ::=
    Module NeModules [klabel(consModules)]
  | Module EmptyModules [klabel(consModules)]
  syntax Modules  ::=
    NeModules 
  | EmptyModules 
  syntax Module  ::=
    "module" ModuleName Declarations "endmodule" Attribute [format(%1 %2%i%n%3%n%d%4 %5%n), klabel(koreModuleLbl), symbol]
  syntax KoreDefinition  ::=
    Attribute Modules [format(%1%n%n%2), klabel(koreDefinitionLbl), symbol]
  syntax Layout  ::=
    r"(/\\*([^\\*]|(\\*+([^\\*/])))*\\*+/|//[^\n\r]*|[\\ \n\r\t])*" [klabel(layout)]
endmodule

module KORE-ABSTRACT 
  imports public KORE-COMMON
  syntax Sorts  ::=
    Sort 
  | Sort "," Sorts [klabel(consSorts), symbol]
  | ".Sorts" [klabel(dotSorts), symbol]
  syntax Patterns  ::=
    Pattern "," Patterns [klabel(consPatterns), symbol]
  | ".Patterns" [klabel(dotPatterns), symbol]
  syntax Declarations  ::=
    Declaration Declarations [format(%1%n%2), klabel(consDeclarations), symbol]
  | ".Declarations" [klabel(dotDeclarations), symbol]
  syntax Module  ::=
    "koreModule" "(" ModuleName "," Declarations "," Attribute ")" [format(%1 %2%i%n%3%n%d%4 %5%n), klabel(koreModuleLbl), symbol]
  syntax Modules  ::=
    Module Modules [format(%1%n%2), klabel(consModules), symbol]
  | ".Modules" [klabel(dotModules), symbol]
  syntax KoreDefinition  ::=
    "koreDefinition" "(" Attribute "," Modules ")" [format(%3%n%n%5), klabel(koreDefinitionLbl), symbol]
  syntax KoreDeclaration  ::=
    "koreImport" "(" ModuleName "," Attribute ")" [klabel(koreImportLbl), symbol]
endmodule

module EKORE-SYNTAX 
  imports public KORE-SYNTAX
  imports public K-PRODUCTION-SYNTAX
  imports public CONFIG-RULE-CONTEXT-SYNTAX
  imports public REWRITES-REQUIRES-IN-PATTERNS-SYNTAX
  imports public BACKTICK-PATTERNS-SYNTAX
  imports public K-DEFINITION-SYNTAX
  syntax Layout  ::=
    r"(/\\*([^\\*]|(\\*+([^\\*/])))*\\*+/|//[^\n\r]*|[\\ \n\r\t])*" [klabel(layout)]
endmodule

module EKORE-ABSTRACT 
  imports public KORE-ABSTRACT
  imports public K-PRODUCTION-ABSTRACT
  imports public CONFIG-RULE-CONTEXT-ABSTRACT
  imports public REWRITES-REQUIRES-IN-PATTERNS-ABSTRACT
  imports public BACKTICK-PATTERNS-ABSTRACT
  imports public K-DEFINITION-ABSTRACT
endmodule

module OUTER-SYNTAX 
  imports public KORE-SYNTAX
  imports public K-PRODUCTION-SYNTAX
  imports public K-DEFINITION-SYNTAX
  imports public RULES-WITH-BUBBLES-SYNTAX
  syntax Layout  ::=
    r"(/\\*([^\\*]|(\\*+([^\\*/])))*\\*+/|//[^\n\r]*|[\\ \n\r\t])*" [klabel(layout)]
endmodule

module OUTER-ABSTRACT 
  imports public KORE-ABSTRACT
  imports public K-PRODUCTION-ABSTRACT
  imports public K-DEFINITION-ABSTRACT
  imports public RULES-WITH-BUBBLES-ABSTRACT
endmodule

module RULES-WITH-BUBBLES-COMMON 
  imports public CONFIG-RULE-CONTEXT-COMMON
  syntax BubbleItem 
  syntax Bubble 
  syntax BubbleWithAttr 
  syntax RuleContents  ::=
    BubbleWithAttr 
endmodule

module RULES-WITH-BUBBLES-ABSTRACT 
  imports public RULES-WITH-BUBBLES-COMMON
  imports public CONFIG-RULE-CONTEXT-ABSTRACT
endmodule

module RULES-WITH-BUBBLES-SYNTAX 
  imports public RULES-WITH-BUBBLES-COMMON
  imports public CONFIG-RULE-CONTEXT-SYNTAX
  syntax BubbleWithAttr  ::=
    Bubble [klabel(noAttrsB)]
  | Bubble KAttributesDeclaration [klabel(attrsB), prefer]
  //syntax BubbleItem  ::=
  //  r"[^ \t\n\r]+" [reject2(rule|syntax|endmodule|configuration|context), token]
  syntax Bubble  ::=
    Bubble BubbleItem [token]
  | BubbleItem [token]
endmodule

module EKORE-BASE 
  imports public KORE-COMMON
  syntax EKoreDeclaration 
  syntax Declaration  ::=
    EKoreDeclaration 
endmodule

module K-PRODUCTION-COMMON 
  imports public EKORE-BASE
  imports public ATTRIBUTES-COMMON
  syntax Tag  ::=
    UpperName 
  | LowerName 
  syntax KNeTagSet  ::=
    Tag KNeTagSet [klabel(kTagSet), symbol]
  syntax AssocAttribute  ::=
    "left:" [klabel(leftAttribute), symbol]
  | "right:" [klabel(rightAttribute), symbol]
  | "non-assoc:" [klabel(nonAssocAttribute), symbol]
  syntax KSortList  ::=
    KSortList "," KSort [klabel(kSortList), symbol]
  syntax KProductionWAttr  ::=
    KProduction OptionalAttributes [klabel(kProductionWAttr), symbol]
  syntax KPrioritySeq  ::=
    KPrioritySeq ">" KNeTagSet [klabel(kPrioritySeq), symbol]
  syntax ProdBlock  ::=
    ProdBlock "|" KProductionWAttr [format(%1%n%2 %3), klabel(prodBlock), symbol]
  syntax PrioritySeqBlock  ::=
    PrioritySeqBlock ">" AssocAttribute ProdBlock [format(  %1%n%2 %3%4), klabel(prioritySeqBlock), symbol]
  syntax KProductionItem 
  syntax KProduction  ::=
    KProductionItem KProduction [klabel(kProduction), symbol, unit(emptyKProduction)]
  | Tag "(" KSortList ")" [klabel(kFuncProduction), symbol]
  | "(" KSortList ")" [klabel(kTupleProduction), symbol]
  syntax SyntaxDeclaration 
  syntax EKoreDeclaration 
endmodule

module K-PRODUCTION-SYNTAX 
  imports public K-PRODUCTION-COMMON
  imports public KORE-SYNTAX
  imports public ATTRIBUTES-SYNTAX
  syntax EKoreDeclaration  ::=
    SyntaxDeclaration [klabel(inj)]
  syntax SyntaxDeclaration  ::=
    "syntax" KSort OptionalAttributes [klabel(kSyntaxSortLbl), symbol]
  | "syntax" KSort "::=" PrioritySeqBlock [format(%1 %2 %3 %4), klabel(kSyntaxProductionLbl), symbol]
  | "syntax" "priority" KPrioritySeq OptionalAttributes [klabel(kSyntaxPriorityLbl), symbol]
  | "syntax" "priorities" KPrioritySeq OptionalAttributes [klabel(kSyntaxPrioritiesLbl), symbol]
  | "syntax" "left" KNeTagSet OptionalAttributes [klabel(kSyntaxLeftLbl), symbol]
  | "syntax" "right" KNeTagSet OptionalAttributes [klabel(kSyntaxRightLbl), symbol]
  | "syntax" "non-assoc" KNeTagSet OptionalAttributes [klabel(kSyntaxNonAssocLbl), symbol]
  syntax AssocAttribute  ::=
    "" [klabel(noAttribute)]
  syntax KProductionItem  ::=
    KSort [klabel(nonTerminalLbl)]
  | EKoreKString [klabel(terminalLbl)]
  | "r" EKoreKString [klabel(regexTerminalLbl)]
  | "NeList" "{" KSort "," EKoreKString "}" [klabel(neListProdLbl)]
  | "List" "{" KSort "," EKoreKString "}" [klabel(listProdLbl)]
  syntax KNeTagSet  ::=
    Tag [klabel(inj)]
  syntax KSortList  ::=
    KSort [klabel(inj)]
  syntax KPrioritySeq  ::=
    KNeTagSet [klabel(inj)]
  syntax ProdBlock  ::=
    KProductionWAttr [klabel(inj)]
  syntax PrioritySeqBlock  ::=
    ProdBlock [klabel(inj)]
  syntax KProduction  ::=
    KProductionItem [klabel(inj)]
endmodule

module K-PRODUCTION-ABSTRACT 
  imports public K-PRODUCTION-COMMON
  imports public KORE-ABSTRACT
  syntax EKoreDeclaration  ::=
    SyntaxDeclaration 
  syntax SyntaxDeclaration  ::=
    "kSyntaxSort" "(" KSort "," OptionalAttributes ")" [klabel(kSyntaxSortLbl), symbol]
  | "kSyntaxProduction" "(" KSort "," PrioritySeqBlock ")" [format(%1%2%3%4%5%6), klabel(kSyntaxProductionLbl), symbol]
  | "kSyntaxPriority" "(" KPrioritySeq "," OptionalAttributes ")" [klabel(kSyntaxPriorityLbl), symbol]
  | "kSyntaxPriorities" "(" KPrioritySeq "," OptionalAttributes ")" [klabel(kSyntaxPrioritiesLbl), symbol]
  | "kSyntaxLeft" "(" KNeTagSet "," OptionalAttributes ")" [klabel(kSyntaxLeftLbl), symbol]
  | "kSyntaxRight" "(" KNeTagSet "," OptionalAttributes ")" [klabel(kSyntaxRightLbl), symbol]
  | "kSyntaxNonAssoc" "(" KNeTagSet "," OptionalAttributes ")" [klabel(kSyntaxNonAssocLbl), symbol]
  syntax AssocAttribute  ::=
    "noAssoc" [klabel(noAttribute), symbol]
  syntax KProductionItem  ::=
    "nonTerminal" "(" KSort ")" [format(%3), klabel(nonTerminalLbl), symbol]
  | "terminal" "(" EKoreKString ")" [format(%3), klabel(terminalLbl), symbol]
  | "regexTerminal" "(" EKoreKString ")" [klabel(regexTerminalLbl), symbol]
  | "neListProd" "(" KSort "," EKoreKString ")" [klabel(neListProdLbl), symbol]
  | "listProd" "(" KSort "," EKoreKString ")" [klabel(listProdLbl), symbol]
  syntax KNeTagSet  ::=
    Tag 
  syntax KSortList  ::=
    KSort 
  syntax KPrioritySeq  ::=
    KNeTagSet 
  syntax ProdBlock  ::=
    KProductionWAttr 
  syntax PrioritySeqBlock  ::=
    ProdBlock 
  syntax KProduction  ::=
    KProductionItem 
endmodule

module CONFIG-RULE-CONTEXT-COMMON 
  imports public KORE-COMMON
  imports public EKORE-BASE
  imports public ATTRIBUTES-COMMON
  syntax RuleContents 
  syntax EKoreDeclaration 
endmodule

module CONFIG-RULE-CONTEXT-ABSTRACT 
  imports public CONFIG-RULE-CONTEXT-COMMON
  imports public RULES-WITH-BUBBLES-COMMON
  imports public KORE-ABSTRACT
  syntax RuleContents  ::=
    "noAttrsP" "(" Pattern ")" [format(%3), klabel(noAttrsP), symbol]
  | "attrsP" "(" Pattern "," KAttributesDeclaration ")" [klabel(attrsP), symbol]
  | "noAttrsB" "(" Bubble ")" [format(%3), klabel(noAttrsB), symbol]
  | "attrsB" "(" Bubble "," KAttributesDeclaration ")" [klabel(attrsB), symbol]
  syntax EKoreDeclaration  ::=
    "kConfiguration" "(" RuleContents ")" [klabel(kConfigurationLbl), symbol]
  | "kRule" "(" RuleContents ")" [klabel(kRuleLbl), symbol]
  | "kContext" "(" RuleContents ")" [klabel(kContextLbl), symbol]
endmodule

module CONFIG-RULE-CONTEXT-SYNTAX 
  imports public CONFIG-RULE-CONTEXT-COMMON
  imports public EKORE-KSTRING-SYNTAX
  imports public KORE-SYNTAX
  syntax RuleContents  ::=
    Pattern [klabel(noAttrs)]
  | Pattern KAttributesDeclaration [klabel(attrs), prefer]
  syntax EKoreDeclaration  ::=
    "configuration" RuleContents [klabel(kConfigurationLbl), symbol]
  | "rule" RuleContents [klabel(kRuleLbl), symbol]
  | "context" RuleContents [klabel(kContextLbl), symbol]
endmodule

module EKORE-KSTRING-COMMON 
  syntax EKoreKString 
endmodule

module EKORE-KSTRING-ABSTRACT 
  imports public EKORE-KSTRING-COMMON
endmodule

module EKORE-KSTRING-SYNTAX 
  imports public EKORE-KSTRING-COMMON
  syntax EKoreKString  ::=
    r"[\\\"](([^\\\"\n\r\\\\])|([\\\\][nrtf\\\"\\\\])|([\\\\][x][0-9a-fA-F]{2})|([\\\\][u][0-9a-fA-F]{4})|([\\\\][U][0-9a-fA-F]{8}))*[\\\"]" [token]
endmodule

module ATTRIBUTES-COMMON 
  imports public EKORE-KSTRING-COMMON
  imports public TOKENS
  syntax Attr 
  syntax AttrList 
  syntax KAttributesDeclaration  ::=
    "[" AttrList "]" [klabel(kAttributesDeclaration), symbol]
  syntax OptionalAttributes 
  syntax TagContent  ::=
    UpperName 
  | LowerName 
  | Numbers 
  | EKoreKString 
  syntax TagContents 
  syntax KEY  ::=
    LowerName 
endmodule

module ATTRIBUTES-ABSTRACT 
  imports public ATTRIBUTES-COMMON
  syntax Attr  ::=
    "tagSimple" "(" KEY ")" [format(%3), klabel(tagSimple), symbol]
  | KEY "(" TagContents ")" [klabel(tagContent), symbol]
  syntax AttrList  ::=
    Attr "," AttrList [format(%1 %2 %3), klabel(consAttrList), symbol]
  | ".AttrList" [klabel(dotAttrList), symbol]
  syntax OptionalAttributes  ::=
    KAttributesDeclaration 
  syntax OptionalAttributes  ::=
    "noAtt" [klabel(noKAttributesDeclaration), symbol]
  syntax TagContents  ::=
    ".tagContents" [format, klabel(dotTagContents), symbol]
  | TagContent TagContents [klabel(tagContents), symbol]
endmodule

module ATTRIBUTES-SYNTAX 
  imports public ATTRIBUTES-COMMON
  imports public TOKENS-SYNTAX
  syntax OptionalAttributes  ::=
    KAttributesDeclaration [klabel(inj)]
  syntax Attr  ::=
    KEY [klabel(tagSimple)]
  | KEY "(" TagContents ")" [klabel(tagContent)]
  syntax EmptyAttrList  ::=
    "" [klabel(dotAttrList)]
  syntax NeAttrList  ::=
    Attr "," NeAttrList [klabel(consAttrList)]
  | Attr EmptyAttrList [klabel(consAttrList)]
  syntax AttrList  ::=
    NeAttrList 
  | EmptyAttrList 
  syntax OptionalAttributes  ::=
    "" [klabel(noKAttributesDeclaration)]
  syntax TagContents  ::=
    "" [klabel(dotTagContents), token]
  | TagContent TagContents [klabel(tagContents), token]
endmodule

module REWRITES-REQUIRES-IN-PATTERNS-ABSTRACT 
  imports public KORE-ABSTRACT
  syntax Pattern  ::=
    "requiresClause" "(" Pattern "," Pattern ")" [klabel(requiresClause), symbol]
  > "krewrite" "(" Pattern "," Pattern ")" [format(%3 => %5), klabel(krewrite), non-assoc, symbol]
endmodule

module REWRITES-REQUIRES-IN-PATTERNS-SYNTAX 
  imports public KORE-SYNTAX
  syntax Pattern  ::=
    Pattern "requires" Pattern [klabel(requiresClause), symbol]
  > Pattern "=>" Pattern [klabel(krewrite), non-assoc, symbol]
endmodule

module BACKTICK-PATTERNS-ABSTRACT 
  imports public KORE-ABSTRACT
  imports public EKORE-KSTRING-ABSTRACT
  syntax Variable  ::=
    "cast" "(" VarName "," KSort ")" [klabel(cast), symbol]
  | VarName 
  syntax Pattern  ::=
    "ktoken" "(" EKoreKString "," EKoreKString ")" [klabel(ktoken), symbol]
  | "wrappedklabel" "(" KLabel2 ")" [klabel(wrappedklabel), symbol]
  > "ksequence" "(" Pattern "," Pattern ")" [klabel(ksequence), left, symbol]
  syntax KLabel2  ::=
    LowerName 
  syntax Symbol  ::=
    KLabel2 
  syntax VarName  ::=
    UpperName 
endmodule

module BACKTICK-PATTERNS-SYNTAX 
  imports public KORE-SYNTAX
  imports public EKORE-KSTRING-SYNTAX
  syntax Variable  ::=
    VarName 
  syntax Pattern  ::=
    "#token" "(" EKoreKString "," EKoreKString ")" [klabel(ktoken)]
  | "#klabel" "(" KLabel2 ")" [klabel(wrappedklabel)]
  | Pattern "requires" Pattern [klabel(requiresClause)]
  > Pattern "~>" Pattern [klabel(ksequence), left]
  syntax KLabel2  ::=
    LowerName 
  | BacktickName 
  syntax Symbol  ::=
    KLabel2 
  syntax VarName  ::=
    UpperName 
  | DollarName 
endmodule

module K-DEFINITION-COMMON 
  imports public TOKENS
  imports public EKORE-KSTRING-COMMON
  syntax KImport 
  syntax KImportList 
  syntax KRequire  ::=
    "kRequire" "(" EKoreKString ")" [klabel(kRequire), symbol]
  syntax KRequireList 
endmodule

module K-DEFINITION-ABSTRACT 
  imports public K-DEFINITION-COMMON
  imports public KORE-ABSTRACT
  imports public ATTRIBUTES-ABSTRACT
  syntax KImport  ::=
    "kImport" "(" ModuleName ")" [klabel(kImportLbl), symbol]
  syntax KImportList  ::=
    ".KImportList" [klabel(emptyKImportList), symbol]
  | KImportList KImport [format(%1%2%n), klabel(kImportList), symbol]
  syntax KRequireList  ::=
    ".KRequireList" [klabel(emptyKRequireList), symbol]
  | KRequireList KRequire [format(%1%2%n), klabel(KRequireList), symbol]
  syntax KDefinition  ::=
    "kDefinition" "(" KRequireList "," Modules ")" [format(%3%n%n%5), klabel(kDefinitionLbl), symbol]
  syntax Definition  ::=
    KDefinition 
  syntax Module  ::=
    "kModule" "(" KModuleName "," OptionalAttributes "," KImportList "," Declarations ")" [klabel(kModuleLbl), symbol]
endmodule

module K-DEFINITION-SYNTAX 
  imports public K-DEFINITION-COMMON
  imports public EKORE-KSTRING-SYNTAX
  imports public KORE-SYNTAX
  imports public ATTRIBUTES-SYNTAX
  syntax KImport  ::=
    "imports" ModuleName [klabel(kImportLbl), symbol]
  syntax KImportList  ::=
    "" [klabel(emptyKImportList)]
  | KImportList KImport [format(%1%2%n%3), klabel(kImportList)]
  syntax KRequireList  ::=
    "" [klabel(emptyKRequireList)]
  | KRequireList KRequire [format(%1%2%n%3), klabel(KRequireList)]
  syntax KDefinition  ::=
    KRequireList Modules [format(%1%n%n%2), klabel(kDefinitionLbl)]
  syntax Definition  ::=
    KDefinition [klabel(inj)]
  syntax Module  ::=
    "module" KModuleName OptionalAttributes KImportList Declarations "endmodule" [format(%1 %2 %3%i%n%4%n%5%n%d%6), klabel(kModuleLbl)]
endmodule

module FILE-UTIL 
  imports public K-IO
  imports public DOMAINS
  syntax K  ::=
    "saveToFile" "(" String "," String ")" [function, klabel(saveToFile)]
  syntax K  ::=
    "saveToFileHelper" "(" IOInt "," K ")" [function, klabel(saveToFileHelper)]
  rule saveToFile(Contents, Path) => saveToFileHelper(#open(Path, "w"), Contents) 
  rule saveToFileHelper(Fd:Int, Contents:String) => saveToFileHelper(Fd, #write(Fd, Contents)) 
  rule saveToFileHelper(Fd:Int, .K) => #close(Fd) 
  syntax IOString  ::=
    "saveToTempFile" "(" String "," String ")" [function, klabel(saveToTempFile)]
  | "saveToTempFileHelper1" "(" K "," IOFile ")" [function, klabel(saveToTempFileHelper1)]
  | "saveToTempFileHelper2" "(" K "," String ")" [function, klabel(saveToTempFileHelper2)]
  rule saveToTempFile(CONTENTS, FTEMPLATE)
    => saveToTempFileHelper1(CONTENTS, #mkstemp(FTEMPLATE)) 
  rule saveToTempFileHelper1(CONTENTS, #tempFile(FILENAME:String, Fd:Int))
    => saveToTempFileHelper2(saveToFileHelper(Fd, CONTENTS), FILENAME:String) 
  rule saveToTempFileHelper2(.K, FILENAME)
    => FILENAME 
  syntax IOString  ::=
    "readFile" "(" String ")" [function, klabel(readFile)]
  syntax IOString  ::=
    "readFileHelper" "(" IOInt "," K "," String ")" [function, klabel(readFileHelper)]
  rule readFile(S)                           => readFileHelper(#open(S, "r"), .K, "") 
  rule readFileHelper(Fd:Int, .K, Acc)       => readFileHelper(Fd, #read(Fd, 4096), Acc) 
  rule readFileHelper(Fd:Int, "", Acc)       => readFileHelper(#EBADF, #close(Fd), Acc) 
  rule readFileHelper(Fd:Int, S:String, Acc) => readFileHelper(Fd, .K, Acc +String S)  [owise]
  rule readFileHelper(#EBADF, .K, Acc)       => Acc 
  rule readFileHelper(ERR:IOError, .K, Acc)  => ERR  [owise]
endmodule

module PARSER-UTIL 
  imports public FILE-UTIL
  imports public K-REFLECTION
  imports public EKORE-ABSTRACT
  imports public KINK-CONFIGURATION
  syntax String  ::=
    "tokenToString" "(" K ")" [function, functional, hook(STRING.token2string), klabel(tokenToString)]
  syntax KItem  ::=
    "parseOuter" "(" String ")" [function, klabel(parseOuter)]
  rule [[ parseOuter(S)
       => doParseKAST(parseHelper( module = "OUTER-SYNTAX"
                               , grammarFile = DEPLOY_DIR +String "/src/ekore.k"
                               , start = "Definition"
                               , input = S
                               , output = "kore"
                   )           )
       ]]
       <kinkDeployedDir> DEPLOY_DIR </kinkDeployedDir> 
  syntax KItem  ::=
    "parseEKore" "(" String ")" [function, klabel(parseEKore)]
  rule [[ parseEKore(S)
       => doParseKAST(parseHelper( module = "EKORE-SYNTAX"
                             , grammarFile = DEPLOY_DIR +String "/src/ekore.k"
                             , start = "Definition"
                             , input = S
                             , output = "kore"
                 )           )
       ]]
       <kinkDeployedDir> DEPLOY_DIR </kinkDeployedDir> 
  syntax KItem  ::=
    "parseSymbolName" "(" String ")" [function, klabel(parseSymbolName)]
  rule [[ parseSymbolName(S)
       => doParseKAST(parseHelper( module = "EKORE-SYNTAX"
                                 , grammarFile = DEPLOY_DIR +String "/src/ekore.k"
                                 , start = "SymbolName"
                                 , input = S
                                 , output = "kore"
                     )           )
       ]]
       <kinkDeployedDir> DEPLOY_DIR </kinkDeployedDir> 
  syntax KItem  ::=
    "doParseKAST" "(" K ")" [function, klabel(doParseKAST)]
  rule doParseKAST(S:String) => #parseKORE(S):KDefinition 
  syntax KItem  ::=
    "parseHelper" "(" "module" "=" String "," "grammarFile" "=" IOString "," "start" "=" String "," "input" "=" KItem "," "output" "=" String ")" [function]
  | "parseHelper1" "(" "module" "=" String "," "grammarFile" "=" IOString "," "start" "=" String "," "inputFile" "=" IOString "," "output" "=" String ")" [function]
  | "parseHelper2" "(" KItem ")" [function]
  rule parseHelper(module = MOD, grammarFile = GRAMMAR, start = START, input = INPUT:String, output = OUTPUT)
    => parseHelper1( module = MOD, grammarFile = GRAMMAR
                   , start = START, inputFile = saveToTempFile(INPUT, "/tmp/parseHelperXXXXXX")
                   , output = OUTPUT
                   ) 
  rule parseHelper1(module = _, grammarFile = _, start = _, inputFile = E:IOError, output = _)
    => E 
  rule parseHelper1(module = MOD, grammarFile = GRAMMAR, start = START, inputFile = INPUTFILE, output = OUTPUT)
    => parseHelper2(#system("k-light2k5.sh --output " +String OUTPUT +String
                                         " --module " +String MOD +String
                                         " " +String GRAMMAR +String
                                         " " +String START +String
                                         " " +String INPUTFILE
                   )       ) 
  rule parseHelper2(#systemResult(0, STDOUT, _))
    => STDOUT 
  syntax KItem  ::=
    "parseWithProductions" "(" Set "," String "," String ")" [function, klabel(parseWithProductions)]
  rule [[ parseWithProductions(GRAMMAR, START, INPUT)
       => doParseKAST(parseHelper( module = "KORE-SYNTAX"
                                 , grammarFile = DEPLOY_DIR +String "/src/kore.k"
                                 , start = "Pattern"
                                 , input = parseHelper( module = "LANGUAGE-GRAMMAR"
                                                      , grammarFile = saveToTempFile("module LANGUAGE-GRAMMAR\n"
                                                                             +String grammarToString(GRAMMAR)
                                                                             +String "endmodule"
                                                                                    , "/tmp/parseKASTXXXXXXXX")
                                                      , start = START
                                                      , input = INPUT
                                                      , output = "kore"
                                                      )
                                 , output = "kore"
                     )           )
       ]]
       <kinkDeployedDir> DEPLOY_DIR </kinkDeployedDir> 
  syntax String  ::=
    "grammarToString" "(" Set ")" [function, klabel(grammarToString)]
  rule grammarToString(.Set) => "" 
  rule grammarToString(SetItem(kSyntaxProduction(S, kProductionWAttr(P, ATTRS))) DECLS)
    => "syntax " +String tokenToString(S) +String " ::= "
                 +String KProductionToString(P) +String " "
                 +String OptionalAttributesToString(ATTRS)
       +String "\n"
       +String grammarToString(DECLS) 
  syntax String  ::=
    "KSortListToString" "(" KSortList ")" [function, klabel(KSortListToString)]
  rule KSortListToString(S:KSort) => tokenToString(S) 
  rule KSortListToString(Ss, S) => KSortListToString(Ss) +String "," +String tokenToString(S) 
  syntax String  ::=
    "KProductionToString" "(" KProduction ")" [function, klabel(KProductionToString)]
  rule KProductionToString(PI:KProductionItem)
    => KProductionItemToString(PI) 
  rule KProductionToString(kProduction(PI, PIs))
    => KProductionItemToString(PI) +String " " +String KProductionToString(PIs) 
  rule KProductionToString(TAG:Tag(KSORTLIST:KSortList))
    => tokenToString(TAG) +String "(" +String KSortListToString(KSORTLIST) +String ")" 
  syntax String  ::=
    "KProductionItemToString" "(" KProductionItem ")" [function, klabel(KProductionItemToString)]
  rule KProductionItemToString(nonTerminal(N)) => tokenToString(N) 
  rule KProductionItemToString(terminal(T))    => tokenToString(T) 
  rule KProductionItemToString(regexTerminal(R)) => "r" +String tokenToString(R) 
  syntax String  ::=
    "OptionalAttributesToString" "(" OptionalAttributes ")" [function, klabel(OptionalAttributesToString)]
  rule OptionalAttributesToString(noAtt) => "" 
  rule OptionalAttributesToString([ ATTRLIST ])
    => "[" +String AttrListToString(ATTRLIST) +String "]" 
  syntax String  ::=
    "AttrListToString" "(" AttrList ")" [function, klabel(AttrListToString)]
  rule AttrListToString(.AttrList)       => "dummy" 
  rule AttrListToString(ATTR, .AttrList) => AttrToString(ATTR) 
  rule AttrListToString(ATTR, ATTRs)     => AttrToString(ATTR) +String "," +String AttrListToString(ATTRs)
    requires notBool ATTRs ==K .AttrList 
  syntax String  ::=
    "AttrToString" "(" Attr ")" [function, klabel(AttrToString)]
  rule AttrToString(tagSimple(KEY))
    => tokenToString(KEY) 
  rule AttrToString(KEY:KEY(CONTENTS:TagContents))
    => tokenToString(KEY) +String "(" +String tokenToString(CONTENTS) +String ")" 
  syntax String  ::=
    "TagContentsToString" "(" TagContents ")" [function, klabel(TagContentsToString)]
  rule TagContentsToString(tagContents(TC, TCs))
    => tokenToString(TC) +String " " +String TagContentsToString(TCs) 
  rule TagContentsToString(.tagContents)
    => "" 
endmodule

module DISAMBIGUATE 
  imports public KORE-ABSTRACT
  imports public KORE-HELPERS
  imports public K-PRODUCTION-ABSTRACT
  imports public SET
  imports public PARSER-UTIL
  syntax KItem  ::=
    "pair" "(" KItem "," KItem ")" [klabel(pair)]
  syntax Pattern  ::=
    "disambiguate" "(" Pattern "," Set ")" [function, klabel(disambiguate)]
  syntax Pattern  ::=
    "applyTypeCheck" "(" Pattern "," Map "," Set "," Set ")" [klabel(applyTypeCheck)]
  syntax Set  ::=
    "collectCasts" "(" Pattern ")" [function, klabel(collectCasts)]
  syntax Set  ::=
    "collectCastsPs" "(" Patterns ")" [function, klabel(collectCastsPs)]
  syntax Set  ::=
    "collectCasts2" "(" Pattern "," String ")" [function, klabel(collectCasts2)]
  rule collectCasts(SName { _ } ( Ps )) => collectCastsPs(Ps)
    requires findString(tokenToString(SName), "SemanticCastTo", 0) =/=Int 0 
  rule collectCasts(SName { _ } ( P, .Patterns )) => collectCasts2(P, substrString(tokenToString(SName), 14, lengthString(tokenToString(SName))))
    requires findString(tokenToString(SName), "SemanticCastTo", 0) ==Int 0 
  rule collectCasts2(\dv{_}(VarName), SortName) => SetItem(pair(VarName, SortName)) 
  rule collectCasts2(P, _) => collectCasts(P)  [owise]
  rule collectCastsPs(P, Ps) => collectCasts(P) collectCastsPs(Ps) 
  rule collectCastsPs(.Patterns) => .Set 
  syntax Set  ::=
    "getSubsorts" "(" Set ")" [function, klabel(getSubsorts)]
  rule getSubsorts(SetItem(S) Ss:Set) => getSubsorts2(S) getSubsorts(Ss) 
  rule getSubsorts(.Set) => .Set 
  syntax Set  ::=
    "getSubsorts2" "(" K ")" [function, klabel(getSubsorts2)]
  rule getSubsorts2(kSyntaxProduction(S1:KSort, kProductionWAttr(nonTerminal(S2:KSort), _))) => SetItem(pair(S1, S2)) 
  rule getSubsorts2(_) => .Set  [owise]
  syntax Set  ::=
    "tclosure" "(" Set ")" [function, klabel(tclosure)]
  rule tclosure(SetItem(pair(A, B)) SetItem(pair(B, C)) R:Set) 
    => tclosure(SetItem(pair(A, B)) SetItem(pair(B, C)) SetItem(pair(A, C)) R:Set)
    requires notBool( pair(A, C) in R) 
  rule tclosure(R) => R  [owise]
  syntax Map  ::=
    "collectSignature" "(" Map "," Set ")" [function, klabel(collectSignature)]
  syntax List  ::=
    "getSortArguments" "(" KProduction ")" [function, klabel(getSortArguments)]
  syntax String  ::=
    "getKLabelFromArgs" "(" K ")" [function, klabel(getKLabelFromArgs)]
  rule collectSignature(M => M [ getKLabelFromArgs(Args) <- ListItem(S1) getSortArguments(Pi) ], (SetItem(kSyntaxProduction(S1:KSort, kProductionWAttr(Pi, Args))) => .Set) S:Set ) 
  rule collectSignature(M, .Set) => M 
  rule getSortArguments(kProduction(P1, P2)) => getSortArguments(P1) getSortArguments(P2) 
  rule getSortArguments(nonTerminal(S2:KSort)) => ListItem(S2) 
  rule getSortArguments(terminal(_)) => .List 
  rule getSortArguments(regexTerminal(_)) => .List 
  rule getKLabelFromArgs(noAtt) => "" 
  rule getKLabelFromArgs(.AttrList) => "" 
  rule getKLabelFromArgs(kAttributesDeclaration(A) => A) 
  rule getKLabelFromArgs(consAttrList(tagContent(#token("klabel","LowerName"), KL), _)) => tokenToString(KL) 
  rule getKLabelFromArgs(consAttrList(_, A) => A)  [owise]
  syntax Bool  ::=
    "isSubsorted" "(" Sort "," Sort "," Set ")" [function, klabel(isSubsorted)]
  syntax Bool  ::=
    "isSubsortedEq" "(" Sort "," Sort "," Set ")" [function, klabel(isSubsortedEq)]
  rule isSubsorted(S1, S2, S) => pair(S1, S2) in S 
  rule isSubsortedEq(S1, S2, S) => S1 ==K S2 orBool isSubsorted(S1, S2, S) 
  rule disambiguate(P, Prods) => applyTypeCheck(P, collectSignature(.Map, Prods), tclosure(getSubsorts(Prods)), collectCasts(P)) 
  syntax Pattern  ::=
    "checkType" "(" Pattern "," SortName "," Map "," Set "," Set ")" [klabel(checkType)]
  syntax Patterns  ::=
    "checkType2" "(" Patterns "," List "," Map "," Set "," Set ")" [klabel(checkType2)]
  rule applyTypeCheck(P, Sigs, Subs, Types) => checkType(P, #token("K","UpperName"), Sigs, Subs, Types) 
  rule checkType (SName { _ } ( Ps ), UpSort, ((SName |-> ListItem(SSort) Sig:List) _:Map) #as Sigs, Subs, Types)
    => checkType2(Ps, Sig, Sigs, Subs, Types)
   requires isSubsortedEq(UpSort, SSort, Subs) 
  rule checkType2((P:Pattern, Ps:Patterns), ListItem(S) Sig, Sigs, Subs, Types)
    => checkType(P, S, Sigs, Subs, Types), checkType2(Ps, Sig, Sigs, Subs, Types) 
  rule checkType2(.Patterns, .List, _, _, _)
    => .Patterns 
endmodule

module PARSER-GEN-HELPERS 
  imports public SET
  imports public STRING-SYNTAX
  imports public KINK-CONFIGURATION
  imports public KORE-HELPERS
  imports public META-ACCESSORS
  syntax UpperName  ::=
    "String2UpperName" "(" String ")" [function, functional, hook(STRING.string2token), klabel(String2UpperName)]
  syntax Set  ::=
    "noCastSortsInit" [function]
  rule noCastSortsInit => // sorts from this list do not receive productions for casting
        SetItem(String2UpperName("Cell"))
        SetItem(String2UpperName("CellName"))
        SetItem(String2UpperName("CellProperties"))
        SetItem(String2UpperName("CellProperty"))
        SetItem(String2UpperName("KConfigVar"))
        SetItem(String2UpperName("KLabel"))
        SetItem(String2UpperName("KList"))
        SetItem(String2UpperName("KString"))
        SetItem(String2UpperName("KVariable"))
        SetItem(String2UpperName("Layout"))
        SetItem(String2UpperName("RuleBody"))
        SetItem(String2UpperName("RuleContent"))
        SetItem(String2UpperName("OptionalDots")) 
  syntax Set  ::=
    "noLatticeSortsInit" [function]
  rule noLatticeSortsInit => // sorts from this list are not included in the automatic subsorts lattice
        SetItem(String2UpperName("Cell"))
        SetItem(String2UpperName("CellName"))
        SetItem(String2UpperName("CellProperties"))
        SetItem(String2UpperName("CellProperty"))
        SetItem(String2UpperName("K"))
        SetItem(String2UpperName("KBott"))
        SetItem(String2UpperName("KConfigVar"))
        SetItem(String2UpperName("KLabel"))
        SetItem(String2UpperName("KList"))
        SetItem(String2UpperName("KString"))
        SetItem(String2UpperName("KVariable"))
        SetItem(String2UpperName("Layout"))
        SetItem(String2UpperName("RuleBody"))
        SetItem(String2UpperName("RuleContent"))
        SetItem(String2UpperName("OptionalDots")) 
  syntax String  ::=
    "token2String" "(" UpperName ")" [function, functional, hook(STRING.token2string), klabel(token2String)]
  syntax Set  ::=
    "#addCasts" "(" Set ")" [function]
  syntax Set  ::=
    "#addCasts2" "(" Set "," Set ")" [function]
  rule #addCasts(Prds) => #addCasts2(Prds, noCastSortsInit) 
  rule #addCasts2(
          SetItem(kSyntaxProduction(SORT, PROD))
          _:Set
          (.Set => SetItem(
            kSyntaxProduction(SORT,
                kProductionWAttr(kProduction(nonTerminal(SORT),
                                             terminal(String2EKoreKString("\":" +String token2String(SORT) +String "\""))),
                                 kAttributesDeclaration(consAttrList(
                                    tagContent(#token("klabel","LowerName"),
                                               String2TagContents("SemanticCastTo" +String token2String(SORT))),dotAttrList(.KList)))))))
          , SORTS (.Set => SetItem(SORT)))
     requires notBool SORT in SORTS 
  rule #addCasts2(Prds, _) => Prds  [owise]
  syntax Set  ::=
    "#addSubsorts" "(" Set ")" [function]
  syntax Set  ::=
    "#addSubsorts2" "(" Set "," Set ")" [function]
  rule #addSubsorts(Prds) => #addSubsorts2(Prds, noLatticeSortsInit) 
  rule #addSubsorts2(
          SetItem(kSyntaxProduction(SORT, PROD))
          _:Set
          (.Set => 
              SetItem(kSyntaxProduction(String2UpperName("K"), kProductionWAttr(nonTerminal(SORT), noAtt)))
              SetItem(kSyntaxProduction(SORT, kProductionWAttr(nonTerminal(String2UpperName("KBott")), noAtt))))
          , SORTS (.Set => SetItem(SORT)))
     requires notBool SORT in SORTS 
  rule #addSubsorts2(Prds, _) => Prds  [owise]
endmodule

module PARSE-OUTER 
  imports public KINK-CONFIGURATION
  imports public PARSER-UTIL
  syntax KItem  ::=
    "#parseOuter" 
  rule <k> PGM:Any ~> #parseOuter => parseOuter(tokenToString(PGM)) ... </k> 
endmodule

module PARSE-PROGRAM 
  imports public KINK-CONFIGURATION
  imports public K-PRODUCTION-ABSTRACT
  imports public EKORE-KSTRING-ABSTRACT
  imports public KORE-HELPERS
  imports public STRING
  imports public FILE-UTIL
  imports public PARSER-UTIL
  imports public META-ACCESSORS
  syntax KItem  ::=
    "#parseProgramPath" "(" String ")" 
  | "#parseProgram" "(" IOString ")" 
  | "#collectPgmGrammar" 
  rule <k> #parseProgramPath(PGM_FILENAME) => #parseProgram(readFile(PGM_FILENAME)) ... </k> 
  rule <k> #parseProgram(PGM)
        => parseWithProductions(GRAMMAR, "Pgm", PGM)
           ...
       </k>
       <prgGrammar> GRAMMAR </prgGrammar>
     requires GRAMMAR =/=K .Set 
  rule <k> #collectPgmGrammar ... </k>
       <name> MName </name>
       <prgGrammar> .Set => #getAllProds(MName) </prgGrammar>
    requires findString(tokenToString(MName), "-SYNTAX", 0) =/=Int -1 
  rule <k> #collectPgmGrammar => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module PARSE-CONFIG 
  imports public RULES-WITH-BUBBLES-COMMON
  imports public KINK-CONFIGURATION
  imports public K-PRODUCTION-ABSTRACT
  imports public EKORE-KSTRING-ABSTRACT
  imports public KORE-HELPERS
  imports public STRING
  imports public FILE-UTIL
  imports public PARSER-UTIL
  imports public KORE-ABSTRACT
  imports public META-ACCESSORS
  imports public PARSER-GEN-HELPERS
  syntax KItem  ::=
    "#parseConfigBubbles" 
  | "#createConfigGrammar" 
  rule <k> #createConfigGrammar ... </k> 
       <name> MName </name>
       <decl> kConfiguration(noAttrsB(_:Bubble)) </decl>
       <configGrammar> .Set => #addSubsorts(#addCasts(#getAllProds(MName) #getAllProds(#token("CONFIG-INNER", "UpperName")))) </configGrammar> 
  rule <k> #parseConfigBubbles ... </k>
       <decl> kConfiguration(noAttrsB(C:Bubble)) => kConfiguration(noAttrsP({parseWithProductions(GRAMMAR, "K", tokenToString(C))}:>Pattern)) </decl>
       <configGrammar> GRAMMAR </configGrammar>
     requires GRAMMAR =/=K .Set 
  rule <k> #parseConfigBubbles => .K ... </k>
       <s> #STUCK() => .K ... </s> 
  syntax KItem  ::=
    "#extractConfigInfo" 
  syntax KItem  ::=
    "collectCellName" "(" Patterns ")" [klabel(collectCellName)]
  rule <k> #extractConfigInfo => collectCellName(P, .Patterns) ~> #extractConfigInfo ... </k>
       <decl> kConfiguration(noAttrsP(P)) </decl>
       <collected> Configs => Configs SetItem(P) </collected>
     requires notBool P in Configs 
  rule <k> collectCellName( _ { _ } (A), .Patterns) => collectCellName(A) ... </k> 
  rule <k> collectCellName(A, B) => collectCellName(A, .Patterns) ~> collectCellName(B) ... </k>
    requires B =/=K .Patterns 
  rule <k> collectCellName( .Patterns ) => .K ... </k> 
  syntax String  ::=
    "token2String" "(" KoreString ")" [function, functional, hook(STRING.token2string), klabel(token2String)]
  rule <k> collectCellName(\dv { Srt { .Sorts } } ( CellName ), .Patterns) => .K ... </k>
       <cellName> .Map => substrString(token2String(CellName), 1, lengthString(token2String(CellName)) -Int 1) |-> token2String(Srt) ... </cellName> 
  rule <k> #extractConfigInfo => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module PARSE-RULE 
  imports public RULES-WITH-BUBBLES-COMMON
  imports public KINK-CONFIGURATION
  imports public K-PRODUCTION-ABSTRACT
  imports public EKORE-KSTRING-ABSTRACT
  imports public KORE-HELPERS
  imports public STRING
  imports public FILE-UTIL
  imports public PARSER-UTIL
  imports public KORE-ABSTRACT
  imports public META-ACCESSORS
  imports public PARSER-GEN-HELPERS
  imports public DISAMBIGUATE
  syntax KItem  ::=
    "#parseRuleBubbles" 
  | "#createRuleGrammar" 
  rule <k> #createRuleGrammar ... </k> // create rule grammar
       <name> MName </name>
       <decl> kRule(noAttrsB(_:Bubble)) </decl>
       <ruleGrammar> .Set => #addRuleCells(#addSubsorts(#addCasts(#getAllProds(MName) #getAllProds(#token("RULE-INNER", "UpperName"))))) </ruleGrammar> 
  rule <k> #createRuleGrammar ... </k> // create rule grammar for rules with attributes
       <name> MName </name>
       <decl> kRule(attrsB(_:Bubble, _)) </decl>
       <ruleGrammar> .Set => #addRuleCells(#addSubsorts(#addCasts(#getAllProds(MName) #getAllProds(#token("RULE-INNER", "UpperName"))))) </ruleGrammar> 
  rule <k> #createRuleGrammar => .K ... </k>
       <s> #STUCK() => .K ... </s> 
  rule <k> #parseRuleBubbles ... </k>
       <decl> kRule(noAttrsB(C:Bubble)) => kRule(noAttrsP(disambiguate({parseWithProductions(GRAMMAR, "RuleContent", tokenToString(C))}:>Pattern, GRAMMAR))) </decl>
       <ruleGrammar> GRAMMAR </ruleGrammar>
     requires GRAMMAR =/=K .Set 
  rule <k> #parseRuleBubbles ... </k>
       <decl> kRule(attrsB(C:Bubble, At)) => kRule(attrsP({parseWithProductions(GRAMMAR, "RuleContent", tokenToString(C))}:>Pattern, At)) </decl>
       <ruleGrammar> GRAMMAR </ruleGrammar>
     requires GRAMMAR =/=K .Set 
  rule <k> #parseRuleBubbles => .K ... </k>
       <s> #STUCK() => .K ... </s> 
  syntax Set  ::=
    "#addRuleCells" "(" Set ")" [function]
  syntax Set  ::=
    "#addRuleCells2" "(" Set "," Set ")" [function]
  rule #addRuleCells(Prds) => #addRuleCells2(Prds, .Set) 
  rule [[ #addRuleCells2(
        Prds:Set
        (.Set => SetItem(
          kSyntaxProduction(#token("Cell","UpperName"), 
              kProductionWAttr(kProduction(
                  terminal(String2EKoreKString("\"<" +String CellName +String ">\"")), kProduction(
                  nonTerminal(#token("OptionalDots","UpperName")), kProduction(
                  nonTerminal(#token("K","UpperName")), kProduction(
                  nonTerminal(#token("OptionalDots","UpperName")),
                  terminal(String2EKoreKString("\"</" +String CellName +String ">\"")))))),
                kAttributesDeclaration(consAttrList(
                   tagContent(#token("klabel","LowerName"), String2TagContents(CellName +String "cell")),consAttrList(
                   tagContent(#token("cellName","LowerName"), String2TagContents(CellName)),consAttrList(
                   tagSimple(#token("cell","LowerName")), dotAttrList(.KList)))))))))
        , Cells (.Set => SetItem(CellName)))
        ]]
       <cellName> CellName |-> "CellName" ... </cellName>
     requires notBool CellName in Cells 
  rule #addRuleCells2(Prods, _) => Prods  [owise]
endmodule

module PARSE-TO-EKORE 
  imports public EKORE-ABSTRACT
  imports public KINK-CONFIGURATION
  imports public PARSER-UTIL
  syntax KItem  ::=
    "#parseToEKore" 
  rule <k> PGM:Any ~> #parseToEKore => parseEKore(tokenToString(PGM)) ... </k> 
endmodule

module COMMAND-LINE-SYNTAX 
  imports public STRING-SYNTAX
  syntax KItem  ::=
    "#parseCommandLine" "(" CommandLine "," Pgm ")" 
  syntax Pgm 
  syntax Path  ::=
    r"[\\\\./a-zA-Z0-9_-][\\\\./a-zA-Z0-9_-]*" [token]
  syntax String  ::=
    "token2String" "(" Path ")" [function, functional, hook(STRING.token2string), klabel(token2String)]
  syntax CommandLine  ::=
    "kompile" 
  | "kast" Path 
  | "frontend-to-ekore" 
  | "ekore-to-kore" 
endmodule

module COMMAND-LINE 
  imports public COMMAND-LINE-SYNTAX
  imports public PARSE-OUTER
  imports public PARSE-PROGRAM
  imports public PARSE-CONFIG
  imports public PARSE-RULE
  imports public PARSE-TO-EKORE
  imports public FRONTEND-MODULES-TO-KORE-MODULES
  imports public FLATTEN-PRODUCTIONS
  imports public OUTER-ABSTRACT
  imports public NON-FUNCTIONAL-PRODUCTIONS-TO-CONSTRUCTORS
  imports public PRODUCTIONS-TO-SORT-DECLARATIONS
  imports public PRODUCTIONS-TO-SYMBOL-DECLARATIONS
  imports public TRANSLATE-FUNCTION-RULES
  imports public REMOVE-FRONTEND-DECLARATIONS
  imports public FILE-UTIL
  imports public INT
  rule <k> #parseCommandLine(kompile, PGM)
        => PGM ~> #ekorePipeline ~> #filterKoreDeclarations
           ...
       </k> 
  rule <k> #parseCommandLine(kast PATH, PGM)
        => PGM ~> #kastPipeline(token2String(PATH))
           ...
       </k> 
  rule <k> #parseCommandLine(frontend-to-ekore, PGM)
        => PGM ~> #frontendPipeline
           ...
       </k> 
  rule <k> #parseCommandLine(ekore-to-kore, PGM)
        => PGM ~> #ekorePipeline
           ...
       </k> 
  syntax KItem  ::=
    "#success" 
  rule <k> P:Pattern ~> (#success => .K) ... </k>
       <exit-code> _ => 0 </exit-code> 
  rule <k> #success => .K ... </k>
       <exit-code> _ => 0 </exit-code> 
  syntax String  ::=
    "tokenToString" "(" Any ")" [function, functional, hook(STRING.token2string), klabel(tokenToString)]
  syntax KItem  ::=
    "#kastPipeline" "(" String ")" 
  rule PGM:Any ~> #kastPipeline(PATH:String)
    => parseOuter(tokenToString(PGM))
    ~> #defnToConfig
    ~> #flattenProductions
    ~> #collectPgmGrammar
    ~> #parseProgramPath(PATH)
    ~> #success 
  syntax KItem  ::=
    "#ekorePipeline" 
  rule #ekorePipeline
    => #parseToEKore
    ~> #defnToConfig
    ~> #flattenProductions
    ~> #nonFunctionProductionsToConstructors
    ~> #productionsToSortDeclarations
    ~> #productionsToSymbolDeclarations
    ~> #translateRules
    ~> #success 
  syntax KItem  ::=
    "#frontendPipeline" 
  rule <k> PGM:Any ~> #frontendPipeline
    =>  parseOuter(
      //{readFile(DEPLOY_DIR +String "/src/inner.k")}:>String
      //+String
      tokenToString(PGM)
      ) // collect required files - hardcoded for now
    ~> #defnToConfig
    ~> #flattenProductions
    // parse bubbles
    ~> #createConfigGrammar
    ~> #parseConfigBubbles
    ~> #extractConfigInfo
    ~> #createRuleGrammar
    ~> #parseRuleBubbles
    ~> #success
    </k>
    <kinkDeployedDir> DEPLOY_DIR </kinkDeployedDir> 
endmodule

module KINK-SYNTAX 
  syntax Any  ::=
    r"([\\n\\r]|.)*" [token]
endmodule

module KINK-CONFIGURATION 
  imports public COMMAND-LINE-SYNTAX
  imports public EKORE-ABSTRACT
  imports public SET
  imports public STRING-SYNTAX
  imports public DEFAULT-STRATEGY
  syntax Any 
  syntax Pgm  ::=
    Any 
  syntax Declaration  ::=
    "nullDecl" 
  syntax DeclCellSet 
  syntax DeclarationsCellFragment 
  configuration <k> #parseCommandLine($COMMANDLINE:CommandLine, $PGM:Any) </k>
                <definition>
                   <defnAttrs format="[ %2 ]%n"> .Patterns </defnAttrs>
                   <modules format="%2%n">
                     <mod format="module %2%i%n%4%n%5%n%d%n %i%dendmodule %3%n%n"
                          multiplicity="*" type="Set">
                       <name format="%2"> #token("UNNAMED", "ModuleName"):ModuleName </name>
                       <attributes format="[ %2 ]"> .Patterns </attributes>
                       <declarations format="%2">
                         <decl format="%2%n" multiplicity="*" type="Set"> nullDecl </decl>
                       </declarations>
                       <parserGenerator format="/*%i%n%1%n%2%n%3%n%4%n%5%d%n*/">
                         <prgGrammar> .Set </prgGrammar> // place to collect the grammar used to parse programs
                         <configGrammar> .Set </configGrammar> // place to collect the grammar used to parse configurations
                         <ruleGrammar> .Set </ruleGrammar> // place to collect the grammar used to parse rules
                       </parserGenerator>
                     </mod>
                   </modules>
                   <configInfo format="/*%i%n%1%n%2%n%3%n%4%n%d%n*/">
                     <cellName> .Map </cellName>
                     <collected> .Set </collected>
                   </configInfo>
                </definition>
                <exit-code exit=""> 1 </exit-code>
                <s />
                <kinkDeployedDir> token2String($KINKDEPLOYEDDIR:Path) </kinkDeployedDir> 
endmodule

module KINK 
  imports public COMMAND-LINE
endmodule

module KORE-HELPERS 
  imports public KORE-ABSTRACT
  imports public K-EQUAL
  imports public STRING
  syntax Declarations  ::=
    Declarations "++Declarations" Declarations [function]
  rule (D1 DS1) ++Declarations DS2 => D1 (DS1 ++Declarations DS2) 
  rule .Declarations ++Declarations DS2 => DS2 
  syntax Modules  ::=
    Modules "++Modules" Modules [function]
  rule (M1 MS1) ++Modules MS2 => M1 (MS1 ++Modules MS2) 
  rule .Modules ++Modules MS2 => MS2 
  syntax Sorts  ::=
    Sorts "++Sorts" Sorts [function]
  rule (S1, SS1) ++Sorts SS2 => S1, (SS1 ++Sorts SS2) 
  rule .Sorts ++Sorts SS2 => SS2 
  syntax Bool  ::=
    Pattern "inPatterns" Patterns [function]
  rule (P inPatterns           .Patterns) => false 
  rule (P inPatterns P:Pattern  ,  PS)    => true 
  rule (P inPatterns P1:Pattern ,  PS)
    => (P inPatterns               PS)
    requires notBool P ==K P1 
  syntax EKoreKString  ::=
    "String2EKoreKString" "(" String ")" [function, functional, hook(STRING.string2token), klabel(String2EKoreKString)]
  syntax TagContents  ::=
    "String2TagContents" "(" String ")" [function, functional, hook(STRING.string2token), klabel(String2TagContents)]
endmodule

module META-ACCESSORS 
  imports public KINK-CONFIGURATION
  imports public KORE-HELPERS
  imports public BOOL
  imports public SET
  syntax Set  ::=
    "#getImportedModules" "(" ModuleName ")" [function, klabel(#getImportedModules)]
  syntax Set  ::=
    "#getImportedModulesSet" "(" ModuleName "," Set ")" [function, klabel(#getImportedModulesSet)]
  rule #getImportedModules(MNAME) => #getImportedModulesSet(MNAME, SetItem(MNAME)) 
  rule [[ #getImportedModulesSet(MNAME, MODS)
       => #getImportedModulesSet(MNAME, MODS SetItem(IMPORTED) #getImportedModules(IMPORTED))
       ]]
       <name> MNAME </name>
       <decl> koreImport(IMPORTED, _) </decl>
    requires notBool IMPORTED in MODS 
  rule #getImportedModulesSet(MNAME, MODS) => MODS  [owise]
  syntax Set  ::=
    "#getLocalProds" "(" ModuleName ")" [function, klabel(#getLocalProds)]
  syntax Set  ::=
    "#getLocalProdsSet" "(" ModuleName "," Set ")" [function, klabel(#getLocalProdsSet)]
  rule #getLocalProds(MNAME) => #getLocalProdsSet(MNAME, .Set) 
  rule [[ #getLocalProdsSet(MNAME, PRODS)
       => #getLocalProdsSet(MNAME, PRODS SetItem(PRD))
       ]]
       <name> MNAME </name>
       <decl> kSyntaxProduction(_, _) #as PRD </decl>
    requires notBool PRD in PRODS 
  rule #getLocalProdsSet(MNAME, PRODS) => PRODS  [owise]
  syntax Set  ::=
    "#getAllProds" "(" ModuleName ")" [function, klabel(#getAllProds)]
  syntax Set  ::=
    "#getAllProdsSet" "(" Set ")" [function, klabel(#getAllProdsSet)]
  rule #getAllProds(MName) => #getAllProdsSet(#getImportedModules(MName)) 
  rule #getAllProdsSet(SetItem(MName) Rest) => #getLocalProds(MName) #getAllProdsSet(Rest) 
  rule #getAllProdsSet(.Set) => .Set 
  syntax Bool  ::=
    "#isSortDeclaredMod" "(" ModuleName "," SortName ")" [function, klabel(#isSortDeclaredMod)]
  rule [[ #isSortDeclaredMod(MNAME:ModuleName, SORT:SortName) => true ]]
       <name> MNAME </name>
       <decl> sort SORT { PARAMS } ATTRS </decl> 
  rule #isSortDeclaredMod(_, _) => false  [owise]
  syntax Bool  ::=
    "#isSortDeclared" "(" ModuleName "," SortName ")" [function, klabel(#isSortDeclared)]
  syntax Bool  ::=
    "#isSortDeclaredSet" "(" Set "," SortName ")" [function, klabel(#isSortDeclaredSet)]
  rule #isSortDeclared(MNAME, SNAME)
    => #isSortDeclaredSet(#getImportedModules(MNAME), SNAME) 
  rule #isSortDeclaredSet(SetItem(M) Ms, SNAME)
    => #isSortDeclaredMod(M, SNAME) orBool #isSortDeclaredSet(Ms, SNAME) 
  rule #isSortDeclaredSet(.Set, SNAME) => false 
  syntax Bool  ::=
    "#isSymbolDeclaredMod" "(" ModuleName "," SymbolName ")" [function, klabel(#isSymbolDeclaredMod)]
  rule [[ #isSymbolDeclaredMod(MNAME, SYMBOL) => true ]]
       <name> MNAME </name>
       <decl> (symbol SYMBOL { _ } ( _ ) : _ ATTRS) </decl> 
  rule #isSymbolDeclaredMod(_, _) => false  [owise]
  syntax Bool  ::=
    "#isSymbolDeclared" "(" ModuleName "," SymbolName ")" [function, klabel(#isSymbolDeclared)]
  syntax Bool  ::=
    "#isSymbolDeclaredSet" "(" Set "," SymbolName ")" [function, klabel(#isSymbolDeclaredSet)]
  rule #isSymbolDeclared(MNAME, SNAME)
    => #isSymbolDeclaredSet(#getImportedModules(MNAME), SNAME) 
  rule #isSymbolDeclaredSet(SetItem(M) Ms, SNAME)
    => #isSymbolDeclaredMod(M, SNAME) orBool #isSymbolDeclaredSet(Ms, SNAME) 
  rule #isSymbolDeclaredSet(.Set, SNAME) => false 
  syntax Set  ::=
    "#getDeclaredKoreSymbolsFromDecls" "(" Declarations ")" [function, klabel(#getDeclaredKoreSymbolsFromDecls)]
  rule #getDeclaredKoreSymbolsFromDecls
           ( (symbol SYMBOL_NAME { SORT_PARAM } ( SORT_ARGS ) : SORT ATTRS):Declaration
             DECLS
           )
    => SetItem(SYMBOL_NAME) #getDeclaredKoreSymbolsFromDecls(DECLS) 
  rule #getDeclaredKoreSymbolsFromDecls(.Declarations)
    => .Set 
  rule #getDeclaredKoreSymbolsFromDecls(DECL DECLS)
    => #getDeclaredKoreSymbolsFromDecls(DECLS)
        [owise]
  syntax Sort  ::=
    "#getReturnSort" "(" ModuleName "," SymbolName ")" [function, klabel(#getReturnSort), withConfig]
  rule [[ #getReturnSort(MNAME, SNAME) => SORT ]]
       <name> MNAME </name>
       <decl> (symbol SNAME { .Sorts } ( _ ) : SORT ATTRS) </decl> 
  syntax Bool  ::=
    "#isFunctionSymbol" "(" ModuleName "," SymbolName ")" [function, klabel(#isFunctionSymbol), withConfig]
  rule [[ #isFunctionSymbol(MNAME, SNAME) => true ]]
       <name> MNAME </name>
       <decl> symbol SNAME { .Sorts } ( _ ) : SORT [ function { .Sorts } ( .Patterns ), ATTRS ]
       </decl> 
  rule [[ #isFunctionSymbol(MNAME, SNAME) => false ]]
       <name> MNAME </name>
       <decl> symbol SNAME { .Sorts } ( _ ) : SORT [ ATTRS ]
       </decl>
        [owise]
  syntax Bool  ::=
    "#keyInAttributes" "(" KEY "," AttrList ")" [function, klabel(#keyInAttributes)]
  rule #keyInAttributes(_, .AttrList) => false 
  rule #keyInAttributes(KEY, (tagSimple(KEY), _)) => true 
  rule #keyInAttributes(KEY, (tagContent(KEY, _), _)) => true 
  rule #keyInAttributes(KEY, (_ , REST))
    => #keyInAttributes(KEY, REST)  [owise]
  syntax TagContents  ::=
    "#getAttributeContent" "(" KEY "," AttrList ")" [function, klabel(#getAttributeContent)]
  rule #getAttributeContent(KEY, (tagContent(KEY, CONTENT), _)) => CONTENT 
  rule #getAttributeContent(KEY, (_ , REST))
    => #getAttributeContent(KEY, REST)  [owise]
endmodule

module FRONTEND-MODULES-TO-KORE-MODULES 
  imports public KINK-CONFIGURATION
  imports public KORE-HELPERS
  imports public STRING-SYNTAX
  syntax KItem  ::=
    "#defnToConfig" 
  rule <k> (PGM:Definition ~> #defnToConfig)
          => (#defnToConfig ~> PGM)
             ...
       </k> 
  rule <k> #defnToConfig
        ~> ( kDefinition(.KRequireList, MODS)
          => koreDefinition([.Patterns], MODS)
           )
           ...
       </k> 
  rule <k> #defnToConfig
        ~> koreDefinition( _
                         , (kModule(MNAME, ATTS, IMPORTS kImport(I), DECLS)
                         => kModule(MNAME, ATTS, IMPORTS, koreImport(I, [.Patterns]) DECLS))
                            MODS
                         )
           ...
       </k> 
  rule <k> #defnToConfig
        ~> koreDefinition(_
                         , ( kModule(MNAME, noAtt, .KImportList, DECLS)
                          => koreModule(MNAME, DECLS, [.Patterns])
                           )
                           MODS
                         )
           ...
       </k> 
  rule <k> #defnToConfig
        ~> ( koreDefinition(DEFATTRS, (koreModule(MNAME, DECLS, [ATTS]) MODS):Modules)
          => koreModule(MNAME, DECLS, [ATTS]) ~> koreDefinition(DEFATTRS, MODS)
           )
           ...
       </k>
       <modules>
         (  .Bag
         => <mod>
              <name> MNAME </name>
              <attributes> ATTS </attributes>
              ...
            </mod>
         )
         ...
       </modules> 
  rule <k> #defnToConfig ~> koreDefinition([.Patterns], .Modules)
        => .K
           ...
       </k> 
  rule <k> #defnToConfig ~> koreModule(MNAME, DECL DECLS:Declarations => DECLS, _) ... </k>
       <name> MNAME </name>
       <declarations> .Bag => <decl> DECL </decl> ... </declarations> 
  rule <k> #defnToConfig ~> (koreModule(MNAME, .Declarations, _) => .K)... </k> 
endmodule

module FLATTEN-PRODUCTIONS 
  imports public KINK-CONFIGURATION
  syntax KItem  ::=
    "#flattenProductions" 
  rule <k> #flattenProductions ... </k>
       <mod>
       <declarations>
          <decl> kSyntaxProduction(SORT, P1 > _:AssocAttribute P2) </decl>
       => <decl> kSyntaxProduction(SORT, P1) </decl>
          <decl> kSyntaxProduction(SORT, P2) </decl>
          ...
       </declarations>
       ...
       </mod> 
  rule <k> #flattenProductions ... </k>
       <mod>
       <declarations>
          <decl> kSyntaxProduction(SORT, P1 | P2) </decl>
       => <decl> kSyntaxProduction(SORT, P1) </decl>
          <decl> kSyntaxProduction(SORT, P2) </decl>
          ...
       </declarations>
       ...
       </mod> 
  rule <k> #flattenProductions => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module PRODUCTIONS-TO-SORT-DECLARATIONS 
  imports public META-ACCESSORS
  syntax KItem  ::=
    "#productionsToSortDeclarations" 
  rule <k>  #productionsToSortDeclarations ... </k>
       <name> MNAME </name>
       <declarations>
         ( .Bag =>
           <decl> sort SORT { .Sorts } [ .Patterns ] </decl>
         )
         <decl> kSyntaxProduction(SORT, _) </decl>
         ...
       </declarations>
    requires notBool(#isSortDeclared(MNAME, SORT)) 
  rule <k> #productionsToSortDeclarations => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module NON-FUNCTIONAL-PRODUCTIONS-TO-CONSTRUCTORS 
  imports public META-ACCESSORS
  syntax KItem  ::=
    "#nonFunctionProductionsToConstructors" 
  rule <k> #nonFunctionProductionsToConstructors ... </k>
       <name> MNAME </name>
       <declarations>
         <decl> kSyntaxProduction(SORT, kProductionWAttr(PROD, [ ATTRS
                                                              => (tagSimple(functional)
                                                                 , tagSimple(constructor)
                                                                 , tagSimple(injective)
                                                                 , ATTRS
                                                                 )
                                                               ]
                                                        ))
         </decl>
         ...
       </declarations>
    requires notBool #keyInAttributes(constructor, ATTRS)
     andBool notBool #keyInAttributes(function, ATTRS) 
  rule <k> #nonFunctionProductionsToConstructors => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module PRODUCTIONS-TO-SYMBOL-DECLARATIONS 
  imports public META-ACCESSORS
  imports public STRING
  imports public ID
  imports public PARSER-UTIL
  syntax KItem  ::=
    "#productionsToSymbolDeclarations" 
  rule <k>  #productionsToSymbolDeclarations ... </k>
       <name> MNAME </name>
       <declarations>
         <decl> kSyntaxProduction(SORT, kProductionWAttr(PROD, [ ATTRS ])) </decl>
         ( .Bag
        => <decl> symbol #symbolNameFromAttrList(ATTRS)
                    { .Sorts } (#symbolArgumentsFromProduction(PROD)) : SORT { .Sorts }
                    [ #removeKlabelAttr(ATTRS) ]
           </decl>
         )
         ...
       </declarations>
    requires notBool #isSymbolDeclared(MNAME, #symbolNameFromAttrList(ATTRS)) 
  rule <k> #productionsToSymbolDeclarations => .K ... </k>
       <s> #STUCK() => .K ... </s> 
  syntax SymbolName  ::=
    "#symbolNameFromAttrList" "(" AttrList ")" [function, klabel(#symbolNameFromAttrList)]
  rule #symbolNameFromAttrList(ATTRS)
    => {parseSymbolName(tokenToString(#getAttributeContent(klabel, ATTRS)))}:>SymbolName 
  syntax Patterns  ::=
    "#removeKlabelAttr" "(" AttrList ")" [function, klabel(#removeKlabelAttr)]
  rule #removeKlabelAttr(consAttrList(tagContent(klabel, _), ATTRS))
    => #attrList2Patterns(ATTRS) 
  rule #removeKlabelAttr(consAttrList(ATTRS, ATTR))
    => #attr2Pattern(ATTRS), #removeKlabelAttr(ATTR)  [owise]
  rule #removeKlabelAttr(.AttrList) => .Patterns 
  syntax Pattern  ::=
    "#attr2Pattern" "(" Attr ")" [function, klabel(#attr2Pattern)]
  rule #attr2Pattern(tagSimple(KEY:LowerName))
    => KEY { .Sorts } ( .Patterns ) 
  syntax Patterns  ::=
    "#attrList2Patterns" "(" AttrList ")" [function, klabel(#attrList2Patterns)]
  rule #attrList2Patterns(ATTR, ATTRS) => #attr2Pattern(ATTR), #attrList2Patterns(ATTRS) 
  rule #attrList2Patterns(.AttrList) => .Patterns 
  syntax Sorts  ::=
    "#symbolArgumentsFromProduction" "(" KProduction ")" [function, klabel(#symbolArgumentsFromProduction)]
  rule #symbolArgumentsFromProduction(PRODITEM:KProductionItem)
    => #sortsFromProdItem(PRODITEM) 
  rule #symbolArgumentsFromProduction(kProduction(PRODITEM, PROD))
    => #sortsFromProdItem(PRODITEM) ++Sorts #symbolArgumentsFromProduction(PROD) 
  syntax Sorts  ::=
    "#sortsFromProdItem" "(" KProductionItem ")" [function, klabel(#sortsFromProdItem)]
  rule #sortsFromProdItem(nonTerminal(KSORT:UpperName))
    => KSORT { .Sorts } , .Sorts 
  rule #sortsFromProdItem(_) => .Sorts  [owise]
endmodule

module TRANSLATE-FUNCTION-RULES 
  imports public KINK-CONFIGURATION
  imports public META-ACCESSORS
  syntax KItem  ::=
    "#translateRules" 
  rule <k> #translateRules ... </k>
       <name> MNAME </name>
       <decl> kRule(noAttrsP(krewrite( SYMBOL { .Sorts } ( ARG_PATTERNS ) #as LHS, RHS)))
           => axiom { #token("R", "UpperName") , .Sorts }
                \equals { #getReturnSort(MNAME, SYMBOL), #token("R", "UpperName") }
                ( LHS , RHS )
         [ .Patterns ]
       </decl>
    requires #isFunctionSymbol(MNAME, SYMBOL) 
  rule <k> #translateRules ... </k>
       <name> MNAME </name>
       <decl> kRule(noAttrsP(krewrite( SYMBOL { .Sorts } ( ARG_PATTERNS ) #as LHS , RHS)))
           => #fun( RETSORT
                 => axiom { .Sorts } \rewrites { RETSORT }
                        ( \and { RETSORT } (\top{ RETSORT }(), LHS)
                        , \and { RETSORT } (\top{ RETSORT }(), RHS)
                        )
                    [ .Patterns ]
                  ) (#getReturnSort(MNAME, SYMBOL))
       </decl>
    requires notBool #isFunctionSymbol(MNAME, SYMBOL) 
  rule <k> #translateRules => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule

module REMOVE-FRONTEND-DECLARATIONS 
  imports public KINK-CONFIGURATION
  imports public BOOL
  syntax KItem  ::=
    "#filterKoreDeclarations" 
  rule <k> #filterKoreDeclarations ... </k>
       <declarations> ( <decl> DECL </decl> => .Bag ) ... </declarations>
    requires notBool isKoreDeclaration(DECL) 
  rule <k> #filterKoreDeclarations => .K ... </k>
       <s> #STUCK() => .K ... </s> 
endmodule


