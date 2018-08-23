#include "kllvm/codegen/DecisionParser.h"

#include <yaml-cpp/yaml.h>

#include <stack>

namespace kllvm {

class DTPreprocessor {
private:
  std::vector<std::string> constructors;
  std::vector<std::string> parents;
  const llvm::StringMap<KOREObjectSymbol *> &syms;
  int counter;

  enum Kind {
    Switch, Function, Leaf, Fail, Swap
  };

  static Kind getKind(YAML::Node node) {
    if (node.IsScalar()) return Fail;
    if (node["specializations"]) return Switch;
    if (node["action"]) return Leaf;
    if (node["swap"]) return Swap;
    return Function;
  }

public:
  DTPreprocessor(int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms) : syms(syms), counter(0) {
    for (int i = numSubjects - 1; i >= 0; --i) {
      constructors.push_back("subject" + std::to_string(i));
    }
  }

  DecisionNode *operator()(YAML::Node node) {
    Kind kind = getKind(node);
    switch(kind) {
    case Swap: {
      int idx = node["swap"][0].as<int>();
      std::string tmp = constructors[constructors.size() - 1 - idx];
      constructors[constructors.size() - 1 - idx] = constructors[constructors.size() - 1];
      constructors[constructors.size() - 1] = tmp;
      parents.push_back("");
      auto result = (*this)(node["swap"][1]);
      parents.pop_back();
      return result;
    }
    case Fail:
      return FailNode::get();
    case Function:
      assert(false && "not implemented: functions");
    case Switch: {
      YAML::Node list = node["specializations"];
      std::string name = constructors.back();
      constructors.pop_back();
      auto result = SwitchNode::Create(name);
      parents.push_back(name);
      for (auto iter = list.begin(); iter != list.end(); ++iter) {
        auto _case = *iter;
        KOREObjectSymbol *symbol = syms.lookup(_case[0].as<std::string>());
        std::vector<std::string> copy = constructors;
	std::vector<std::string> bindings;
        for (int i = 0; i < symbol->getArguments().size(); ++i) {
          std::string binding = "_" + std::to_string(counter++);
          constructors.push_back(binding);
	  bindings.push_back(binding);
        }
	std::reverse(constructors.end()-symbol->getArguments().size(), constructors.end());
        DecisionNode *child = (*this)(_case[1]);
        constructors = copy;
        result->addCase({symbol, bindings, child});
      }
      auto _case = node["default"];
      if (!_case.IsNull()) {
        std::vector<std::string> copy = constructors;
        DecisionNode *child = (*this)(_case);
        constructors = copy;
        result->addCase({nullptr, std::vector<std::string>{}, child});
      }
      parents.pop_back();
      return result;
    }
    case Leaf: {
      int action = node["action"].as<int>();
      std::string name = "apply_rule_" + std::to_string(action);
      auto result = LeafNode::Create(name);
      YAML::Node vars = node["variables"];
      for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
        auto var = *iter;
        int idx1 = var[0].as<int>();
        int idx2 = var[1].as<int>();
        if (idx1 == 0) {
          result->addBinding(constructors[constructors.size()-1-idx2]);
	} else {
          result->addBinding(parents[parents.size()-idx1]);
        }
      }
      return result;
    }
    }     
  }
};

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms) {
  YAML::Node root = YAML::Load(yaml);
  return DTPreprocessor(numSubjects, syms)(root);
}

DecisionNode *parseYamlDecisionTree(std::string filename, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms) {
  YAML::Node root = YAML::LoadFile(filename);
  return DTPreprocessor(numSubjects, syms)(root);
}

}
