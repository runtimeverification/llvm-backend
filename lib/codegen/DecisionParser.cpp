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

class DTPreprocessor {
private:
  std::map<yaml_node_t *, DecisionNode *> uniqueNodes;
  const std::map<std::string, KORESymbol *> &syms;
  const std::map<ValueType, sptr<KORECompositeSort>> &sorts;
  KORESymbol *dv;
  yaml_document_t *doc;
  llvm::Module *mod;

  enum Kind {
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

  Kind getKind(yaml_node_t *node) {
    if (node->type == YAML_SCALAR_NODE)
      return Fail;
    if (get(node, "collection"))
      return MakeIterator;
    if (get(node, "iterator"))
      return IterNext;
    if (get(node, "isnull"))
      return CheckNull;
    if (get(node, "pattern"))
      return MakePattern;
    if (get(node, "bitwidth"))
      return SwitchLiteral;
    if (get(node, "specializations"))
      return Switch;
    if (get(node, "action"))
      return Leaf;
    if (get(node, "function"))
      return Function;
    throw node;
  }

public:
  yaml_node_t *get(yaml_node_t *node, std::string name) {
    yaml_node_pair_t *entry;
    for (entry = node->data.mapping.pairs.start;
         entry < node->data.mapping.pairs.top; ++entry) {
      yaml_node_t *key = yaml_document_get_node(doc, entry->key);
      if (name == (char *)key->data.scalar.value) {
        return yaml_document_get_node(doc, entry->value);
      }
    }
    return nullptr;
  }

  yaml_node_t *get(yaml_node_t *node, size_t off) {
    return yaml_document_get_node(doc, node->data.sequence.items.start[off]);
  }

  std::string str(yaml_node_t *node) {
    return std::string(
        (char *)node->data.scalar.value, node->data.scalar.length);
  }

  std::vector<std::string> vec(yaml_node_t *node) {
    std::vector<std::string> result;
    yaml_node_item_t *entry;
    for (entry = node->data.sequence.items.start;
         entry < node->data.sequence.items.top; ++entry) {
      result.push_back(str(yaml_document_get_node(doc, *entry)));
    }
    return result;
  }

  DTPreprocessor(
      const std::map<std::string, KORESymbol *> &syms,
      const std::map<ValueType, sptr<KORECompositeSort>> &sorts,
      llvm::Module *mod, yaml_document_t *doc)
      : syms(syms)
      , sorts(sorts)
      , doc(doc)
      , mod(mod) {
    dv = KORESymbol::Create("\\dv").release();
  }

  std::string to_string(std::vector<std::string> occurrence) {
    std::string result = "";
    for (std::string i : occurrence) {
      result.push_back('_');
      result += i;
    }
    return result;
  }

  DecisionNode *function(yaml_node_t *node) {
    std::string function = str(get(node, "function"));
    std::string hookName = str(get(node, "sort"));
    ValueType cat = KORECompositeSort::getCategory(hookName);

    std::string binding = to_string(vec(get(node, "occurrence")));

    auto child = (*this)(get(node, "next"));

    auto result = FunctionNode::Create(
        binding, function, child, cat, getParamType(cat, mod));

    yaml_node_t *vars = get(node, "args");
    for (auto iter = vars->data.sequence.items.start;
         iter < vars->data.sequence.items.top; ++iter) {
      auto var = yaml_document_get_node(doc, *iter);
      auto occurrence = vec(get(var, 0));
      auto hook = str(get(var, 1));
      if (occurrence.size() == 3 && occurrence[0] == "lit"
          && occurrence[2] == "MINT.MInt 64") {
        result->addBinding(
            occurrence[1], KORECompositeSort::getCategory(hook), mod);
      } else {
        result->addBinding(
            to_string(occurrence), KORECompositeSort::getCategory(hook), mod);
      }
    }
    return result;
  }

