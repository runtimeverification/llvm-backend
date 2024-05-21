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
  case token::Empty: abort();
  case token::Module: return "module";
  case token::EndModule: return "endmodule";
  case token::Import: return "import";
  case token::Sort: return "sort";
  case token::Symbol: return "symbol";
  case token::Where: return "where";
  case token::Alias: return "alias";
  case token::Axiom: return "axiom";
  case token::Claim: return "claim";
  case token::HookedSort: return "hooked-sort";
  case token::HookedSymbol: return "hooked-symbol";
  case token::Colon: return ":";
  case token::LeftBrace: return "{";
  case token::RightBrace: return "}";
  case token::LeftBracket: return "[";
  case token::RightBracket: return "]";
  case token::LeftParen: return "(";
  case token::RightParen: return ")";
  case token::Comma: return ",";
  case token::ColonEqual: return ":=";
  case token::Id: return "<identifier>";
  case token::String: return "<string>";
  case token::TokenEof: return "<EOF>";
  default: abort();
  }
}

std::string kore_parser::consume(token next) {
  std::string data;
  token actual = token::Empty;
  if (buffer_.tok == token::Empty) {
    actual = scanner_.yylex(&data, &loc_);
  } else {
    actual = buffer_.tok;
    data = buffer_.data;
    buffer_.tok = token::Empty;
  }
  if (actual == next) {
    return data;
  }
  error(loc_, "Expected: " + str(next) + " Actual: " + str(actual));
}

token kore_parser::peek() {
  std::string data;
  if (buffer_.tok == token::Empty) {
    buffer_.tok = scanner_.yylex(&data, &loc_);
    buffer_.data = data;
  }
  return buffer_.tok;
}

ptr<kore_definition> kore_parser::definition() {
  consume(token::LeftBracket);
  auto result = kore_definition::create();
  attributes(result.get());
  consume(token::RightBracket);
  modules(result.get());
  consume(token::TokenEof);
  return result;
}

sptr<kore_pattern> kore_parser::pattern() {
  auto result = pattern_internal();
  consume(token::TokenEof);
  return result;
}

template <typename Node>
void kore_parser::attributes(Node *node) {
  if (peek() == token::Id) {
    attributes_ne(node);
  }
}

template <typename Node>
void kore_parser::attributes_ne(Node *node) {
  auto pat = application_pattern_internal();
  node->attributes().add(std::move(pat));
  while (peek() == token::Comma) {
    consume(token::Comma);
    pat = application_pattern_internal();
    node->attributes().add(std::move(pat));
  }
}

void kore_parser::modules(kore_definition *node) {
  auto mod = module();
  node->add_module(std::move(mod));
  while (peek() == token::Module) {
    mod = module();
    node->add_module(std::move(mod));
  }
}

ptr<kore_module> kore_parser::module() {
  consume(token::Module);
  std::string name = consume(token::Id);
  auto mod = kore_module::create(name);
  sentences(mod.get());
  consume(token::EndModule);
  consume(token::LeftBracket);
  attributes(mod.get());
  consume(token::RightBracket);
  return mod;
}

void kore_parser::sentences(kore_module *node) {
  while (peek() != token::EndModule) {
    auto decl = sentence();
    node->add_declaration(std::move(decl));
  }
}

std::vector<ptr<kore_declaration>> kore_parser::declarations() {
  std::vector<ptr<kore_declaration>> decls;
  while (peek() != token::TokenEof) {
    auto decl = sentence();
    decls.push_back(std::move(decl));
  }
  return decls;
}

