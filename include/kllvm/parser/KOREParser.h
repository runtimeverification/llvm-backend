#ifndef KOREPARSER_H
#define KOREPARSER_H

#include "kllvm/ast/AST.h"

#include <string>

namespace kllvm {
namespace parser {

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
