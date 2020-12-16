#ifndef KOREPARSER_H
#define KOREPARSER_H

#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREScanner.h"

namespace kllvm {
namespace parser {

class KOREParser {
public:
  KOREParser(std::string filename) :
	  scanner(KOREScanner(filename)), loc(location(filename)) {}

  ptr<KOREDefinition> definition(void);
  ptr<KOREPattern> pattern(void);
  std::vector<ptr<KOREDeclaration>> declarations(void);

private:
  KOREScanner scanner;
  location loc;
  [[ noreturn ]] void error(const location &loc, const std::string &err_message);

  std::string consume(token next);
  token peek(void);

  template <typename Node>
  void attributes(Node *node);

  template <typename Node>
  void attributesNE(Node *node);

  void modules(KOREDefinition *node);
  ptr<KOREModule> module(void);

  void sentences(KOREModule *node);
  ptr<KOREDeclaration> sentence(void);

  void sortVariables(KOREDeclaration *node);
  void sortVariablesNE(KOREDeclaration *node);

  template <typename Node>
  void sorts(Node *node);
  template <typename Node>
  void sortsNE(Node *node);
  sptr<KORESort> sort(void);

  ptr<KOREPattern> _pattern(void);
  void patterns(KORECompositePattern *node);
  void patternsNE(KORECompositePattern *node);
  void patterns(std::vector<ptr<KOREPattern>> &node);
  void patternsNE(std::vector<ptr<KOREPattern>> &node);

  ptr<KOREPattern> applicationPattern(void);
  ptr<KOREPattern> applicationPattern(std::string name);
  ptr<KORECompositePattern> _applicationPattern(void);
  ptr<KORECompositePattern> _applicationPattern(std::string name);

  struct {
    std::string data;
    token tok;
  } buffer = {"", token::EMPTY};
};

} // end namespace parser
} // end namespace kllvm

#endif // KOREPARSER_
