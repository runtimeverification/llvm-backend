#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/Util.h"

#include <base64/base64.h>

#include <fmt/format.h>

#include <yaml.h>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

namespace kllvm {

class dt_preprocessor {
private:
  std::map<yaml_node_t *, decision_node *> unique_nodes_{};
  std::map<std::string, kore_symbol *> const &syms_;
  std::map<value_type, sptr<kore_composite_sort>> const &sorts_;
  kore_symbol *dv_;
  yaml_document_t *doc_;
  llvm::Module *mod_;

  enum kind {
    Switch,
    SwitchLiteral,
    CheckNull,
    MakePattern,
    Function,
    MakeIterator,
    IterNext,
    Leaf,
    Fail
  };

  kind get_kind(yaml_node_t *node) {
    if (node->type == YAML_SCALAR_NODE) {
      return Fail;
    }
    if (get(node, "collection")) {
      return MakeIterator;
    }
    if (get(node, "iterator")) {
      return IterNext;
    }
    if (get(node, "isnull")) {
      return CheckNull;
    }
    if (get(node, "pattern")) {
      return MakePattern;
    }
    if (get(node, "bitwidth")) {
      return SwitchLiteral;
    }
    if (get(node, "specializations")) {
      return Switch;
    }
    if (get(node, "action")) {
      return Leaf;
    }
    if (get(node, "function")) {
      return Function;
    }
    throw *node;
  }

public:
  yaml_node_t *get(yaml_node_t *node, std::string const &name) {
    yaml_node_pair_t *entry = nullptr;
    for (entry = node->data.mapping.pairs.start;
         entry < node->data.mapping.pairs.top; ++entry) {
      yaml_node_t *key = yaml_document_get_node(doc_, entry->key);
      if (name == (char *)key->data.scalar.value) {
        return yaml_document_get_node(doc_, entry->value);
      }
    }
    return nullptr;
  }

  yaml_node_t *get(yaml_node_t *node, size_t off) {
    return yaml_document_get_node(doc_, node->data.sequence.items.start[off]);
  }

  static std::string str(yaml_node_t *node) {
    return {(char *)node->data.scalar.value, node->data.scalar.length};
  }

  std::vector<std::string> vec(yaml_node_t *node) {
    std::vector<std::string> result;
    yaml_node_item_t *entry = nullptr;
    for (entry = node->data.sequence.items.start;
         entry < node->data.sequence.items.top; ++entry) {
      result.push_back(str(yaml_document_get_node(doc_, *entry)));
    }
    return result;
  }

  dt_preprocessor(
      std::map<std::string, kore_symbol *> const &syms,
      std::map<value_type, sptr<kore_composite_sort>> const &sorts,
      llvm::Module *mod, yaml_document_t *doc)
      : syms_(syms)
      , sorts_(sorts)
      , dv_(kore_symbol::create("\\dv").release())
      , doc_(doc)
      , mod_(mod) { }

  static std::string to_string(std::vector<std::string> const &occurrence) {
    std::string result;
    for (auto const &i : occurrence) {
      result.push_back('_');
      result += i;
    }
    return result;
  }

  decision_node *function(yaml_node_t *node) {
    std::string function = str(get(node, "function"));
    std::string hook_name = str(get(node, "sort"));
    value_type cat = kore_composite_sort::get_category(hook_name);

    std::string binding = to_string(vec(get(node, "occurrence")));

    auto *child = (*this)(get(node, "next"));

    auto *result = function_node::create(
        binding, function, child, cat, getParamType(cat, mod_));

    yaml_node_t *vars = get(node, "args");
    for (auto *iter = vars->data.sequence.items.start;
         iter < vars->data.sequence.items.top; ++iter) {
      auto *var = yaml_document_get_node(doc_, *iter);
      auto occurrence = vec(get(var, 0));
      auto hook = str(get(var, 1));
      if (occurrence.size() == 3 && occurrence[0] == "lit"
          && occurrence[2] == "MINT.MInt 64") {
        result->add_binding(
            occurrence[1], kore_composite_sort::get_category(hook), mod_);
      } else {
        result->add_binding(
            to_string(occurrence), kore_composite_sort::get_category(hook), mod_);
      }
    }
    return result;
  }