  ptr<KOREPattern> parsePattern(
      yaml_node_t *node,
      std::vector<std::pair<std::string, llvm::Type *>> &uses) {
    if (auto o = get(node, "occurrence")) {
      std::string name;
      if (o->type == YAML_SEQUENCE_NODE) {
        name = to_string(vec(o));
      } else {
        name = str(o);
      }
      ValueType hook = KORECompositeSort::getCategory(str(get(node, "hook")));
      uses.push_back(std::make_pair(name, getParamType(hook, mod)));
      return KOREVariablePattern::Create(name, sorts.at(hook));
    } else if (get(node, "literal")) {
      auto sym = KORESymbol::Create("\\dv");
      auto hook = str(get(node, "hook"));
      auto sort = sorts.at(KORECompositeSort::getCategory(hook));
      auto lit = get(node, "literal");
      auto val = str(lit);
      if (hook == "BOOL.Bool") {
        val = val == "1" ? "true" : "false";
      }
      if (std::string((char *)lit->tag) == "tag:yaml.org,2002:binary") {
        val = base64_decode(val);
      }

      sym->addFormalArgument(sort);
      sym->addSort(sort);
      auto pat = KORECompositePattern::Create(std::move(sym));
      pat->addArgument(KOREStringPattern::Create(val));
      return pat;
    } else {
      if (!get(node, "constructor")) {
        std::cerr << node << std::endl;
        abort();
      }
      auto sym = syms.at(str(get(node, "constructor")));
      auto pat = KORECompositePattern::Create(sym);
      auto seq = get(node, "args");
      for (auto iter = seq->data.sequence.items.start;
           iter < seq->data.sequence.items.top; iter++) {
        auto child = yaml_document_get_node(doc, *iter);
        pat->addArgument(parsePattern(child, uses));
      }
      return pat;
    }
  }

  DecisionNode *makePattern(yaml_node_t *node) {
    std::string name = to_string(vec(get(node, "occurrence")));
    llvm::Type *type = getParamType(
        KORECompositeSort::getCategory(str(get(node, "sort"))), mod);

    std::vector<std::pair<std::string, llvm::Type *>> uses;

    ptr<KOREPattern> pat = parsePattern(get(node, "pattern"), uses);

    auto child = (*this)(get(node, "next"));

    return MakePatternNode::Create(name, type, pat.release(), uses, child);
  }

  DecisionNode *makeIterator(yaml_node_t *node) {
    std::string name = to_string(vec(get(node, "collection")));
    llvm::Type *type = getParamType(
        KORECompositeSort::getCategory(str(get(node, "sort"))), mod);
    std::string function = str(get(node, "function"));
    auto child = (*this)(get(node, "next"));

    return MakeIteratorNode::Create(
        name, type, name + "_iter",
        llvm::PointerType::getUnqual(
            llvm::StructType::getTypeByName(mod->getContext(), "iter")),
        function, child);
  }

  DecisionNode *iterNext(yaml_node_t *node) {
    std::string iterator = to_string(vec(get(node, "iterator"))) + "_iter";
    std::string name = to_string(vec(get(node, "binding")));
    llvm::Type *type = getParamType(
        KORECompositeSort::getCategory(str(get(node, "sort"))), mod);
    std::string function = str(get(node, "function"));
    auto child = (*this)(get(node, "next"));

    return IterNextNode::Create(
        iterator,
        llvm::PointerType::getUnqual(
            llvm::StructType::getTypeByName(mod->getContext(), "iter")),
        name, type, function, child);
  }

  DecisionNode *switchCase(Kind kind, yaml_node_t *node) {
    yaml_node_t *list = get(node, "specializations");
    auto occurrence = vec(get(node, "occurrence"));
    std::string name = to_string(occurrence);
    llvm::Type *type = getParamType(
        KORECompositeSort::getCategory(str(get(node, "sort"))), mod);
    auto result = SwitchNode::Create(name, type, kind == CheckNull);
    for (auto iter = list->data.sequence.items.start;
         iter < list->data.sequence.items.top; ++iter) {
      auto _case = yaml_document_get_node(doc, *iter);
      std::vector<std::pair<std::string, llvm::Type *>> bindings;
      KORESymbol *symbol;
      if (kind == SwitchLiteral || kind == CheckNull) {
        symbol = dv;
      } else {
        std::string symName = str(get(_case, 0));
        symbol = syms.at(symName);
        if (!symbol) {
          std::cerr << symName << std::endl;
          abort();
        }
        for (unsigned i = 0; i < symbol->getArguments().size(); ++i) {
          auto newOccurrence = occurrence;
          newOccurrence.insert(newOccurrence.begin(), std::to_string(i));
          std::string binding = to_string(newOccurrence);
          std::string hook = str(get(get(_case, 2), i));
          bindings.push_back(std::make_pair(
              binding,
              getParamType(KORECompositeSort::getCategory(hook), mod)));
        }
      }
      DecisionNode *child = (*this)(get(_case, 1));
      switch (kind) {
      case SwitchLiteral: {
        unsigned bitwidth = stoi(str(get(node, "bitwidth")));
        result->addCase({symbol, {bitwidth, str(get(_case, 0)), 10}, child});
        break;
      }
      case Switch: result->addCase({symbol, bindings, child}); break;
      case CheckNull:
        result->addCase({symbol, {1, str(get(_case, 0)), 10}, child});
        break;
      default: assert(false && "not reachable"); abort();
      }
    }
    auto _case = get(node, "default");
    if (_case->type != YAML_SCALAR_NODE || !str(_case).empty()) {
      DecisionNode *child = (*this)(_case);
      result->addCase(
          {nullptr, std::vector<std::pair<std::string, llvm::Type *>>{},
           child});
    }
    return result;
  }

