#include "kllvm/parser/KOREParser.h"
#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREScanner.h"
#include "kllvm/util/temporary_file.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace kllvm::parser {

std::unique_ptr<kore_parser> kore_parser::from_string(std::string const &text) {
  auto temp_file = temporary_file("tmp.parse.XXXXXX");
  temp_file.ofstream() << text;

  auto parser = std::make_unique<kore_parser>(temp_file.filename());
  return parser;
}

void kore_parser::error(location const &loc, std::string const &err_message) {
  std::cerr << "Syntax error at " << loc << ": " << err_message << "\n";
  exit(-1);
}

static std::string str(token tok) {
  switch (tok) {
  case token::EMPTY: abort();
  case token::MODULE: return "module";
  case token::ENDMODULE: return "endmodule";
  case token::IMPORT: return "import";
  case token::SORT: return "sort";
  case token::SYMBOL: return "symbol";
  case token::WHERE: return "where";
  case token::ALIAS: return "alias";
  case token::AXIOM: return "axiom";
  case token::CLAIM: return "claim";
  case token::HOOKEDSORT: return "hooked-sort";
  case token::HOOKEDSYMBOL: return "hooked-symbol";
  case token::COLON: return ":";
  case token::LEFTBRACE: return "{";
  case token::RIGHTBRACE: return "}";
  case token::LEFTBRACKET: return "[";
  case token::RIGHTBRACKET: return "]";
  case token::LEFTPAREN: return "(";
  case token::RIGHTPAREN: return ")";
  case token::COMMA: return ",";
  case token::COLONEQUAL: return ":=";
  case token::ID: return "<identifier>";
  case token::STRING: return "<string>";
  case token::TOKEN_EOF: return "<EOF>";
  default: abort();
  }
}

std::string kore_parser::consume(token next) {
  std::string data;
  token actual = token::EMPTY;
  if (buffer_.tok == token::EMPTY) {
    actual = scanner_.yylex(&data, &loc_);
  } else {
    actual = buffer_.tok;
    data = buffer_.data;
    buffer_.tok = token::EMPTY;
  }
  if (actual == next) {
    return data;
  }
  error(loc_, "Expected: " + str(next) + " Actual: " + str(actual));
}

token kore_parser::peek() {
  std::string data;
  if (buffer_.tok == token::EMPTY) {
    buffer_.tok = scanner_.yylex(&data, &loc_);
    buffer_.data = data;
  }
  return buffer_.tok;
}

ptr<kore_definition> kore_parser::definition() {
  consume(token::LEFTBRACKET);
  auto result = kore_definition::Create();
  attributes(result.get());
  consume(token::RIGHTBRACKET);
  modules(result.get());
  consume(token::TOKEN_EOF);
  return result;
}

sptr<kore_pattern> kore_parser::pattern() {
  auto result = _pattern();
  consume(token::TOKEN_EOF);
  return result;
}

template <typename Node>
void kore_parser::attributes(Node *node) {
  if (peek() == token::ID) {
    attributesNE(node);
  }
}

template <typename Node>
void kore_parser::attributesNE(Node *node) {
  auto pat = _applicationPattern();
  node->attributes().add(std::move(pat));
  while (peek() == token::COMMA) {
    consume(token::COMMA);
    pat = _applicationPattern();
    node->attributes().add(std::move(pat));
  }
}

void kore_parser::modules(kore_definition *node) {
  auto mod = module();
  node->addModule(std::move(mod));
  while (peek() == token::MODULE) {
    mod = module();
    node->addModule(std::move(mod));
  }
}

ptr<kore_module> kore_parser::module() {
  consume(token::MODULE);
  std::string name = consume(token::ID);
  auto mod = kore_module::Create(name);
  sentences(mod.get());
  consume(token::ENDMODULE);
  consume(token::LEFTBRACKET);
  attributes(mod.get());
  consume(token::RIGHTBRACKET);
  return mod;
}

void kore_parser::sentences(kore_module *node) {
  while (peek() != token::ENDMODULE) {
    auto decl = sentence();
    node->addDeclaration(std::move(decl));
  }
}

std::vector<ptr<kore_declaration>> kore_parser::declarations() {
  std::vector<ptr<kore_declaration>> decls;
  while (peek() != token::TOKEN_EOF) {
    auto decl = sentence();
    decls.push_back(std::move(decl));
  }
  return decls;
}

