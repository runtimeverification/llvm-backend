#ifndef KOREPARSER_H
#define KOREPARSER_H

#include "kllvm/ast/AST.h"

#include <string>

namespace kllvm {
namespace parser {

class position {
public:
  std::string filename;
  unsigned line;
  unsigned column;

  void lines(int count = 1) {
    if (count) {
      column = 1;
      line = add_(line, count, 1);
    }
  }

  void columns(int count = 1) {
    column = add_(column, count, 1);
  }

private:
  static unsigned add_(unsigned lhs, int rhs, int min) {
    return static_cast<unsigned>(std::max(min, static_cast<int>(lhs) + rhs));
  }
};

inline position&
operator+= (position& res, int width)
{
  res.columns(width);
  return res;
}
 
template <typename YYChar>
std::basic_ostream<YYChar>&
operator<< (std::basic_ostream<YYChar>& ostr, const position& pos) {
  ostr << pos.filename << ':';
  return ostr << pos.line << '.' << pos.column;
}

class location {
public:
  position begin;
  position end;

  void step() {
    begin = end;
  }

  void columns(int count = 1) {
    end += count;
  }

  void lines(int count = 1) {
    end.lines(count);
  }

};

template <typename YYChar>
std::basic_ostream<YYChar>&
operator<< (std::basic_ostream<YYChar>& ostr, const location& loc) {
  unsigned end_col = 0 < loc.end.column ? loc.end.column - 1 : 0;
  ostr << loc.begin;
  if (loc.begin.filename != loc.end.filename)
    ostr << '-' << loc.end.filename << ':' << loc.end.line << '.' << end_col;
  else if (loc.begin.line < loc.end.line)
    ostr << '-' << loc.end.line << '.' << end_col;
  else if (loc.begin.column < end_col)
    ostr << '-' << end_col;
  return ostr;
}

class KOREScanner;

class KOREParser {
public:
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

  KOREParser(KOREScanner &scanner) :
	  scanner(scanner) {}

  KOREDefinition *definition(void);
  KOREPattern *pattern(void);

private:
  KOREScanner &scanner;
  [[ noreturn ]] void error(const location &loc, const std::string &err_message);

  std::string consume(token next);
  token peek(void);

  template <typename Node>
  void attributes(Node *node);

  template <typename Node>
  void attributesNE(Node *node);

  void modules(KOREDefinition *node);
  KOREModule *module(void);

  void sentences(KOREModule *node);
  KOREDeclaration *sentence(void);

  void sortVariables(KOREDeclaration *node);
  void sortVariablesNE(KOREDeclaration *node);

  template <typename Node>
  void sorts(Node *node);
  template <typename Node>
  void sortsNE(Node *node);
  KORESort *sort(void);

  KOREPattern *_pattern(void);
  void patterns(KORECompositePattern *node);
  void patternsNE(KORECompositePattern *node);

  KORECompositePattern *applicationPattern(void);
  KORECompositePattern *applicationPattern(std::string name);
};

} // end namespace parser
} // end namespace kllvm

#endif // KOREPARSER_
