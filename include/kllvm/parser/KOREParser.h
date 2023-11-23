#ifndef KOREPARSER_H
#define KOREPARSER_H

#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/location.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace kllvm {
namespace parser {

class KOREParser {
public:
  KOREParser(std::string filename)
      : scanner(KOREScanner(filename))
      , loc(location(filename)) { }

  static std::unique_ptr<KOREParser> from_string(std::string text);

  ptr<KOREDefinition> definition(void);
  sptr<KOREPattern> pattern(void);
  sptr<KORESort> sort(void);
  std::vector<ptr<KOREDeclaration>> declarations(void);

  std::pair<std::string, std::vector<sptr<KORESort>>> symbol_sort_list();

private:
  KOREScanner scanner;
  location loc;
  [[noreturn]] void error(const location &loc, const std::string &err_message);

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

  sptr<KOREPattern> _pattern(void);
  void patterns(KORECompositePattern *node);
  void patternsNE(KORECompositePattern *node);
  void patterns(std::vector<sptr<KOREPattern>> &node);
  void patternsNE(std::vector<sptr<KOREPattern>> &node);

  sptr<KOREPattern> applicationPattern(void);
  sptr<KOREPattern> applicationPattern(std::string name);
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