ptr<kore_declaration> kore_parser::sentence() {
  std::string name;
  token current = peek();
  switch (current) {
  case token::IMPORT: {
    consume(token::IMPORT);
    name = consume(token::ID);
    auto import = kore_module_import_declaration::Create(name);
    consume(token::LEFTBRACKET);
    attributes(import.get());
    consume(token::RIGHTBRACKET);
    return import;
  }
  case token::SORT:
  case token::HOOKEDSORT: {
    consume(current);
    name = consume(token::ID);
    consume(token::LEFTBRACE);
    auto sortDecl = kore_composite_sort_declaration::Create(
        name, current == token::HOOKEDSORT);
    sortVariables(sortDecl.get());
    consume(token::RIGHTBRACE);
    consume(token::LEFTBRACKET);
    attributes(sortDecl.get());
    consume(token::RIGHTBRACKET);
    return sortDecl;
  }
  case token::SYMBOL:
  case token::HOOKEDSYMBOL: {
    consume(current);
    name = consume(token::ID);
    consume(token::LEFTBRACE);
    auto symbol
        = kore_symbol_declaration::Create(name, current == token::HOOKEDSYMBOL);
    sortVariables(symbol.get());
    consume(token::RIGHTBRACE);
    consume(token::LEFTPAREN);
    sorts(symbol->getSymbol());
    consume(token::RIGHTPAREN);
    consume(token::COLON);
    auto returnSort = sort();
    symbol->getSymbol()->addSort(std::move(returnSort));
    consume(token::LEFTBRACKET);
    attributes(symbol.get());
    consume(token::RIGHTBRACKET);
    return symbol;
  }
  case token::ALIAS: {
    consume(token::ALIAS);
    name = consume(token::ID);
    consume(token::LEFTBRACE);
    auto alias = kore_alias_declaration::Create(name);
    sortVariables(alias.get());
    consume(token::RIGHTBRACE);
    consume(token::LEFTPAREN);
    sorts(alias->getSymbol());
    consume(token::RIGHTPAREN);
    consume(token::COLON);
    auto returnSort = sort();
    alias->getSymbol()->addSort(std::move(returnSort));
    consume(token::WHERE);
    auto variables = _applicationPattern();
    alias->addVariables(std::move(variables));
    consume(token::COLONEQUAL);
    auto pat = _pattern();
    alias->addPattern(std::move(pat));
    consume(token::LEFTBRACKET);
    attributes(alias.get());
    consume(token::RIGHTBRACKET);
    return alias;
  }
  case token::AXIOM:
  case token::CLAIM: {
    consume(current);
    consume(token::LEFTBRACE);
    auto axiom = kore_axiom_declaration::Create(current == token::CLAIM);
    sortVariables(axiom.get());
    consume(token::RIGHTBRACE);
    auto pat = _pattern();
    axiom->addPattern(std::move(pat));
    consume(token::LEFTBRACKET);
    attributes(axiom.get());
    consume(token::RIGHTBRACKET);
    return axiom;
  }
  default:
    error(
        loc_, "Expected: [import, sort, hooked-sort, symbol, hooked-symbol, "
             "alias, axiom, claim] Actual: "
                 + str(current));
  }
}

void kore_parser::sortVariables(kore_declaration *node) {
  if (peek() == token::ID) {
    sortVariablesNE(node);
  }
}

void kore_parser::sortVariablesNE(kore_declaration *node) {
  std::string name = consume(token::ID);
  auto var = kore_sort_variable::Create(name);
  node->addObjectSortVariable(var);
  while (peek() == token::COMMA) {
    consume(token::COMMA);
    name = consume(token::ID);
    var = kore_sort_variable::Create(name);
    node->addObjectSortVariable(var);
  }
}

template <typename Node>
void kore_parser::sorts(Node *node) {
  if (peek() == token::ID) {
    sortsNE(node);
  }
}

template <typename Node>
void kore_parser::sortsNE(Node *node) {
  auto _sort = sort();
  node->addArgument(std::move(_sort));
  while (peek() == token::COMMA) {
    consume(token::COMMA);
    _sort = sort();
    node->addArgument(std::move(_sort));
  }
}

std::pair<std::string, std::vector<sptr<kore_sort>>>
kore_parser::symbol_sort_list() {
  auto ret = std::vector<sptr<kore_sort>>{};

  auto id = consume(token::ID);
  consume(token::LEFTBRACE);

  if (peek() == token::ID) {
    ret.push_back(sort());

    while (peek() == token::COMMA) {
      consume(token::COMMA);
      ret.push_back(sort());
    }
  }

  consume(token::RIGHTBRACE);
  return {id, ret};
}

sptr<kore_sort> kore_parser::sort() {
  std::string name = consume(token::ID);
  if (peek() == token::LEFTBRACE) {
    consume(token::LEFTBRACE);
    auto sort = kore_composite_sort::Create(name);
    sorts(sort.get());
    consume(token::RIGHTBRACE);
    return sort;
  }
  return kore_sort_variable::Create(name);
}

