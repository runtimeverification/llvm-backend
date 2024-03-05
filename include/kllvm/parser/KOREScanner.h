#ifndef KORESCANNER_H
#define KORESCANNER_H

#include "kllvm/parser/location.h"

namespace kllvm::parser {

enum class token {
  Empty,
  Module,
  EndModule,
  Import,
  Sort,
  Symbol,
  Where,
  Alias,
  Axiom,
  Claim,
  HookedSort,
  HookedSymbol,
  Colon,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
  LeftParen,
  RightParen,
  Comma,
  ColonEqual,
  Id,
  String,
  TokenEof,
};

class kore_scanner {
public:
  kore_scanner(std::string filename);
  ~kore_scanner();
  int scan();

  friend class kore_parser;

  using yyscan_t = void *;

  kore_scanner(kore_scanner const &other) = delete;
  kore_scanner &operator=(kore_scanner const &other) = delete;

  kore_scanner(kore_scanner &&other) = delete;
  kore_scanner &operator=(kore_scanner &&other) = delete;

private:
  yyscan_t scanner_;
  token yylex(std::string *lval, location *loc, yyscan_t yyscanner);
  token yylex(std::string *lval, location *loc) {
    return yylex(lval, loc, scanner_);
  }
  void error(location const &loc, std::string const &err_message);
  std::string codepoint_to_utf8(unsigned long int code, location const &loc);

  FILE *in_;
  std::string string_buffer_;
};

} // namespace kllvm::parser

#endif // KORESCANNER_H