  ptr<kore_pattern> parse_pattern(
      yaml_node_t *node,
      std::vector<std::pair<std::string, llvm::Type *>> &uses) {
    if (auto *o = get(node, "occurrence")) {
      std::string name;
      if (o->type == YAML_SEQUENCE_NODE) {
        name = to_string(vec(o));
      } else {
        name = str(o);
      }
      value_type hook = kore_composite_sort::get_category(str(get(node, "hook")));
      uses.emplace_back(name, getParamType(hook, mod_));
      return kore_variable_pattern::create(name, sorts_.at(hook));
    }

    if (get(node, "literal")) {
      auto sym = kore_symbol::create("\\dv");
      auto hook = str(get(node, "hook"));
      auto sort = sorts_.at(kore_composite_sort::get_category(hook));
      auto *lit = get(node, "literal");
      auto val = str(lit);
      if (hook == "BOOL.Bool") {
        val = val == "1" ? "true" : "false";
      }
      if (std::string((char *)lit->tag) == "tag:yaml.org,2002:binary") {
        val = base64_decode(val);
      }

      sym->add_formal_argument(sort);
      sym->add_sort(sort);
      auto pat = kore_composite_pattern::create(std::move(sym));
      pat->add_argument(kore_string_pattern::create(val));
      return pat;
    }

    if (!get(node, "constructor")) {
      std::cerr << node << std::endl;
      abort();
    }

    auto *sym = syms_.at(str(get(node, "constructor")));
    auto pat = kore_composite_pattern::create(sym);
    auto *seq = get(node, "args");
    for (auto *iter = seq->data.sequence.items.start;
         iter < seq->data.sequence.items.top; iter++) {
      auto *child = yaml_document_get_node(doc_, *iter);
      pat->add_argument(parse_pattern(child, uses));
    }
    return pat;
  }

  decision_node *make_pattern(yaml_node_t *node) {
    std::string name = to_string(vec(get(node, "occurrence")));
    llvm::Type *type = getParamType(
        kore_composite_sort::get_category(str(get(node, "sort"))), mod_);

    std::vector<std::pair<std::string, llvm::Type *>> uses;

    ptr<kore_pattern> pat = parse_pattern(get(node, "pattern"), uses);

    auto *child = (*this)(get(node, "next"));

    return make_pattern_node::create(name, type, pat.release(), uses, child);
  }

  decision_node *make_iterator(yaml_node_t *node) {
    auto collection = to_string(vec(get(node, "collection")));
    auto *type = getParamType(
        kore_composite_sort::get_category(str(get(node, "sort"))), mod_);
    auto hook_name = str(get(node, "function"));
    auto *child = (*this)(get(node, "next"));

    return make_iterator_node::create(
        collection, type, collection + "_iter",
        llvm::PointerType::getUnqual(
            llvm::StructType::getTypeByName(mod_->getContext(), "iter")),
        hook_name, child);
  }

  decision_node *iter_next(yaml_node_t *node) {
    auto iterator = to_string(vec(get(node, "iterator"))) + "_iter";
    auto binding = to_string(vec(get(node, "binding")));
    auto *type = getParamType(
        kore_composite_sort::get_category(str(get(node, "sort"))), mod_);
    auto function = str(get(node, "function"));
    auto *child = (*this)(get(node, "next"));

    return iter_next_node::create(
        iterator,
        llvm::PointerType::getUnqual(
            llvm::StructType::getTypeByName(mod_->getContext(), "iter")),
        binding, type, function, child);
  }

  decision_node *switch_case(kind kind, yaml_node_t *node) {
    yaml_node_t *list = get(node, "specializations");
    auto occurrence = vec(get(node, "occurrence"));
    std::string name = to_string(occurrence);
    llvm::Type *type = getParamType(
        kore_composite_sort::get_category(str(get(node, "sort"))), mod_);
    auto *result = switch_node::create(name, type, kind == CheckNull);
    for (auto *iter = list->data.sequence.items.start;
         iter < list->data.sequence.items.top; ++iter) {
      auto *_case = yaml_document_get_node(doc_, *iter);
      std::vector<std::pair<std::string, llvm::Type *>> bindings;
      kore_symbol *symbol = nullptr;
      if (kind == SwitchLiteral || kind == CheckNull) {
        symbol = dv_;
      } else {
        std::string sym_name = str(get(_case, 0));
        symbol = syms_.at(sym_name);
        if (!symbol) {
          std::cerr << sym_name << std::endl;
          abort();
        }
        for (unsigned i = 0; i < symbol->get_arguments().size(); ++i) {
          auto new_occurrence = occurrence;
          new_occurrence.insert(new_occurrence.begin(), std::to_string(i));
          std::string binding = to_string(new_occurrence);
          std::string hook = str(get(get(_case, 2), i));
          bindings.emplace_back(
              binding, getParamType(kore_composite_sort::get_category(hook), mod_));
        }
      }
      decision_node *child = (*this)(get(_case, 1));
      switch (kind) {
      case SwitchLiteral: {
        unsigned bitwidth = stoi(str(get(node, "bitwidth")));
        result->add_case({symbol, {bitwidth, str(get(_case, 0)), 10}, child});
        break;
      }
      case Switch: result->add_case({symbol, bindings, child}); break;
      case CheckNull:
        result->add_case({symbol, {1, str(get(_case, 0)), 10}, child});
        break;
      default: assert(false && "not reachable"); abort();
      }
    }
    auto *_case = get(node, "default");
    if (_case->type != YAML_SCALAR_NODE || !str(_case).empty()) {
      decision_node *child = (*this)(_case);
      result->add_case(
          {nullptr, std::vector<std::pair<std::string, llvm::Type *>>{},
           child});
    }
    return result;
  }

