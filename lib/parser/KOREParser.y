%skeleton "lalr1.cc"
%define api.namespace {kllvm::parser}
%define parser_class_name {KOREParser}
%define api.value.type variant
%define parse.assert
%define parse.trace
%locations
%parse-param {KOREScanner &scanner}
%parse-param {KOREParserDriver &driver}
%parse-param {ptr<KOREDefinition> *definition}

%code requires {

#include "kllvm/ast/AST.h"

namespace kllvm {
namespace parser {
class KOREScanner;
class KOREParserDriver;
} // end namespace parser
} // end namespace kllvm

#define YYDEBUG 0

}

%code {

#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"

#undef yylex
#define yylex scanner.yylex

}

/* tokens - ternimal symbols */
%token MODULE                 "module"
%token ENDMODULE              "endmodule"
%token IMPORT                 "import"
%token SORT                   "sort"
%token SYMBOL                 "symbol"
%token ALIAS                  "alias"
%token WHERE                  "where"
%token AXIOM                  "axiom"
%token HOOKEDSORT             "hooked-sort"
%token HOOKEDSYMBOL           "hooked-symbol"
%token CLNEQ                  ":="
%token CLN                    ":"
%token LCB                    "{"
%token RCB                    "}"
%token LBR                    "["
%token RBR                    "]"
%token LP                     "("
%token RP                     ")"
%token COMMA                  ","
%token <char> CHAR            "kore-char"
%token <std::string> STRING   "kore-string"
%token <std::string> OBJECTID "object-id"
%token <std::string> METAID   "meta-id"

/* non-terminal symbols */
%start definition

%type <ptr<KOREModule>>  module
%type <std::string>   module-name

%type<ptr<KOREDeclaration>>                    declaration
%type<ptr<KOREModuleImportDeclaration>>        import-declaration
%type<ptr<KOREObjectCompositeSortDeclaration>> sort-declaration
%type<ptr<KORESymbolDeclaration>>              symbol-declaration
%type<ptr<KOREObjectSymbolDeclaration>>        object-symbol-declaration
%type<ptr<KOREMetaSymbolDeclaration>>          meta-symbol-declaration
%type<ptr<KORESymbolDeclaration>>              alias-declaration
%type<ptr<KOREObjectAliasDeclaration>>         object-alias-declaration
%type<ptr<KOREMetaAliasDeclaration>>           meta-alias-declaration
%type<ptr<KOREAxiomDeclaration>>               axiom-declaration

%type <ptr<KOREPattern>>               pattern
%type <ptr<KOREObjectPattern>>         object-pattern
%type <ptr<KOREMetaPattern>>           meta-pattern
%type <ptr<KOREObjectVariablePattern>> object-variable
%type <ptr<KOREMetaVariablePattern>>   meta-variable

%type <ptr<KOREObjectSort>>          object-sort
%type <ptr<KOREMetaSort>>            meta-sort
%type <ptr<KOREObjectSortVariable>>  object-sort-variable
%type <ptr<KOREMetaSortVariable>>    meta-sort-variable
%type <ptr<KOREObjectCompositeSort>> object-non-variable-sort
%type <ptr<KOREMetaCompositeSort>>   meta-non-variable-sort

/* associativity and priority definition */

%%

/* Sorts */
object-sort
  : object-sort-variable     { $$ = std::move($1); }
  | object-non-variable-sort { $$ = std::move($1); }
  ;

object-sort-variable
  : "object-id" { $$ = KOREObjectSortVariable::Create($1); }
  ;

object-non-variable-sort
  : "object-id"              { driver.startObjectSort($1);     }
    "{" object-sort-list "}" { $$ = driver.finishObjectSort(); }
    ;

object-sort-list
  : /* empty */
  | object-sort-list-non-empty
  ;

object-sort-list-non-empty
  : object-sort                                { driver.addObjectSort(std::move($1)); }
  | object-sort-list-non-empty "," object-sort { driver.addObjectSort(std::move($3)); }
  ;

meta-sort
  : meta-sort-variable     { $$ = std::move($1); }
  | meta-non-variable-sort { $$ = std::move($1); }
  ;