sptr<kore_pattern> kore_parser::_pattern() {
  token current = peek();
  switch (current) {
  case token::ID: {
    std::string name = consume(token::ID);
    current = peek();
    switch (current) {
    case token::COLON:
      consume(token::COLON);
      return kore_variable_pattern::Create(name, sort());
    case token::LEFTBRACE: return applicationPattern(name);
    default: error(loc_, "Expected: [:, {] Actual: " + str(current));
    }
  }
  case token::STRING: return kore_string_pattern::Create(consume(token::STRING));
  default: error(loc_, "Expected: [<id>, <string>] Actual: " + str(current));
  }
}

sptr<kore_pattern> kore_parser::applicationPattern() {
  return applicationPattern(consume(token::ID));
}
ptr<kore_composite_pattern> kore_parser::_applicationPattern() {
  return _applicationPattern(consume(token::ID));
}

sptr<kore_pattern> kore_parser::applicationPattern(std::string const &name) {
  if (name == "\\left-assoc" || name == "\\right-assoc") {
    consume(token::LEFTBRACE);
    consume(token::RIGHTBRACE);
    consume(token::LEFTPAREN);
    std::string symbol = consume(token::ID);
    consume(token::LEFTBRACE);
    auto pat = kore_composite_pattern::Create(symbol);
    sorts(pat->getConstructor());
    pat->getConstructor()->initPatternArguments();
    consume(token::RIGHTBRACE);
    consume(token::LEFTPAREN);
    std::vector<sptr<kore_pattern>> pats;
    patterns(pats);
    consume(token::RIGHTPAREN);
    consume(token::RIGHTPAREN);
    if (name == "\\left-assoc") {
      sptr<kore_pattern> accum = pats[0];
      for (auto i = 1U; i < pats.size(); i++) {
        sptr<kore_composite_pattern> newAccum
            = kore_composite_pattern::Create(pat->getConstructor());
        newAccum->addArgument(accum);
        newAccum->addArgument(pats[i]);
        accum = newAccum;
      }
      return accum;
    }
    sptr<kore_pattern> accum = pats[pats.size() - 1];
    for (int i = pats.size() - 2; i >= 0; i--) {
      sptr<kore_composite_pattern> newAccum
          = kore_composite_pattern::Create(pat->getConstructor());
      newAccum->addArgument(pats[i]);
      newAccum->addArgument(accum);
      accum = newAccum;
    }
    return accum;
  }
  auto result = _applicationPattern(name);
  if (name == "\\or") {
    if (result->getArguments().empty()) {
      auto pat = kore_composite_pattern::Create("\\bottom");
      pat->getConstructor()->addArgument(
          result->getConstructor()->getFormalArguments()[0]);
      pat->getConstructor()->initPatternArguments();
      return pat;
    }
    if (result->getArguments().size() == 1) {
      return result->getArguments()[0];
    }
  } else if (name == "\\and") {
    if (result->getArguments().empty()) {
      auto pat = kore_composite_pattern::Create("\\top");
      pat->getConstructor()->addArgument(
          result->getConstructor()->getFormalArguments()[0]);
      pat->getConstructor()->initPatternArguments();
      return pat;
    }
    if (result->getArguments().size() == 1) {
      return result->getArguments()[0];
    }
  }
  return result;
}

ptr<kore_composite_pattern>
kore_parser::_applicationPattern(std::string const &name) {
  consume(token::LEFTBRACE);
  auto pat = kore_composite_pattern::Create(name);
  sorts(pat->getConstructor());
  pat->getConstructor()->initPatternArguments();
  consume(token::RIGHTBRACE);
  consume(token::LEFTPAREN);
  patterns(pat.get());
  consume(token::RIGHTPAREN);
  return pat;
}

void kore_parser::patterns(kore_composite_pattern *node) {
  if (peek() == token::RIGHTPAREN) {
    return;
  }
  patternsNE(node);
}

void kore_parser::patternsNE(kore_composite_pattern *node) {
  auto pat = _pattern();
  node->addArgument(pat);
  while (peek() == token::COMMA) {
    consume(token::COMMA);
    pat = _pattern();
    node->addArgument(pat);
  }
}

void kore_parser::patterns(std::vector<sptr<kore_pattern>> &node) {
  if (peek() == token::RIGHTPAREN) {
    return;
  }
  patternsNE(node);
}

void kore_parser::patternsNE(std::vector<sptr<kore_pattern>> &node) {
  auto pat = _pattern();
  node.push_back(pat);
  while (peek() == token::COMMA) {
    consume(token::COMMA);
    pat = _pattern();
    node.push_back(pat);
  }
}

} // namespace kllvm::parser
