#ifndef KOREPARSER_H
#define KOREPARSER_H

#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/location.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace kllvm::parser {

class KOREParser {
public:
  KOREParser(std::string const &filename)
      : scanner(KOREScanner(filename))
      , loc(location(filename)) { }

  static std::unique_ptr<KOREParser> from_string(std::string const &text);

  ptr<KOREDefinition> definition();
  sptr<KOREPattern> pattern();
  sptr<KORESort> sort();
  std::vector<ptr<KOREDeclaration>> declarations();

  std::pair<std::string, std::vector<sptr<KORESort>>> symbol_sort_list();

private:
  KOREScanner scanner;
  location loc;
  [[noreturn]] static void
  error(location const &loc, std::string const &err_message);

  std::string consume(token next);
  token peek();

  template <typename Node>
  void attributes(Node *node);

  template <typename Node>
  void attributesNE(Node *node);

  void modules(KOREDefinition *node);
  ptr<KOREModule> module();

  void sentences(KOREModule *node);
  ptr<KOREDeclaration> sentence();

  void sortVariables(KOREDeclaration *node);
  void sortVariablesNE(KOREDeclaration *node);

  template <typename Node>
  void sorts(Node *node);
  template <typename Node>
  void sortsNE(Node *node);

  sptr<KOREPattern> _pattern();
  void patterns(KORECompositePattern *node);
  void patternsNE(KORECompositePattern *node);
  void patterns(std::vector<sptr<KOREPattern>> &node);
  void patternsNE(std::vector<sptr<KOREPattern>> &node);

  sptr<KOREPattern> applicationPattern();
  sptr<KOREPattern> applicationPattern(std::string const &name);
  ptr<KORECompositePattern> _applicationPattern();
  ptr<KORECompositePattern> _applicationPattern(std::string const &name);

  struct {
    std::string data;
    token tok;
  } buffer = {"", token::EMPTY};
};

} // namespace kllvm::parser

#endif // KOREPARSER_
