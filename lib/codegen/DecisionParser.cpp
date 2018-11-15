#include "kllvm/codegen/DecisionParser.h"

#include <yaml-cpp/yaml.h>

#include <stack>
#include <iostream>

namespace kllvm {

class DTPreprocessor {
private:
  // we do a giant nasty hack around the fact that YAML::Node doesn't
  // implement comparison or hashing by storing the pointer to the cached
  // DecisionNode as a scalar in a YAML node.
  YAML::Node uniqueNodes;
  const llvm::StringMap<KOREObjectSymbol *> &syms;
  const llvm::StringMap<KOREObjectCompositeSort *> &sorts;
  KOREObjectSymbol *dv;

  enum Kind {
    Switch, SwitchLiteral, CheckNull, MakePattern, Function, Leaf, Fail, Swap
  };

  static Kind getKind(YAML::Node node) {
    if (node.IsScalar()) return Fail;
    if (node["isnull"]) return CheckNull;
    if (node["pattern"]) return MakePattern;
    if (node["bitwidth"]) return SwitchLiteral;
    if (node["specializations"]) return Switch;
    if (node["action"]) return Leaf;
    if (node["swap"]) return Swap;
    if (node["function"]) return Function;
    throw node;
  }

public:
  DTPreprocessor(
      const llvm::StringMap<KOREObjectSymbol *> &syms,
      const llvm::StringMap<KOREObjectCompositeSort *> &sorts)
      : syms(syms), sorts(sorts) {
    dv = KOREObjectSymbol::Create("\\dv");
  }

  std::string to_string(std::vector<int> occurrence) {
    std::string result = "";
    for (int i : occurrence) {
      result.push_back('_');
      result += std::to_string(i);
    }
    return result;
  }

  DecisionNode *swap(YAML::Node node) {
    auto result = (*this)(node["swap"][1]);
    return result;
  }

  DecisionNode *function(YAML::Node node) {
    std::string function = node["function"].as<std::string>();
    std::string hookName = node["sort"].as<std::string>();
    ValueType cat = KOREObjectCompositeSort::getCategory(hookName);

    std::string binding = to_string(node["occurrence"].as<std::vector<int>>());

    auto child = (*this)(node["next"]); 

    auto result = FunctionNode::Create(binding, function, child, cat);
    
    YAML::Node vars = node["args"];
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto var = *iter;
      auto occurrence = var.as<std::vector<int>>();
      if (occurrence.size() == 2 && occurrence[1] == -1) {
        result->addBinding(std::to_string(occurrence[0]));
      } else {
        result->addBinding(to_string(occurrence));
      }
    }
    return result;
  }

  KOREObjectPattern *parsePattern(YAML::Node node, std::vector<std::string> &uses) {
    if (node["occurrence"]) {
      std::string name = to_string(node["occurrence"].as<std::vector<int>>());
      uses.push_back(name);
      return KOREObjectVariablePattern::Create(name, sorts.lookup(node["hook"].as<std::string>()));
    } else if (node["literal"]) {
      auto sym = KOREObjectSymbol::Create("\\dv");
      sym->addFormalArgument(sorts.lookup(node["hook"].as<std::string>()));
      auto pat = KOREObjectCompositePattern::Create(sym);
      pat->addArgument(KOREMetaStringPattern::Create(node["literal"].as<std::string>()));
      return pat;
    } else {
      auto sym = syms.lookup(node["constructor"].as<std::string>());
      auto pat = KOREObjectCompositePattern::Create(sym);
      for (auto child : node["args"]) {
        pat->addArgument(parsePattern(child, uses));
      }
      return pat;
    }
  }

  DecisionNode *makePattern(YAML::Node node) {
    std::string name = to_string(node["occurrence"].as<std::vector<int>>());

    std::vector<std::string> uses;

    KOREObjectPattern *pat = parsePattern(node["pattern"], uses);

    auto child = (*this)(node["next"]);

    return MakePatternNode::Create(name, pat, uses, child);
  }

  DecisionNode *switchCase(Kind kind, YAML::Node node) {
    YAML::Node list = node["specializations"];
    auto occurrence = node["occurrence"].as<std::vector<int>>();
    std::string name = to_string(occurrence);
    auto result = SwitchNode::Create(name, kind == CheckNull);
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      auto _case = *iter;
      std::vector<std::string> bindings;
      KOREObjectSymbol *symbol;
      if (kind == SwitchLiteral || kind == CheckNull) {
        symbol = dv;
      } else {
        std::string symName = _case[0].as<std::string>();
        symbol = syms.lookup(symName);
        if (!symbol) {
          std::cerr << symName << std::endl;
          abort();
        }
        for (unsigned i = 0; i < symbol->getArguments().size(); ++i) {
          auto newOccurrence = occurrence;
          newOccurrence.insert(newOccurrence.begin(), i);
          std::string binding = to_string(newOccurrence);
          bindings.push_back(binding);
        }
      }
      DecisionNode *child = (*this)(_case[1]);
      switch (kind) {
      case SwitchLiteral: {
        unsigned bitwidth = node["bitwidth"].as<unsigned>();
        result->addCase({symbol, {bitwidth, _case[0].as<std::string>(), 10}, child}); 
        break;
      }
      case Switch:
        result->addCase({symbol, bindings, child});
        break;
      case CheckNull:
        result->addCase({symbol, {1, _case[0].as<std::string>(), 10}, child}); 
        break;
      default:
        assert(false && "not reachable");
        abort();
      }
    }
    auto _case = node["default"];
    if (!_case.IsNull()) {
      DecisionNode *child = (*this)(_case);
      result->addCase({nullptr, std::vector<std::string>{}, child});
    }
    return result;
  }

  DecisionNode *leaf(YAML::Node node) {
    int action = node["action"][0].as<int>();
    std::string name = "apply_rule_" + std::to_string(action);
    auto result = LeafNode::Create(name);
    YAML::Node vars = node["action"][1];
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto var = *iter;
      auto occurrence = var.as<std::vector<int>>();
      result->addBinding(to_string(occurrence));
    }
    return result;
  }

  DecisionNode *operator()(YAML::Node node) {
    auto unique = uniqueNodes[node];
    if (unique) {
      return (DecisionNode *)unique.as<uintptr_t>();
    }
    Kind kind = getKind(node);
    DecisionNode * ret = nullptr;
    switch(kind) {
    case Swap:
      ret = swap(node); break;
    case Fail:
      ret = FailNode::get(); break;
    case Function:
      ret = function(node); break;
    case MakePattern:
      ret = makePattern(node); break;
    case SwitchLiteral:
    case Switch:
    case CheckNull:
      ret = switchCase(kind, node); break;
    case Leaf:
      ret = leaf(node); break;
    }
    uniqueNodes[node] = (uintptr_t)ret;
    return ret;
  }
};

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts) {
  YAML::Node root = YAML::Load(yaml);
  return DTPreprocessor(syms, sorts)(root);
}

DecisionNode *parseYamlDecisionTree(std::string filename, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts) {
  YAML::Node root = YAML::LoadFile(filename);
  return DTPreprocessor(syms, sorts)(root);
}

}
