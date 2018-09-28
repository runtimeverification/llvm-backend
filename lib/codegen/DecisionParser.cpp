#include "kllvm/codegen/DecisionParser.h"

#include <yaml-cpp/yaml.h>

#include <stack>
#include <iostream>

namespace kllvm {

class DTPreprocessor {
private:
  std::map<std::vector<int>, std::string> occurrences;
  const llvm::StringMap<KOREObjectSymbol *> &syms;
  const llvm::StringMap<KOREObjectCompositeSort *> &sorts;
  int counter;
  KOREObjectSymbol *dv;

  enum Kind {
    Switch, SwitchLiteral, CheckNull, MakeLiteral, MakePattern, Function, Leaf, Fail, Swap
  };

  static Kind getKind(YAML::Node node) {
    if (node.IsScalar()) return Fail;
    if (node["isnull"]) return CheckNull;
    if (node["hook"]) return MakeLiteral;
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
      int numSubjects, 
      const llvm::StringMap<KOREObjectSymbol *> &syms,
      const llvm::StringMap<KOREObjectCompositeSort *> &sorts)
      : syms(syms), sorts(sorts), counter(0) {
    for (int i = numSubjects - 1; i >= 0; --i) {
      std::string name = "subject" + std::to_string(i);
      occurrences[{i+1}] = name;
    }
    dv = KOREObjectSymbol::Create("\\dv");
  }

  DecisionNode *swap(YAML::Node node) {
    auto result = (*this)(node["swap"][1]);
    return result;
  }

  DecisionNode *function(YAML::Node node) {
    std::string function = node["function"].as<std::string>();
    std::string hookName = node["sort"].as<std::string>();
    ValueType cat = KOREObjectCompositeSort::getCategory(hookName);

    std::string binding = "_" + std::to_string(counter++);
    occurrences[node["occurrence"].as<std::vector<int>>()] = binding;

    auto child = (*this)(node["next"]); 

    auto result = FunctionNode::Create(binding, function, child, cat);
    
    YAML::Node vars = node["args"];
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto var = *iter;
      auto occurrence = var.as<std::vector<int>>();
      if (occurrence.size() == 2 && occurrence[1] == -1) {
        result->addBinding(std::to_string(occurrence[0]));
      } else {
        result->addBinding(occurrences[occurrence]);
      }
    }
    return result;
  }

  DecisionNode *makeLiteral(YAML::Node node) {
    std::string hookName = node["hook"].as<std::string>();
    std::string literal = node["literal"].as<std::string>();
    ValueType cat = KOREObjectCompositeSort::getCategory(hookName);

    std::string name = "_" + std::to_string(counter++);
    occurrences[node["occurrence"].as<std::vector<int>>()] = name;

    auto child = (*this)(node["next"]); 
    
    return MakeLiteralNode::Create(name, cat, literal, child);
  }

  KOREObjectPattern *parsePattern(YAML::Node node) {
    if (node["occurrence"]) {
      return KOREObjectVariablePattern::Create(occurrences[node["occurrence"].as<std::vector<int>>()], sorts.lookup(node["hook"].as<std::string>()));
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
        pat->addArgument(parsePattern(child));
      }
      return pat;
    }
  }

  DecisionNode *makePattern(YAML::Node node) {
    std::string name = "_" + std::to_string(counter++);
    occurrences[node["occurrence"].as<std::vector<int>>()] = name;

    KOREObjectPattern *pat = parsePattern(node["pattern"]);

    auto child = (*this)(node["next"]);

    return MakePatternNode::Create(name, pat, child);
  }

  DecisionNode *switchCase(Kind kind, YAML::Node node) {
    YAML::Node list = node["specializations"];
    auto occurrence = node["occurrence"].as<std::vector<int>>();
    std::string name = occurrences[occurrence];
    auto result = SwitchNode::Create(name, kind == CheckNull);
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      auto _case = *iter;
      auto copy = occurrences;
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
          std::string binding = "_" + std::to_string(counter++);
          bindings.push_back(binding);
          auto newOccurrence = occurrence;
          newOccurrence.insert(newOccurrence.begin(), i);
          occurrences[newOccurrence] = binding;
        }
      }
      DecisionNode *child = (*this)(_case[1]);
      occurrences = copy;
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
      auto copy = occurrences;
      DecisionNode *child = (*this)(_case);
      occurrences = copy;
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
      result->addBinding(occurrences[occurrence]);
    }
    return result;
  }
 
  DecisionNode *operator()(YAML::Node node) {
    Kind kind = getKind(node);
    switch(kind) {
    case Swap:
      return swap(node);
    case Fail:
      return FailNode::get();
    case Function:
      return function(node);
    case MakeLiteral:
      return makeLiteral(node);
    case MakePattern:
      return makePattern(node);
    case SwitchLiteral:
    case Switch:
    case CheckNull:
      return switchCase(kind, node);
    case Leaf:
      return leaf(node);
    }    
  }
};

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts) {
  YAML::Node root = YAML::Load(yaml);
  return DTPreprocessor(numSubjects, syms, sorts)(root);
}

DecisionNode *parseYamlDecisionTree(std::string filename, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts) {
  YAML::Node root = YAML::LoadFile(filename);
  return DTPreprocessor(numSubjects, syms, sorts)(root);
}

}