  decision_node *leaf(yaml_node_t *node) {
    int action = stoi(str(get(get(node, "action"), 0)));
    std::string name = "apply_rule_" + std::to_string(action);
    if (auto *next = get(node, "next")) {
      name = name + "_search";
    }
    auto *result = leaf_node::create(name);
    yaml_node_t *vars = get(get(node, "action"), 1);
    for (auto *iter = vars->data.sequence.items.start;
         iter < vars->data.sequence.items.top; ++iter) {
      auto *var = yaml_document_get_node(doc_, *iter);
      auto occurrence = vec(get(var, 0));
      auto hook = str(get(var, 1));
      value_type cat = kore_composite_sort::get_category(hook);
      result->add_binding(to_string(occurrence), cat, mod_);
    }
    if (auto *next = get(node, "next")) {
      auto *child = (*this)(next);
      result->set_child(child);
    }
    return result;
  }

  decision_node *operator()(yaml_node_t *node) {
    auto *unique = unique_nodes_[node];
    if (unique) {
      return unique;
    }
    kind kind = get_kind(node);
    decision_node *ret = nullptr;
    switch (kind) {
    case Fail: ret = fail_node::get(); break;
    case Function: ret = function(node); break;
    case MakePattern: ret = make_pattern(node); break;
    case SwitchLiteral:
    case Switch:
    case CheckNull: ret = switch_case(kind, node); break;
    case Leaf: ret = leaf(node); break;
    case MakeIterator: ret = make_iterator(node); break;
    case IterNext: ret = iter_next(node); break;
    }
    unique_nodes_[node] = ret;
    return ret;
  }

  partial_step makeresiduals(yaml_node_t *residuals, decision_node *dt) {
    std::vector<residual> res;
    for (auto *iter = residuals->data.sequence.items.start;
         iter < residuals->data.sequence.items.top; ++iter) {
      residual r;
      yaml_node_t *list_node = yaml_document_get_node(doc_, *iter);
      r.occurrence = to_string(vec(get(list_node, 1)));
      std::vector<std::pair<std::string, llvm::Type *>> uses;
      r.pattern = parse_pattern(get(list_node, 0), uses).release();
      res.push_back(r);
    }
    partial_step retval;
    retval.dt = dt;
    retval.residuals = res;
    return retval;
  }
};

decision_node *parseYamldecisionTreeFromString(
    llvm::Module *mod, std::string const &yaml,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts) {
  yaml_parser_t parser;
  yaml_document_t doc;
  yaml_parser_initialize(&parser);
  yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);
  yaml_parser_set_input_string(
      &parser, (unsigned char *)yaml.c_str(), yaml.size());
  yaml_parser_load(&parser, &doc);
  yaml_node_t *root = yaml_document_get_root_node(&doc);
  if (!root) {
    throw std::runtime_error("Failed to parse decision tree from YAML string");
  }
  auto *result = dt_preprocessor(syms, sorts, mod, &doc)(root);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  return result;
}

decision_node *parseYamldecisionTree(
    llvm::Module *mod, std::string const &filename,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts) {
  yaml_parser_t parser;
  yaml_document_t doc;
  yaml_parser_initialize(&parser);
  FILE *f = fopen(filename.c_str(), "rb");
  yaml_parser_set_input_file(&parser, f);
  yaml_parser_load(&parser, &doc);
  yaml_node_t *root = yaml_document_get_root_node(&doc);
  if (!root) {
    throw std::runtime_error(
        fmt::format("Failed to parse decision tree from file: {}", filename));
  }
  auto *result = dt_preprocessor(syms, sorts, mod, &doc)(root);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  fclose(f);
  return result;
}

partial_step parseYamlSpecialdecisionTree(
    llvm::Module *mod, std::string const &filename,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts) {
  yaml_parser_t parser;
  yaml_document_t doc;
  yaml_parser_initialize(&parser);
  FILE *f = fopen(filename.c_str(), "rb");
  yaml_parser_set_input_file(&parser, f);
  yaml_parser_load(&parser, &doc);
  yaml_node_t *root = yaml_document_get_root_node(&doc);
  if (!root) {
    throw std::runtime_error(fmt::format(
        "Failed to parse special decision tree from file: {}", filename));
  }
  auto pp = dt_preprocessor(syms, sorts, mod, &doc);
  auto *dt = pp(pp.get(root, 0));
  auto result = pp.makeresiduals(pp.get(root, 1), dt);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  fclose(f);
  return result;
}

} // namespace kllvm