meta-sort-variable
  : "meta-id" { $$ = KOREMetaSortVariable::Create($1); }
  ;

meta-non-variable-sort
  : "meta-id" "{" "}" { $$ = KOREMetaCompositeSort::Create($1); }
  ;

meta-sort-list
  : /* empty */
  | meta-sort-list-non-empty
  ;

meta-sort-list-non-empty
  : meta-sort                              { driver.addMetaSort(std::move($1)); }
  | meta-sort-list-non-empty "," meta-sort { driver.addMetaSort(std::move($3)); }
  ;


/* Patterns */
pattern
  : object-pattern { $$ = std::move($1); }
  | meta-pattern   { $$ = std::move($1); }
  ;

object-pattern
  : object-variable { $$ = std::move($1); }
  | "object-id"
    { driver.startObjectPattern($1);     }
    "{" object-sort-list "}" "(" pattern-list ")"
    { $$ = driver.finishObjectPattern(); }
  ;

object-variable
  : "object-id" ":" object-sort
    { $$ = KOREObjectVariablePattern::Create($1, std::move($3)); }
  ;

meta-pattern
  : meta-variable { $$ = std::move($1); }
  | "kore-string" { $$ = KOREMetaStringPattern::Create($1); }
  | "kore-char"   { $$ = KOREMetaCharPattern::Create($1);   }
  | "meta-id"
    { driver.startMetaPattern($1);     }
    "{" meta-sort-list "}" "(" pattern-list ")"
    { $$ = driver.finishMetaPattern(); }
  ;

meta-variable
  : "meta-id" ":" meta-sort
    { $$ = KOREMetaVariablePattern::Create($1, std::move($3)); }
  ;

pattern-list
  : /* empty */
  | pattern-list-non-empty
  ;

pattern-list-non-empty
  : pattern                            { driver.addPattern(std::move($1)); }
  | pattern-list-non-empty "," pattern { driver.addPattern(std::move($3)); }
  ;


/* Attributes */
attribute
  : "[" pattern-list "]"
  ;


/* Modules and Declarations */
module
  : "module" module-name
    { driver.startModule($2); }
    declaration-list "endmodule" attribute
    { $$ = driver.finishModule(); }
  ;

declaration
  : import-declaration { $$ = std::move($1); }
  | sort-declaration   { $$ = std::move($1); }
  | symbol-declaration { $$ = std::move($1); }
  | alias-declaration  { $$ = std::move($1); }
  | axiom-declaration  { $$ = std::move($1); }
  ;

import-declaration
  : "import" module-name
    { driver.startModuleImportDeclaration($2);     }
    attribute
    { $$ = driver.finishModuleImportDeclaration(); }
  ;

sort-declaration
  : "sort" "object-id"
    { driver.startObjectSortDeclaration($2);       }
    "{" object-sort-variable-list "}" attribute
    { $$ = driver.finishObjectSortDeclaration();   }
  | "hooked-sort" "object-id"
    { driver.startObjectSortDeclaration($2, true); }
    "{" object-sort-variable-list "}" attribute
    { $$ = driver.finishObjectSortDeclaration();   }
  ;

symbol-declaration
  : object-symbol-declaration { $$ = std::move($1); }
  | meta-symbol-declaration   { $$ = std::move($1); }
  ;

object-symbol-declaration
  : "symbol" "object-id"
    { driver.startObjectSymbolDeclaration($2);       }
    "{" object-sort-variable-list "}" "(" object-sort-list ")"
    ":" object-sort attribute
    { $$ = driver.finishObjectSymbolDeclaration(std::move($11)); }
  | "hooked-symbol" "object-id"
    { driver.startObjectSymbolDeclaration($2, true);  }
    "{" object-sort-variable-list "}" "(" object-sort-list ")"
    ":" object-sort attribute
    { $$ = driver.finishObjectSymbolDeclaration(std::move($11)); }
  ;

