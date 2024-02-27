#ifndef KORESCANNER_H
#define KORESCANNER_H

#include "kllvm/parser/location.h"

namespace kllvm::parser {

enum class token {
  EMPTY,
  MODULE,
  ENDMODULE,
  IMPORT,
  SORT,
  SYMBOL,
  WHERE,
  ALIAS,
  AXIOM,
  CLAIM,
  HOOKEDSORT,
  HOOKEDSYMBOL,
  COLON,
  LEFTBRACE,
  RIGHTBRACE,
  LEFTBRACKET,
  RIGHTBRACKET,
  LEFTPAREN,
  RIGHTPAREN,
  COMMA,
  COLONEQUAL,
  ID,
  STRING,
  TOKEN_EOF,
};

class KOREScanner {
public:
  KOREScanner(std::string filename);
  ~KOREScanner();
  int scan();

  friend class KOREParser;

  using yyscan_t = void *;

  KOREScanner(KOREScanner const &other) = delete;
  KOREScanner &operator=(KOREScanner const &other) = delete;

  KOREScanner(KOREScanner &&other) = delete;
  KOREScanner &operator=(KOREScanner &&other) = delete;

private:
  yyscan_t scanner;
  token yylex(std::string *lval, location *loc, yyscan_t yyscanner);
  token yylex(std::string *lval, location *loc) {
    return yylex(lval, loc, scanner);
  }
  void error(location const &loc, std::string const &err_message);
  std::string codepoint_to_utf8(unsigned long int code, location const &loc);

  FILE *in;
  std::string stringBuffer;
};

} // namespace kllvm::parser

#endif // KORESCANNER_H
