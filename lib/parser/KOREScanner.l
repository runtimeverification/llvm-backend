%{

#include "kllvm/parser/KOREScanner.h"
#include <iostream>

using namespace kllvm::parser;

#undef  YY_DECL
#define YY_DECL \
  KOREParser::token KOREScanner::yylex(std::string *lval, location *loc)

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

#define YY_NO_INPUT
%}

/* Flex options */
%option noyywrap
%option nounput

/* Flex macros */
ident @?[a-zA-Z][a-zA-Z0-9'-]*

/* Flex extra states */
%x COMM STR

%%

%{

/* Code executed at the beginning of yylex */
yyin = in;

%}

"\n"    { loc->lines(); }
[ \t\r] { /* nothing */ }

"module"        { return KOREParser::token::MODULE;       }
"endmodule"     { return KOREParser::token::ENDMODULE;    }
"import"        { return KOREParser::token::IMPORT;       }
"sort"          { return KOREParser::token::SORT;         }
"symbol"        { return KOREParser::token::SYMBOL;       }
"where"         { return KOREParser::token::WHERE;        }
"alias"         { return KOREParser::token::ALIAS;        }
"axiom"         { return KOREParser::token::AXIOM;        }
"claim"         { return KOREParser::token::CLAIM;        }
"hooked-sort"   { return KOREParser::token::HOOKEDSORT;   }
"hooked-symbol" { return KOREParser::token::HOOKEDSYMBOL; }

":"  { return KOREParser::token::COLON;   }
"{"  { return KOREParser::token::LEFTBRACE;   }
"}"  { return KOREParser::token::RIGHTBRACE;   }
"["  { return KOREParser::token::LEFTBRACKET;   }
"]"  { return KOREParser::token::RIGHTBRACKET;   }
"("  { return KOREParser::token::LEFTPAREN;    }
")"  { return KOREParser::token::RIGHTPAREN;    }
","  { return KOREParser::token::COMMA; }
":=" { return KOREParser::token::COLONEQUAL; }

{ident} {
  *lval = std::string(yytext, yyleng);
  return KOREParser::token::ID;
}

"\\"{ident} {
  *lval = std::string(yytext, yyleng);
  return KOREParser::token::ID;
}

"\""            { stringBuffer.clear(); BEGIN(STR);           }
<STR>[^\"\n\\]* { stringBuffer.append(yytext);                }
<STR>"\\n"       { stringBuffer.push_back('\n'); loc->lines(); }
<STR>"\\r"       { stringBuffer.push_back('\r'); loc->lines(); }
<STR>"\\t"       { stringBuffer.push_back('\t'); loc->lines(); }
<STR>"\\f"       { stringBuffer.push_back('\f'); loc->lines(); }
<STR>"\\\""     { stringBuffer.push_back('\"');               }
<STR>"\\\\"     { stringBuffer.push_back('\\');               }
<STR>\\[0-9]{3} { stringBuffer.push_back((yytext[1] - '0') * 64 + (yytext[2] - '0') * 8 + yytext[3] - '0'); }
<STR>\\x[0-9a-fA-F]{2} { stringBuffer.push_back(strtol(yytext+2, NULL, 16)); }
<STR>"\""       { BEGIN(INITIAL);
                  *lval = stringBuffer;
                  return KOREParser::token::STRING;           }

"/*"           { BEGIN(COMM);    }
<COMM>[^/*\n]* { /* nothing */   }
<COMM>"/"      { /* nothing */   }
<COMM>"*"      { /* nothing */   }
<COMM>\n       { loc->lines();   }
<COMM>"*/"     { BEGIN(INITIAL); }

"//".*         { /* nothing */ }

<COMM,STR,INITIAL><<EOF>> {
  if(YYSTATE != INITIAL)
    error(*loc, "Either a comment or string haven't been closed\n");

  return KOREParser::token::TOKEN_EOF;
}

. { error(*loc, std::string("Unknown token \"") + yytext + std::string("\"\n")); }

%%

KOREScanner::KOREScanner(const char *filename) {
  if (!(in = fopen(filename, "r"))) {
    std::cerr << "Cannot read file: " << filename << "\n";
    exit(1);
  }
}

void KOREScanner::error(
      const location &loc, const std::string &err_message) {
  std::cerr << "Scanner error at " << loc << ": " << err_message << "\n";
  exit(-1);
}

int KOREScanner::scan() {
  KOREParser::token token;
  do {
    std::string sem;
    location loc;
    token = yylex(&sem, &loc);
    std::string lexeme;
    switch (token) {
    case KOREParser::token::ID:
    case KOREParser::token::STRING:
      lexeme = sem;
      break;
    default:
      lexeme = yytext;
    }
  } while (token != KOREParser::token::TOKEN_EOF);

  return 0;
}
