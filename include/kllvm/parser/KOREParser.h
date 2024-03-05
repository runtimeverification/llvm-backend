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
      : scanner_(kore_scanner(filename))
      , loc_(location(filename)) { }

  static std::unique_ptr<kore_parser> from_string(std::string const &text);

  ptr<kore_definition> definition();
  sptr<kore_pattern> pattern();
  sptr<kore_sort> sort();
  std::vector<ptr<kore_declaration>> declarations();

  std::pair<std::string, std::vector<sptr<kore_sort>>> symbol_sort_list();

private:
  kore_scanner scanner_;
  location loc_;
  [[noreturn]] static void
  error(location const &loc, std::string const &err_message);

  std::string consume(token next);
  token peek();

  template <typename Node>
  void attributes(Node *node);

  template <typename Node>
  void attributes_ne(Node *node);

  void modules(kore_definition *node);
  ptr<kore_module> module();

  void sentences(kore_module *node);
  ptr<kore_declaration> sentence();

  void sort_variables(kore_declaration *node);
  void sort_variables_ne(kore_declaration *node);

  template <typename Node>
  void sorts(Node *node);
  template <typename Node>
  void sorts_ne(Node *node);

  sptr<kore_pattern> pattern_internal();
  void patterns(kore_composite_pattern *node);
  void patterns_ne(kore_composite_pattern *node);
  void patterns(std::vector<sptr<kore_pattern>> &node);
  void patterns_ne(std::vector<sptr<kore_pattern>> &node);

  sptr<kore_pattern> application_pattern();
  sptr<kore_pattern> application_pattern(std::string const &name);
  ptr<kore_composite_pattern> application_pattern_internal();
  ptr<kore_composite_pattern>
  application_pattern_internal(std::string const &name);

  struct {
    std::string data;
    token tok;
  } buffer_ = {"", token::Empty};
};

} // namespace kllvm::parser

#endif // KOREPARSER_
