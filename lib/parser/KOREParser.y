%skeleton "lalr1.cc"
%define api.namespace {kllvm::parser}
%define parser_class_name {KOREParser}
%define api.value.type variant
%define parse.assert
%define parse.trace
%locations
%parse-param {KOREScanner &scanner}
%parse-param {KOREParserDriver &driver}
%parse-param {KOREDefinition **definition}

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

/* non-terminal symbols */
%start definition

%type <KOREModule *>  module
%type <std::string>   module-name

%type<KOREDeclaration *>                    declaration
%type<KOREModuleImportDeclaration *>        import-declaration
%type<KORECompositeSortDeclaration *> sort-declaration
%type<KORESymbolOrAliasDeclaration *>              symbol-declaration
%type<KORESymbolDeclaration *>        object-symbol-declaration
%type<KORESymbolOrAliasDeclaration *>              alias-declaration
%type<KOREAliasDeclaration *>         object-alias-declaration
%type<KOREAxiomDeclaration *>               axiom-declaration

%type <KOREPattern *>               pattern
%type <KOREPattern *>         object-pattern
%type <KOREVariablePattern *> object-variable

%type <KORESort *>          object-sort
%type <KORESortVariable *>  object-sort-variable
%type <KORECompositeSort *> object-non-variable-sort

/* associativity and priority definition */

%%

/* Sorts */
object-sort
  : object-sort-variable     { $$ = $1; }
  | object-non-variable-sort { $$ = $1; }
  ;

object-sort-variable
  : "object-id" { $$ = KORESortVariable::Create($1); }
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
  : object-sort                                { driver.addObjectSort($1); }
  | object-sort-list-non-empty "," object-sort { driver.addObjectSort($3); }
  ;

/* Patterns */
pattern
  : object-pattern { $$ = $1; }
  ;

object-pattern
  : object-variable { $$ = $1; }
  | "kore-string" { $$ = KOREStringPattern::Create($1); }
  | "object-id"
    { driver.startObjectPattern($1);     }
    "{" object-sort-list "}" "(" pattern-list ")"
    { $$ = driver.finishObjectPattern(); }
  ;

object-variable
  : "object-id" ":" object-sort
    { $$ = KOREVariablePattern::Create($1, $3); }
  ;

pattern-list
  : /* empty */
  | pattern-list-non-empty
  ;

pattern-list-non-empty
  : pattern                            { driver.addPattern($1); }
  | pattern-list-non-empty "," pattern { driver.addPattern($3); }
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
  : import-declaration { $$ = $1; }
  | sort-declaration   { $$ = $1; }
  | symbol-declaration { $$ = $1; }
  | alias-declaration  { $$ = $1; }
  | axiom-declaration  { $$ = $1; }
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
  : object-symbol-declaration { $$ = $1; }
  ;

object-symbol-declaration
  : "symbol" "object-id"
    { driver.startObjectSymbolDeclaration($2);       }
    "{" object-sort-variable-list "}" "(" object-sort-list ")"
    ":" object-sort attribute
    { $$ = driver.finishObjectSymbolDeclaration($11); }
  | "hooked-symbol" "object-id"
    { driver.startObjectSymbolDeclaration($2, true);  }
    "{" object-sort-variable-list "}" "(" object-sort-list ")"
    ":" object-sort attribute
    { $$ = driver.finishObjectSymbolDeclaration($11); }
  ;

alias-declaration
  : object-alias-declaration { $$ = $1; }
  ;

object-alias-declaration
  : "alias" "object-id"
    { driver.startObjectAliasDeclaration($2);             }
    "{" object-sort-variable-list "}" "(" object-sort-list ")" ":" object-sort
    "where" "object-id" "{" object-sort-variable-list "}"
    "(" object-variable-list ")" ":=" object-pattern attribute
    { $$ = driver.finishObjectAliasDeclaration($11, $21); }
  ;

axiom-declaration
  : "axiom"
    { driver.startAxiomDeclaration();       }
    "{" sort-variable-list "}" pattern attribute
    { $$ = driver.finishAxiomDeclaration($6); }
  ;

sort-variable-list
  : /* empty */
  | sort-variable-list-non-empty
  ;

sort-variable-list-non-empty
  : object-sort-variable { driver.addObjectSortVariable($1); }
  | sort-variable-list-non-empty "," object-sort-variable
    { driver.addObjectSortVariable($3); }
  ;

object-sort-variable-list
  : /* empty */
  | object-sort-variable-list-non-empty
  ;

object-sort-variable-list-non-empty
  : object-sort-variable { driver.addObjectSortVariable($1); }
  | object-sort-variable-list-non-empty "," object-sort-variable
    { driver.addObjectSortVariable($3); }
  ;

object-variable-list
  : /* empty */
  | object-variable-list-non-empty
  ;

object-variable-list-non-empty
  : object-variable { driver.addObjectVariable($1); }
  | object-variable-list-non-empty "," object-variable
    { driver.addObjectVariable($3); }
  ;

declaration-list
  : /* empty */
  | declaration-list declaration { driver.addDeclaration($2); }
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
  : module                       { driver.addModule($1); }
  | module-list-non-empty module { driver.addModule($2); }
  ;

%%

void kllvm::parser::KOREParser::error(
      const location_type &loc, const std::string &err_message) {
  std::cerr << "Syntax error at " << loc << ": " << err_message << "\n";
  exit(-1);
}
