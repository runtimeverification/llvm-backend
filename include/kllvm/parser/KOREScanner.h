#ifndef KORESCANNER_H
#define KORESCANNER_H

#include "kllvm/parser/KOREParser.hh"

namespace kllvm {
namespace parser {

class KOREScanner {
public:
  KOREScanner(const char *filename);
  int scan();

friend class KOREParser;

private:
  int yylex(KOREParser::semantic_type *lval, KOREParser::location_type *loc);
  void error(const KOREParser::location_type &loc, const std::string &err_message);
  FILE *in;
  std::string stringBuffer;
};

} // end namespace parser
} // end namespace kllvm

#endif // KORESCANNER_H