ptr<kore_declaration> kore_parser::sentence() {
  std::string name;
  token current = peek();
  switch (current) {
  case token::Import: {
    consume(token::Import);
    name = consume(token::Id);
    auto import = kore_module_import_declaration::create(name);
    consume(token::LeftBracket);
    attributes(import.get());
    consume(token::RightBracket);
    return import;
  }
  case token::Sort:
  case token::HookedSort: {
    consume(current);
    name = consume(token::Id);
    consume(token::LeftBrace);
    auto sort_decl = kore_composite_sort_declaration::create(
        name, current == token::HookedSort);
    sort_variables(sort_decl.get());
    consume(token::RightBrace);
    consume(token::LeftBracket);
    attributes(sort_decl.get());
    consume(token::RightBracket);
    return sort_decl;
  }
  case token::Symbol:
  case token::HookedSymbol: {
    consume(current);
    name = consume(token::Id);
    consume(token::LeftBrace);
    auto symbol
        = kore_symbol_declaration::create(name, current == token::HookedSymbol);
    sort_variables(symbol.get());
    consume(token::RightBrace);
    consume(token::LeftParen);
    sorts(symbol->get_symbol());
    consume(token::RightParen);
    consume(token::Colon);
    auto return_sort = sort();
    symbol->get_symbol()->add_sort(std::move(return_sort));
    consume(token::LeftBracket);
    attributes(symbol.get());
    consume(token::RightBracket);
    return symbol;
  }
  case token::Alias: {
    consume(token::Alias);
    name = consume(token::Id);
    consume(token::LeftBrace);
    auto alias = kore_alias_declaration::create(name);
    sort_variables(alias.get());
    consume(token::RightBrace);
    consume(token::LeftParen);
    sorts(alias->get_symbol());
    consume(token::RightParen);
    consume(token::Colon);
    auto return_sort = sort();
    alias->get_symbol()->add_sort(std::move(return_sort));
    consume(token::Where);
    auto variables = application_pattern_internal();
    alias->add_variables(std::move(variables));
    consume(token::ColonEqual);
    auto pat = pattern_internal();
    alias->add_pattern(std::move(pat));
    consume(token::LeftBracket);
    attributes(alias.get());
    consume(token::RightBracket);
    return alias;
  }
  case token::Axiom:
  case token::Claim: {
    consume(current);
    consume(token::LeftBrace);
    auto axiom = kore_axiom_declaration::create(current == token::Claim);
    sort_variables(axiom.get());
    consume(token::RightBrace);
    auto pat = pattern_internal();
    axiom->add_pattern(std::move(pat));
    consume(token::LeftBracket);
    attributes(axiom.get());
    consume(token::RightBracket);
    return axiom;
  }
  default:
    error(
        loc_, "Expected: [import, sort, hooked-sort, symbol, hooked-symbol, "
              "alias, axiom, claim] Actual: "
                  + str(current));
  }
}

void kore_parser::sort_variables(kore_declaration *node) {
  if (peek() == token::Id) {
    sort_variables_ne(node);
  }
}

void kore_parser::sort_variables_ne(kore_declaration *node) {
  std::string name = consume(token::Id);
  auto var = kore_sort_variable::create(name);
  node->add_object_sort_variable(var);
  while (peek() == token::Comma) {
    consume(token::Comma);
    name = consume(token::Id);
    var = kore_sort_variable::create(name);
    node->add_object_sort_variable(var);
  }
}

template <typename Node>
void kore_parser::sorts(Node *node) {
  if (peek() == token::Id) {
    sorts_ne(node);
  }
}

template <typename Node>
void kore_parser::sorts_ne(Node *node) {
  auto s = sort();
  node->add_argument(std::move(s));
  while (peek() == token::Comma) {
    consume(token::Comma);
    s = sort();
    node->add_argument(std::move(s));
  }
}

std::pair<std::string, std::vector<sptr<kore_sort>>>
kore_parser::symbol_sort_list() {
  auto ret = std::vector<sptr<kore_sort>>{};

  auto id = consume(token::Id);
  consume(token::LeftBrace);

  if (peek() == token::Id) {
    ret.push_back(sort());

    while (peek() == token::Comma) {
      consume(token::Comma);
      ret.push_back(sort());
    }
  }

  consume(token::RightBrace);
  return {id, ret};
}

sptr<kore_sort> kore_parser::sort() {
  std::string name = consume(token::Id);
  if (peek() == token::LeftBrace) {
    consume(token::LeftBrace);
    auto sort = kore_composite_sort::create(name);
    sorts(sort.get());
    consume(token::RightBrace);
    return sort;
  }
  return kore_sort_variable::create(name);
}

sptr<kore_pattern> kore_parser::pattern_internal() {
  token current = peek();
  switch (current) {
  case token::Id: {
    std::string name = consume(token::Id);
    current = peek();
    switch (current) {
    case token::Colon:
      consume(token::Colon);
      return kore_variable_pattern::create(name, sort());
    case token::LeftBrace: return application_pattern(name);
    default: error(loc_, "Expected: [:, {] Actual: " + str(current));
    }
  }
  case token::String:
    return kore_string_pattern::create(consume(token::String));
  default: error(loc_, "Expected: [<id>, <string>] Actual: " + str(current));
  }
}

