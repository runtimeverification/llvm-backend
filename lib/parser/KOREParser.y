%skeleton "lalr1.cc"
%define api.namespace {kllvm::parser}
%define parser_class_name {KOREParser}
%define api.value.type variant
%define parse.assert
%define parse.trace
%locations
%parse-param {KOREScanner &scanner}

%code requires {

namespace kllvm {
namespace parser {
class KOREScanner;
} // end namespace parser
} // end namespace kllvm

#define YYDEBUG 0

}

%code {

#include "kllvm/parser/KOREScanner.h"

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

/* associativity and priority definition */

%%

/* Sorts */
object-sort
  : object-sort-variable
  | object-non-variable-sort
  ;

object-sort-variable : "object-id"
                     ;

object-non-variable-sort : object-sort-constructor "{" object-sort-list "}"
                         ;

object-sort-constructor : "object-id"
                        ;

object-sort-list : /* empty */
                 | object-sort-list-non-empty
                 ;

object-sort-list-non-empty : object-sort
                           | object-sort-list-non-empty "," object-sort
                           ;

meta-sort : meta-sort-variable
          | meta-non-variable-sort
          ;

meta-sort-variable : "meta-id"
                   ;

meta-non-variable-sort : meta-sort-constructor "{" "}"
                       ;

meta-sort-constructor : "meta-id" // Only specific meta ids can be used here

meta-sort-list : /* empty */
               | meta-sort-list-non-empty
               ;

meta-sort-list-non-empty : meta-sort
                         | meta-sort-list-non-empty "," meta-sort
                         ;


/* Heads */
object-head : object-head-constructor "{" object-sort-list "}"
            ;

object-head-constructor : "object-id"
                        ;

meta-head : meta-head-constructor "{" meta-sort-list "}"
            ;

meta-head-constructor : "meta-id"
                        ;


/* Patterns */
pattern : object-pattern
        | meta-pattern
        ;

object-pattern : object-variable
               | object-head "(" pattern-list ")"
               ;

object-variable : "object-id" ":" object-sort
                ;

meta-pattern : meta-variable
             | "kore-string"
             | "kore-char"
             | meta-head "(" pattern-list ")"
             ;

meta-variable : "meta-id" ":" meta-sort
              ;

pattern-list : /* empty */
             | pattern-list-non-empty
             ;

pattern-list-non-empty : pattern
                       | pattern-list-non-empty "," pattern
                       ;


/* Attributes */
attribute : "[" pattern-list "]"
          ;


/* Modules and Declarations */
module : "module" module-name declaration-list "endmodule" attribute
       ;

declaration : import-declaration
            | sort-declaration
            | symbol-declaration
            | alias-declaration
            | axiom-declaration
            | hook-declaration
            ;

import-declaration : "import" module-name attribute
                   ;

sort-declaration : "sort" object-sort-constructor "{" object-sort-variable-list "}" attribute
                 ;

symbol-declaration : object-symbol-declaration
                   | meta-symbol-declaration
                   ;

object-symbol-declaration
  : "symbol" object-head-constructor "{" object-sort-variable-list "}"
    "("object-sort-list ")" ":" object-sort attribute
  ;

meta-symbol-declaration
  : "symbol" meta-head-constructor "{" meta-sort-variable-list "}"
    "("meta-sort-list ")" ":" meta-sort attribute
  ;

alias-declaration : object-alias-declaration
                  | meta-alias-declaration
                  ;

object-alias-declaration
  : "alias" object-head-constructor "{" object-sort-variable-list "}"
    "(" object-sort-list ")" ":" object-sort
    "where" object-head-constructor "{" object-sort-variable-list "}"
    "(" object-variable-list ")" ":=" object-pattern attribute
  ;

meta-alias-declaration
  : "alias" meta-head-constructor "{" meta-sort-variable-list "}"
    "(" meta-sort-list ")" ":" meta-sort
    "where" meta-head-constructor "{" meta-sort-variable-list "}"
    "(" meta-variable-list ")" ":=" meta-pattern attribute
  ;

axiom-declaration : "axiom" "{" sort-variable-list "}" pattern attribute
                  ;

hook-declaration
  : "hooked-sort" object-sort-constructor "{" object-sort-variable-list "}" attribute
  | "hooked-symbol" object-head-constructor "{" object-sort-variable-list "}"
    "(" object-sort-list ")" ":" object-sort attribute
  ;

sort-variable-list : /* empty */
                   | sort-variable-list-non-empty
                   ;

sort-variable-list-non-empty : object-sort-variable
                             | meta-sort-variable
                             | sort-variable-list-non-empty "," object-sort-variable
                             | sort-variable-list-non-empty "," meta-sort-variable
                             ;

object-sort-variable-list : /* empty */
                          | object-sort-variable-list-non-empty
                          ;

object-sort-variable-list-non-empty : object-sort-variable
                                    | object-sort-variable-list-non-empty "," object-sort-variable
                                    ;

meta-sort-variable-list : /* empty */
                        | meta-sort-variable-list-non-empty
                        ;

meta-sort-variable-list-non-empty : meta-sort-variable
                                  | meta-sort-variable-list-non-empty "," meta-sort-variable
                                  ;

object-variable-list : /* empty */
                     | object-variable-list-non-empty
                     ;

object-variable-list-non-empty : object-variable
                               | object-variable-list-non-empty "," object-variable
                               ;

meta-variable-list : /* empty */
                   | meta-variable-list-non-empty
                   ;

meta-variable-list-non-empty : meta-variable
                             | meta-variable-list-non-empty "," meta-variable
                             ;

declaration-list : /* empty */
                 | declaration-list declaration
                 ;

module-name : "object-id"
            ;


/* Definitions */
definition : attribute module-list-non-empty
           ;

module-list-non-empty : module
                      | module-list-non-empty module
                      ;

%%

void kllvm::parser::KOREParser::error(
      const location_type &loc, const std::string &err_message) {
  std::cerr << "Syntax error at " << loc << ": " << err_message << "\n";
  exit(-1);
}
