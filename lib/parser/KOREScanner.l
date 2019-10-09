%{

#include "kllvm/parser/KOREScanner.h"
#include <iostream>

using namespace kllvm::parser;

#undef  YY_DECL
#define YY_DECL \
  token KOREScanner::yylex(std::string *lval, location *loc, yyscan_t yyscanner)

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

#define YY_NO_INPUT
%}

/* Flex options */
%option noyywrap
%option nounput
%option reentrant

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

"module"        { return token::MODULE;       }
"endmodule"     { return token::ENDMODULE;    }
"import"        { return token::IMPORT;       }
"sort"          { return token::SORT;         }
"symbol"        { return token::SYMBOL;       }
"where"         { return token::WHERE;        }
"alias"         { return token::ALIAS;        }
"axiom"         { return token::AXIOM;        }
"claim"         { return token::CLAIM;        }
"hooked-sort"   { return token::HOOKEDSORT;   }
"hooked-symbol" { return token::HOOKEDSYMBOL; }

":"  { return token::COLON;   }
"{"  { return token::LEFTBRACE;   }
"}"  { return token::RIGHTBRACE;   }
"["  { return token::LEFTBRACKET;   }
"]"  { return token::RIGHTBRACKET;   }
"("  { return token::LEFTPAREN;    }
")"  { return token::RIGHTPAREN;    }
","  { return token::COMMA; }
":=" { return token::COLONEQUAL; }

{ident} {
  *lval = std::string(yytext, yyleng);
  return token::ID;
}

"\\"{ident} {
  *lval = std::string(yytext, yyleng);
  return token::ID;
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
                  return token::STRING;           }

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

  return token::TOKEN_EOF;
}

. { error(*loc, std::string("Unknown token \"") + yytext + std::string("\"\n")); }

%%

KOREScanner::KOREScanner(std::string filename) {
  if (!(in = fopen(filename.c_str(), "r"))) {
    std::cerr << "Cannot read file: " << filename << "\n";
    exit(1);
  }
  yylex_init(&scanner);
}

KOREScanner::~KOREScanner() {
  fclose(in);
  yylex_destroy(scanner);
}

void KOREScanner::error(
      const location &loc, const std::string &err_message) {
  std::cerr << "Scanner error at " << loc << ": " << err_message << "\n";
  exit(-1);
}

int KOREScanner::scan() {
  token token;
  do {
    std::string sem;
    location loc("");
    token = yylex(&sem, &loc, scanner);
    std::string lexeme;
    switch (token) {
    case token::ID:
    case token::STRING:
      lexeme = sem;
      break;
    default:
      lexeme = yyget_text(scanner);
    }
  } while (token != token::TOKEN_EOF);

  return 0;
}