sptr<kore_pattern> kore_parser::application_pattern() {
  return application_pattern(consume(token::Id));
}
ptr<kore_composite_pattern> kore_parser::application_pattern_internal() {
  return application_pattern_internal(consume(token::Id));
}

sptr<kore_pattern> kore_parser::application_pattern(std::string const &name) {
  if (name == "\\left-assoc" || name == "\\right-assoc") {
    consume(token::LeftBrace);
    consume(token::RightBrace);
    consume(token::LeftParen);
    std::string symbol = consume(token::Id);
    consume(token::LeftBrace);
    auto pat = kore_composite_pattern::create(symbol);
    sorts(pat->get_constructor());
    pat->get_constructor()->init_pattern_arguments();
    consume(token::RightBrace);
    consume(token::LeftParen);
    std::vector<sptr<kore_pattern>> pats;
    patterns(pats);
    consume(token::RightParen);
    consume(token::RightParen);
    if (name == "\\left-assoc") {
      sptr<kore_pattern> accum = pats[0];
      for (auto i = 1U; i < pats.size(); i++) {
        sptr<kore_composite_pattern> new_accum
            = kore_composite_pattern::create(pat->get_constructor());
        new_accum->add_argument(accum);
        new_accum->add_argument(pats[i]);
        accum = new_accum;
      }
      return accum;
    }
    sptr<kore_pattern> accum = pats[pats.size() - 1];
    for (int i = pats.size() - 2; i >= 0; i--) {
      sptr<kore_composite_pattern> new_accum
          = kore_composite_pattern::create(pat->get_constructor());
      new_accum->add_argument(pats[i]);
      new_accum->add_argument(accum);
      accum = new_accum;
    }
    return accum;
  }
  auto result = application_pattern_internal(name);
  if (name == "\\or") {
    if (result->get_arguments().empty()) {
      auto pat = kore_composite_pattern::create("\\bottom");
      pat->get_constructor()->add_argument(
          result->get_constructor()->get_formal_arguments()[0]);
      pat->get_constructor()->init_pattern_arguments();
      return pat;
    }
    if (result->get_arguments().size() == 1) {
      return result->get_arguments()[0];
    }
  } else if (name == "\\and") {
    if (result->get_arguments().empty()) {
      auto pat = kore_composite_pattern::create("\\top");
      pat->get_constructor()->add_argument(
          result->get_constructor()->get_formal_arguments()[0]);
      pat->get_constructor()->init_pattern_arguments();
      return pat;
    }
    if (result->get_arguments().size() == 1) {
      return result->get_arguments()[0];
    }
  }
  return result;
}

ptr<kore_symbol> kore_parser::symbol() {
  std::string symbol = consume(token::Id);
  consume(token::LeftBrace);
  auto pat = kore_composite_pattern::create(symbol);
  sorts(pat->get_constructor());
  pat->get_constructor()->init_pattern_arguments();
  consume(token::RightBrace);
  return std::make_unique<kore_symbol>(*pat->get_constructor());
}

ptr<kore_composite_pattern>
kore_parser::application_pattern_internal(std::string const &name) {
  consume(token::LeftBrace);
  auto pat = kore_composite_pattern::create(name);
  sorts(pat->get_constructor());
  pat->get_constructor()->init_pattern_arguments();
  consume(token::RightBrace);
  consume(token::LeftParen);
  patterns(pat.get());
  consume(token::RightParen);
  return pat;
}

void kore_parser::patterns(kore_composite_pattern *node) {
  if (peek() == token::RightParen) {
    return;
  }
  patterns_ne(node);
}

void kore_parser::patterns_ne(kore_composite_pattern *node) {
  auto pat = pattern_internal();
  node->add_argument(pat);
  while (peek() == token::Comma) {
    consume(token::Comma);
    pat = pattern_internal();
    node->add_argument(pat);
  }
}

void kore_parser::patterns(std::vector<sptr<kore_pattern>> &node) {
  if (peek() == token::RightParen) {
    return;
  }
  patterns_ne(node);
}

void kore_parser::patterns_ne(std::vector<sptr<kore_pattern>> &node) {
  auto pat = pattern_internal();
  node.push_back(pat);
  while (peek() == token::Comma) {
    consume(token::Comma);
    pat = pattern_internal();
    node.push_back(pat);
  }
}

} // namespace kllvm::parser