  DecisionNode *leaf(yaml_node_t *node) {
    int action = stoi(str(get(get(node, "action"), 0)));
    std::string name = "apply_rule_" + std::to_string(action);
    if (auto next = get(node, "next")) {
      name = name + "_search";
    }
    auto result = LeafNode::Create(name);
    yaml_node_t *vars = get(get(node, "action"), 1);
    for (auto iter = vars->data.sequence.items.start;
         iter < vars->data.sequence.items.top; ++iter) {
      auto var = yaml_document_get_node(doc, *iter);
      auto occurrence = vec(get(var, 0));
      auto hook = str(get(var, 1));
      ValueType cat = KORECompositeSort::getCategory(hook);
      result->addBinding(to_string(occurrence), cat, mod);
    }
    if (auto next = get(node, "next")) {
      auto child = (*this)(next);
      result->setChild(child);
    }
    return result;
  }

  DecisionNode *operator()(yaml_node_t *node) {
    auto unique = uniqueNodes[node];
    if (unique) {
      return unique;
    }
    Kind kind = getKind(node);
    DecisionNode *ret = nullptr;
    switch (kind) {
    case Fail: ret = FailNode::get(); break;
    case Function: ret = function(node); break;
    case MakePattern: ret = makePattern(node); break;
    case SwitchLiteral:
    case Switch:
    case CheckNull: ret = switchCase(kind, node); break;
    case Leaf: ret = leaf(node); break;
    case MakeIterator: ret = makeIterator(node); break;
    case IterNext: ret = iterNext(node); break;
    }
    uniqueNodes[node] = ret;
    return ret;
  }

  PartialStep makeResiduals(yaml_node_t *residuals, DecisionNode *dt) {
    std::vector<Residual> res;
    for (auto iter = residuals->data.sequence.items.start;
         iter < residuals->data.sequence.items.top; ++iter) {
      Residual r;
      yaml_node_t *listNode = yaml_document_get_node(doc, *iter);
      r.occurrence = to_string(vec(get(listNode, 1)));
      std::vector<std::pair<std::string, llvm::Type *>> uses;
      r.pattern = parsePattern(get(listNode, 0), uses).release();
      res.push_back(r);
    }
    PartialStep retval;
    retval.dt = dt;
    retval.residuals = res;
    return retval;
  }
};

DecisionNode *parseYamlDecisionTreeFromString(
    llvm::Module *mod, std::string yaml,
    const std::map<std::string, KORESymbol *> &syms,
    const std::map<ValueType, sptr<KORECompositeSort>> &sorts) {
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
  auto result = DTPreprocessor(syms, sorts, mod, &doc)(root);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  return result;
}

DecisionNode *parseYamlDecisionTree(
    llvm::Module *mod, std::string filename,
    const std::map<std::string, KORESymbol *> &syms,
    const std::map<ValueType, sptr<KORECompositeSort>> &sorts) {
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
  auto result = DTPreprocessor(syms, sorts, mod, &doc)(root);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  fclose(f);
  return result;
}

PartialStep parseYamlSpecialDecisionTree(
    llvm::Module *mod, std::string filename,
    const std::map<std::string, KORESymbol *> &syms,
    const std::map<ValueType, sptr<KORECompositeSort>> &sorts) {
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
  auto pp = DTPreprocessor(syms, sorts, mod, &doc);
  auto dt = pp(pp.get(root, 0));
  auto result = pp.makeResiduals(pp.get(root, 1), dt);
  yaml_document_delete(&doc);
  yaml_parser_delete(&parser);
  fclose(f);
  return result;
}

} // namespace kllvm
