#ifndef KORESCANNER_H
#define KORESCANNER_H

#include "kllvm/parser/location.h"

namespace kllvm {
namespace parser {

class KOREScanner {
public:
  KOREScanner(const char *filename);
  int scan();

friend class KOREParser;

private:
  KOREParser::token yylex(std::string *lval, location *loc);
  void error(const location &loc, const std::string &err_message);
  FILE *in;
  std::string stringBuffer;
};

} // end namespace parser
} // end namespace kllvm

#endif // KORESCANNER_H
