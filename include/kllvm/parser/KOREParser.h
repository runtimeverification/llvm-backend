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

class kore_parser {
public:
  kore_parser(std::string const &filename)
      : scanner(kore_scanner(filename))
      , loc(location(filename)) { }

  static std::unique_ptr<kore_parser> from_string(std::string const &text);

  ptr<kore_definition> definition();
  sptr<kore_pattern> pattern();
  sptr<kore_sort> sort();
  std::vector<ptr<kore_declaration>> declarations();

  std::pair<std::string, std::vector<sptr<kore_sort>>> symbol_sort_list();

private:
  kore_scanner scanner;
  location loc;
  [[noreturn]] static void
  error(location const &loc, std::string const &err_message);

  std::string consume(token next);
  token peek();

  template <typename Node>
  void attributes(Node *node);

  template <typename Node>
  void attributesNE(Node *node);

  void modules(kore_definition *node);
  ptr<kore_module> module();

  void sentences(kore_module *node);
  ptr<kore_declaration> sentence();

  void sortVariables(kore_declaration *node);
  void sortVariablesNE(kore_declaration *node);

  template <typename Node>
  void sorts(Node *node);
  template <typename Node>
  void sortsNE(Node *node);

  sptr<kore_pattern> _pattern();
  void patterns(kore_composite_pattern *node);
  void patternsNE(kore_composite_pattern *node);
  void patterns(std::vector<sptr<kore_pattern>> &node);
  void patternsNE(std::vector<sptr<kore_pattern>> &node);

  sptr<kore_pattern> applicationPattern();
  sptr<kore_pattern> applicationPattern(std::string const &name);
  ptr<kore_composite_pattern> _applicationPattern();
  ptr<kore_composite_pattern> _applicationPattern(std::string const &name);

  struct {
    std::string data;
    token tok;
  } buffer = {"", token::EMPTY};
};

} // namespace kllvm::parser

#endif // KOREPARSER_