meta-symbol-declaration
  : "symbol" "meta-id"
    { driver.startMetaSymbolDeclaration($2);        }
    "{" meta-sort-variable-list "}" "(" meta-sort-list ")"
    ":" meta-sort attribute
    { $$ = driver.finishMetaSymbolDeclaration(std::move($11)); }
  ;

alias-declaration
  : object-alias-declaration { $$ = std::move($1); }
  | meta-alias-declaration   { $$ = std::move($1); }
  ;

object-alias-declaration
  : "alias" "object-id"
    { driver.startObjectAliasDeclaration($2);             }
    "{" object-sort-variable-list "}" "(" object-sort-list ")" ":" object-sort
    "where" "object-id" "{" object-sort-variable-list "}"
    "(" object-variable-list ")" ":=" object-pattern attribute
    { $$ = driver.finishObjectAliasDeclaration(std::move($11), std::move($21)); }
  ;

meta-alias-declaration
  : "alias" "meta-id"
    { driver.startMetaAliasDeclaration($2);             }
    "{" meta-sort-variable-list "}" "(" meta-sort-list ")" ":" meta-sort
    "where" "meta-id" "{" meta-sort-variable-list "}"
    "(" meta-variable-list ")" ":=" meta-pattern attribute
    { $$ = driver.finishMetaAliasDeclaration(std::move($11), std::move($21)); }
  ;

axiom-declaration
  : "axiom"
    { driver.startAxiomDeclaration();       }
    "{" sort-variable-list "}" pattern attribute
    { $$ = driver.finishAxiomDeclaration(std::move($6)); }
  ;

sort-variable-list
  : /* empty */
  | sort-variable-list-non-empty
  ;

sort-variable-list-non-empty
  : object-sort-variable { driver.addObjectSortVariable(std::move($1)); }
  | meta-sort-variable   { driver.addMetaSortVariable(std::move($1));   }
  | sort-variable-list-non-empty "," object-sort-variable
    { driver.addObjectSortVariable(std::move($3)); }
  | sort-variable-list-non-empty "," meta-sort-variable
    { driver.addMetaSortVariable(std::move($3)); }
  ;

object-sort-variable-list
  : /* empty */
  | object-sort-variable-list-non-empty
  ;

object-sort-variable-list-non-empty
  : object-sort-variable { driver.addObjectSortVariable(std::move($1)); }
  | object-sort-variable-list-non-empty "," object-sort-variable
    { driver.addObjectSortVariable(std::move($3)); }
  ;

meta-sort-variable-list
  : /* empty */
  | meta-sort-variable-list-non-empty
  ;

meta-sort-variable-list-non-empty
  : meta-sort-variable { driver.addMetaSortVariable(std::move($1)); }
  | meta-sort-variable-list-non-empty "," meta-sort-variable
    { driver.addMetaSortVariable(std::move($3)); }
  ;

object-variable-list
  : /* empty */
  | object-variable-list-non-empty
  ;

object-variable-list-non-empty
  : object-variable { driver.addObjectVariable(std::move($1)); }
  | object-variable-list-non-empty "," object-variable
    { driver.addObjectVariable(std::move($3)); }
  ;

meta-variable-list
  : /* empty */
  | meta-variable-list-non-empty
  ;

meta-variable-list-non-empty
  : meta-variable { driver.addMetaVariable(std::move($1)); }
  | meta-variable-list-non-empty "," meta-variable
    { driver.addMetaVariable(std::move($3)); }
  ;

declaration-list
  : /* empty */
  | declaration-list declaration { driver.addDeclaration(std::move($2)); }
  ;

module-name
  : "object-id" { $$ = $1; }
  ;


/* Definitions */
definition
  :                                 { driver.startDefinition();                }
    attribute module-list-non-empty { *definition = driver.finishDefinition(); }
  ;

module-list-non-empty
  : module                       { driver.addModule(std::move($1)); }
  | module-list-non-empty module { driver.addModule(std::move($2)); }
  ;

%%

void kllvm::parser::KOREParser::error(
      const location_type &loc, const std::string &err_message) {
  std::cerr << "Syntax error at " << loc << ": " << err_message << "\n";
  exit(-1);
}
