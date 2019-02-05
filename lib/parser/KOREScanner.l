%{

#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/location.hh"

using namespace kllvm::parser;

#undef  YY_DECL
#define YY_DECL \
  int KOREScanner::yylex(KOREParser::semantic_type *lval, location *loc)

#define TOKEN_EOF 0

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

#define YY_NO_INPUT
%}

/* Flex options */
%option noyywrap
%option nounput

/* Flex macros */
ident [a-zA-Z][a-zA-Z0-9'-]*

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
"hooked-sort"   { return KOREParser::token::HOOKEDSORT;   }
"hooked-symbol" { return KOREParser::token::HOOKEDSYMBOL; }

":"  { return KOREParser::token::CLN;   }
"{"  { return KOREParser::token::LCB;   }
"}"  { return KOREParser::token::RCB;   }
"["  { return KOREParser::token::LBR;   }
"]"  { return KOREParser::token::RBR;   }
"("  { return KOREParser::token::LP;    }
")"  { return KOREParser::token::RP;    }
","  { return KOREParser::token::COMMA; }
":=" { return KOREParser::token::CLNEQ; }

{ident} {
  lval->build<std::string>(yytext);
  return KOREParser::token::OBJECTID;
}

"\\"{ident} {
  lval->build<std::string>(yytext);
  return KOREParser::token::OBJECTID;
}

"#"{ident} {
  lval->build<std::string>(yytext + 1);
  return KOREParser::token::METAID;
}

"#\\"{ident} {
  lval->build<std::string>(yytext + 1);
  return KOREParser::token::METAID;
}

"#`"{ident} {
  lval->build<std::string>(yytext + 1);
  return KOREParser::token::METAID;
}

"'"[[:print:]]"'" {
  lval->build<char>(yytext[1]);
  return KOREParser::token::CHAR;
}

"\""            { stringBuffer.clear(); BEGIN(STR);           }
<STR>[^\"\n\\]* { stringBuffer.append(yytext);                }
<STR>"\n"       { stringBuffer.push_back('\n'); loc->lines(); }
<STR>"\\\""     { stringBuffer.push_back('\"');               }
<STR>"\\\\"     { stringBuffer.push_back('\\');               }
<STR>\\[0-9]{3} { stringBuffer.push_back((yytext[1] - '0') * 64 + (yytext[2] - '0') * 8 + yytext[3] - '0'); }
<STR>\\x[0-9a-fA-F]{2} { stringBuffer.push_back(strtol(yytext+2, NULL, 16)); }
<STR>"\""       { BEGIN(INITIAL);
                  lval->build<std::string>(stringBuffer);
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

  return TOKEN_EOF;
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
      const KOREParser::location_type &loc, const std::string &err_message) {
  std::cerr << "Scanner error at " << loc << ": " << err_message << "\n";
  exit(-1);
}

int KOREScanner::scan() {
  int token;
  do {
    KOREParser::semantic_type sem;
    KOREParser::location_type loc;
    token = yylex(&sem, &loc);
    std::string lexeme;
    switch (token) {
    case KOREParser::token::OBJECTID:
    case KOREParser::token::METAID:
    case KOREParser::token::STRING:
      lexeme = sem.as<std::string>();
      sem.destroy<std::string>();
      break;
    case KOREParser::token::CHAR:
      lexeme.push_back(sem.as<char>());
      sem.destroy<char>();
      break;
    default:
      lexeme = yytext;
    }
    std::cout << "token=" << token
              << ", lexeme=\"" << lexeme << "\""
              << ", location=" << loc
              << "\n";
  } while (token != TOKEN_EOF);

  return 0;
}
