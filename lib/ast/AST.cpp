#include "kllvm/ast/AST.h"

#include <unordered_set>
#include <unordered_map>

using namespace kllvm;

size_t kllvm::hash_value(const kllvm::KORESort &s) {
  return HashSort{}(s);
}

bool KOREObjectSortVariable::operator==(const KOREObjectSort &other) const {
  if (auto var = dynamic_cast<const KOREObjectSortVariable *>(&other)) {
    return var->name == name;
  }
  return false;
}

void KOREObjectCompositeSort::addArgument(KOREObjectSort *Argument) {
  arguments.push_back(Argument);
}

bool KOREObjectCompositeSort::operator==(const KOREObjectSort &other) const {
  if (auto sort = dynamic_cast<const KOREObjectCompositeSort *>(&other)) {
    if (sort->name != name || sort->arguments.size() != arguments.size()) {
      return false;
    }
    for (int i = 0; i < arguments.size(); ++i) {
      if (*sort->arguments[i] != *arguments[i]) return false;
    }
    return true;
  }
  return false;
}

KOREObjectSort *KOREObjectCompositeSort::substitute(const std::unordered_map<KOREObjectSortVariable, KOREObjectSort *, HashSort> &subst) {
  bool dirty = false;
  std::vector<KOREObjectSort *> newArgs;
  for (auto arg : arguments) {
    auto newArg = arg->substitute(subst);
    if (newArg != arg) {
      dirty = true;
    }
    newArgs.push_back(newArg);
  }
  if (dirty) {
    KOREObjectCompositeSort *retval = Create(name);
    retval->arguments = newArgs;
    return retval;
  }
  return this;
}

ValueType KOREObjectCompositeSort::getCategory(KOREDefinition *definition) {
  if (category.cat != SortCategory::Uncomputed)
    return category;
  std::string name = getHook(definition);
  category = getCategory(name);
  return category;
}

std::string KOREObjectCompositeSort::getHook(KOREDefinition *definition) {
  auto &att = definition->getSortDeclarations().at(this->getName())->getAttributes();
  if (!att.count("hook")) {
    return "STRING.String";
  }
  KOREObjectCompositePattern *hookAtt = att.at("hook");
  assert(hookAtt->getArguments().size() == 1);
  auto strPattern = dynamic_cast<KOREMetaStringPattern *>(hookAtt->getArguments()[0]);
  return strPattern->getContents();
}

ValueType KOREObjectCompositeSort::getCategory(std::string name) {
  SortCategory category;
  uint64_t bits = 0;
  if (name == "MAP.Map") category = SortCategory::Map;
  else if (name == "LIST.List") category = SortCategory::List;
  else if (name == "SET.Set") category = SortCategory::Set;
  else if (name == "ARRAY.Array") category = SortCategory::Symbol; // ARRAY is implemented in K
  else if (name == "INT.Int") category = SortCategory::Int;
  else if (name == "FLOAT.Float") category = SortCategory::Float;
  else if (name == "BUFFER.StringBuffer") category = SortCategory::StringBuffer;
  else if (name == "BOOL.Bool") category = SortCategory::Bool;
  else if (name == "KVAR.KVar") category = SortCategory::Variable;
  // we expect the "hook" of a MInt to be of the form "MINT.MInt N" for some bitwidth N
  else if (name.substr(0, 9) == "MINT.MInt") {
    category = SortCategory::MInt;
    bits = std::stoi(name.substr(10));
  }
  else category = SortCategory::Symbol;
  return {category, bits};
}

void KOREObjectSymbol::addArgument(KOREObjectSort *Argument) {
  arguments.push_back(Argument);
}

void KOREObjectSymbol::addFormalArgument(KOREObjectSort *Argument) {
  formalArguments.push_back(Argument);
}

void KOREObjectSymbol::addSort(KOREObjectSort *Sort) {
  sort = Sort;
}

bool KOREObjectSymbol::operator==(KOREObjectSymbol other) const {
  if (name != other.name || arguments.size() != other.arguments.size()) {
    return false;
  }
  for (int i = 0; i < arguments.size(); ++i) {
    if (*arguments[i] != *other.arguments[i]) return false;
  }
  return true;
}

std::string KOREObjectSymbol::layoutString(KOREDefinition *definition) const {
  std::string result;
  for (auto arg : arguments) {
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(arg);
    ValueType cat = sort->getCategory(definition);
    switch(cat.cat) {
    case SortCategory::Map:
      result.push_back('1');
      break;
    case SortCategory::List:
      result.push_back('2');
      break;
    case SortCategory::Set:
      result.push_back('3');
      break;
    case SortCategory::Int:
      result.push_back('4');
      break;
    case SortCategory::Float:
      result.push_back('5');
      break;
    case SortCategory::StringBuffer:
      result.push_back('6');
      break;
    case SortCategory::Bool:
      result.push_back('7');
      break;
    case SortCategory::Variable:
      result.push_back('8');
      break;
    case SortCategory::MInt:
      result.append("_" + std::to_string(cat.bits) + "_");
    case SortCategory::Symbol:
      result.push_back('0');
      break;
    case SortCategory::Uncomputed:
      abort();
    }
  }
  return result;
}

bool KOREObjectSymbol::isConcrete() const {
  for (auto sort : arguments) {
    if (!sort->isConcrete()) {
      return false;
    }
  }
  return true;
}

bool KOREObjectSymbol::isPolymorphic() const {
  for (auto sort : arguments) {
    if (sort->isConcrete()) {
      return false;
    }
  }
  return true;
}

static std::unordered_set<std::string> BUILTINS{
  "\\and", "\\not", "\\or", "\\implies", "\\iff", "\\forall", "\\exists",
  "\\ceil", "\\floor", "\\equals", "\\in", "\\top", "\\bottom", "\\dv",
  "\\rewrites", "\\next"};

bool KOREObjectSymbol::isBuiltin() const {
  return BUILTINS.count(name);
}

void KOREObjectSymbol::instantiateSymbol(KOREObjectSymbolDeclaration *decl) {
  std::vector<KOREObjectSort *> instantiated;
  int i = 0;
  std::unordered_map<KOREObjectSortVariable, KOREObjectSort *, HashSort> vars;
  for (auto var : decl->getObjectSortVariables()) {
    vars.emplace(*var, formalArguments[i++]);
  }
  for (auto sort : decl->getSymbol()->getArguments()) {
    instantiated.push_back(sort->substitute(vars));
  }
  auto returnSort = decl->getSymbol()->sort;
  sort = returnSort->substitute(vars);

  arguments = instantiated;
}

void KOREMetaSymbol::addArgument(KOREMetaSort *Argument) {
  arguments.push_back(Argument);
}

void KOREMetaSymbol::addSort(KOREMetaSort *Sort) {
  sort = Sort;
}

std::string KOREObjectVariable::getName() const {
  return name;
}

std::string KOREObjectVariablePattern::getName() const {
  return name->getName();
}

void KOREObjectCompositePattern::addArgument(KOREPattern *Argument) {
  arguments.push_back(Argument);
}

void KOREObjectCompositePattern::markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &map) {
  if (!constructor->isBuiltin()) {
    if (!map.count(constructor->getName())) {
      map.emplace(constructor->getName(), std::vector<KOREObjectSymbol *>{});
    }
    map.at(constructor->getName()).push_back(constructor);
  }
  for (KOREPattern *arg : arguments) {
    arg->markSymbols(map);
  }
}

void KOREObjectCompositePattern::markVariables(std::map<std::string, KOREObjectVariablePattern *> &map) {
  for (KOREPattern *arg : arguments) {
    arg->markVariables(map);
  }
}

void KOREMetaCompositePattern::addArgument(KOREPattern *Argument) {
  arguments.push_back(Argument);
}

void KOREMetaCompositePattern::markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &map) {
  for (KOREPattern *arg : arguments) {
    arg->markSymbols(map);
  }
}

void KOREMetaCompositePattern::markVariables(std::map<std::string, KOREObjectVariablePattern *> &map) {
  for (KOREPattern *arg : arguments) {
    arg->markVariables(map);
  }
}

void KOREDeclaration::addAttribute(KOREPattern *Attribute) {
  if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(Attribute)) {
    attributes.insert({constructor->getConstructor()->getName(), constructor});
    return;
  }
  assert(false && "Invalid attribute found");
}

void
KOREDeclaration::addObjectSortVariable(KOREObjectSortVariable *SortVariable) {
  objectSortVariables.push_back(SortVariable);
}

void KOREDeclaration::addMetaSortVariable(KOREMetaSortVariable *SortVariable) {
  metaSortVariables.push_back(SortVariable);
}

void KOREAxiomDeclaration::addPattern(KOREPattern *Pattern) {
  pattern = Pattern;
}

static const std::string ASSOC = "assoc";
static const std::string COMM = "comm";
static const std::string IDEM = "idem";
static const std::string UNIT = "unit";
static const std::string FUNCTIONAL = "functional";
static const std::string SUBSORT = "subsort";
static const std::string CONSTRUCTOR = "constructor";
static const std::string CEIL = "ceil";

bool KOREAxiomDeclaration::isRequired() {
  return !attributes.count(ASSOC) && !attributes.count(COMM) && !attributes.count(IDEM) && !attributes.count(UNIT) && !attributes.count(FUNCTIONAL) && !attributes.count(CONSTRUCTOR) && !attributes.count(SUBSORT) && !attributes.count(CEIL);
}

bool KOREAxiomDeclaration::isTopAxiom() {
  if (auto top = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto bottomPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[0])) {
        if (bottomPattern->getConstructor()->getName() == "\\bottom" && bottomPattern->getArguments().empty()) {
          return true;
	}
      }
      return false;
    } else if (top->getConstructor()->getName() == "\\rewrites" && top->getArguments().size() == 2) {
      return true;
    } else if (top->getConstructor()->getName() == "\\and" && top->getArguments().size() == 2) {
      return true;
    }
  }
  return false;
}

KOREPattern *KOREAxiomDeclaration::getRightHandSide() const {
  if (auto top = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[1])) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto bottomPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[0])) {
            if (bottomPattern->getConstructor()->getName() == "\\bottom" && bottomPattern->getArguments().empty()) {
              if (auto andPattern2 = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[1])) {
                if (andPattern2->getConstructor()->getName() == "\\and" && andPattern2->getArguments().size() == 2) {
                  if (auto rewrites = dynamic_cast<KOREObjectCompositePattern *>(andPattern2->getArguments()[1])) {
                    if (rewrites->getConstructor()->getName() == "\\rewrites" && rewrites->getArguments().size() == 2) {
                      return rewrites->getArguments()[1];
                    }
                  }
                } else if (andPattern2->getConstructor()->getName() == "\\rewrites" && andPattern2->getArguments().size() == 2) {
                  if (auto andPattern3 = dynamic_cast<KOREObjectCompositePattern *>(andPattern2->getArguments()[1])) {
                    if (andPattern3->getConstructor()->getName() == "\\and" && andPattern3->getArguments().size() == 2) {
                      return andPattern3->getArguments()[1];
                    }
                  }
                }
              }
            } else if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[0])) {
              if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                return equals->getArguments()[1];
              }
            }
          }
        } else if (andPattern->getConstructor()->getName() == "\\rewrites" && andPattern->getArguments().size() == 2) {
          if (auto andPattern2 = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[1])) {
            if (andPattern2->getConstructor()->getName() == "\\and" && andPattern2->getArguments().size() == 2) {
              return andPattern2->getArguments()[1];
            }
          }
	}
      }
    } else if (top->getConstructor()->getName() == "\\and" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[1])) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto rewrites = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[1])) {
            if (rewrites->getConstructor()->getName() == "\\rewrites" && rewrites->getArguments().size() == 2) {
              return rewrites->getArguments()[1];
            }
          }
        }
      }
    } else if (top->getConstructor()->getName() == "\\equals" && top->getArguments().size() == 2) {
      return top->getArguments()[1];
    } else if (top->getConstructor()->getName() == "\\rewrites" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[1])) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          return andPattern->getArguments()[1];
        }
      }
    }
  }
  assert(false && "could not compute right hand side of axiom");
  abort();
}

KOREPattern *KOREAxiomDeclaration::getRequires() const {
  if (auto top = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[0])) {
        if (equals->getConstructor()->getName() == "\\and" && equals->getArguments().size() == 2) {
          equals = dynamic_cast<KOREObjectCompositePattern *>(equals->getArguments()[1]);
          assert(equals);
        }
        if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
          return equals->getArguments()[0];
        } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
          return nullptr;
        } else if (equals->getConstructor()->getName() == "\\bottom" && equals->getArguments().empty()) {
          // strategy axiom hack
          if (auto trueTop = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[1])) {
            if (trueTop->getConstructor()->getName() == "\\and" && trueTop->getArguments().size() == 2) {
              if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(trueTop->getArguments()[0])) {
                if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                  return equals->getArguments()[0];
                } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
                  return nullptr;
                }
              }
            } else if (trueTop->getConstructor()->getName() == "\\rewrites" && trueTop->getArguments().size() == 2) {
              if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(trueTop->getArguments()[0])) {
                if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
                  if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[0])) {
                    if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                      return equals->getArguments()[0];
                    } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
                      return nullptr;
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else if (top->getConstructor()->getName() == "\\and" && top->getArguments().size() == 2) {
      if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[0])) {
        if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
          return equals->getArguments()[0];
        } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
          return nullptr;
        }
      }
    } else if (top->getConstructor()->getName() == "\\equals" && top->getArguments().size() == 2) {
      return nullptr;
    } else if (top->getConstructor()->getName() == "\\rewrites" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[0])) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[0])) {
            if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
              return equals->getArguments()[0];
            } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
              return nullptr;
            }
          }
        }
      }
    }
  }
  assert(false && "ill-formed axiom");
  abort();
}


void
KOREObjectAliasDeclaration::addVariable(KOREObjectVariablePattern *Variable) {
  boundVariables.push_back(Variable);
}

void KOREObjectAliasDeclaration::addPattern(KOREObjectPattern *Pattern) {
  pattern = Pattern;
}

void KOREMetaAliasDeclaration::addVariable(KOREMetaVariablePattern *Variable) {
  boundVariables.push_back(Variable);
}

void KOREMetaAliasDeclaration::addPattern(KOREMetaPattern *Pattern) {
  pattern = Pattern;
}

bool KOREObjectSymbolDeclaration::isAnywhere() {
  return getAttributes().count("anywhere");
}

void KOREModule::addAttribute(KOREPattern *Attribute) {
  if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(Attribute)) {
    attributes.insert({constructor->getConstructor()->getName(), constructor});
    return;
  }
  assert(false && "Invalid attribute found");
}

void KOREModule::addDeclaration(KOREDeclaration *Declaration) {
  declarations.push_back(Declaration);
}

void KOREDefinition::addModule(KOREModule *Module) {
  modules.push_back(Module);
  for (auto decl : Module->getDeclarations()) {
    if (auto sortDecl = dynamic_cast<KOREObjectCompositeSortDeclaration *>(decl)) {
      sortDeclarations.insert({sortDecl->getName(), sortDecl});
      auto sort = KOREObjectCompositeSort::Create(sortDecl->getName());
      hookedSorts[sort->getHook(this)] = sort;
    } else if (auto symbolDecl = dynamic_cast<KOREObjectSymbolDeclaration *>(decl)) {
      symbolDeclarations.insert({symbolDecl->getSymbol()->getName(), symbolDecl});
    } else if (auto axiom = dynamic_cast<KOREAxiomDeclaration *>(decl)) {
      axioms.push_back(axiom);
    }
  }
}

void KOREDefinition::addAttribute(KOREPattern *Attribute) {
  if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(Attribute)) {
    attributes.insert({constructor->getConstructor()->getName(), constructor});
    return;
  }
  assert(false && "Invalid attribute found");
}

void KOREDefinition::preprocess() {
  auto symbols = std::map<std::string, std::vector<KOREObjectSymbol *>>{};
  unsigned nextOrdinal = 0;
  for (auto iter = symbolDeclarations.begin(); iter != symbolDeclarations.end(); ++iter) {
    auto decl = *iter;
    if (decl.second->getAttributes().count("freshGenerator")) {
      auto sort = decl.second->getSymbol()->sort;
      if (sort->isConcrete()) {
        freshFunctions[dynamic_cast<KOREObjectCompositeSort *>(sort)->getName()] = decl.second->getSymbol();
      }
    }
  }
  for (auto iter = axioms.begin(); iter != axioms.end();) {
    auto axiom = *iter;
    axiom->ordinal = nextOrdinal++;
    axiom->pattern->markSymbols(symbols);
    if (!axiom->isRequired()) {
      iter = axioms.erase(iter);
    } else {
      ++iter;
    }
  }
  for (auto moditer = modules.begin(); moditer != modules.end(); ++moditer) {
    auto declarations = (*moditer)->getDeclarations();
    for (auto iter = declarations.begin(); iter != declarations.end(); ++iter) {
      KOREObjectSymbolDeclaration * decl = dynamic_cast<KOREObjectSymbolDeclaration *>(*iter);
      if (decl == nullptr) {
        continue;
      }
      if (decl->isHooked() && decl->getObjectSortVariables().empty()) {
        KOREObjectSymbol * symbol = decl->getSymbol();
        symbols.emplace(symbol->getName(), std::vector<KOREObjectSymbol *>{symbol});
      }
    }
  }
  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto entry = *iter;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KOREObjectSymbol *symbol = *iter;
      auto decl = symbolDeclarations.at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }
  uint32_t nextSymbol = 0;
  uint16_t nextLayout = 1;
  auto instantiations = std::unordered_map<KOREObjectSymbol, uint32_t, HashSymbol>{};
  auto layouts = std::unordered_map<std::string, uint16_t>{};
  auto variables = std::unordered_map<std::string, std::pair<uint32_t, uint32_t>>{};
  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto entry = *iter;
    uint32_t firstTag = nextSymbol;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KOREObjectSymbol *symbol = *iter;
      if (symbol->isConcrete()) {
        if (!instantiations.count(*symbol)) {
          instantiations.emplace(*symbol, nextSymbol++);
        }
        std::string layoutStr = symbol->layoutString(this);
        if (!layouts.count(layoutStr)) {
          layouts.emplace(layoutStr, nextLayout++);
        }
        symbol->firstTag = symbol->lastTag = instantiations.at(*symbol);
        symbol->layout = layouts.at(layoutStr);
        objectSymbols[symbol->firstTag] = symbol;
        std::ostringstream Out;
        symbol->print(Out);
        allObjectSymbols[Out.str()] = symbol;
      }
    }
    uint32_t lastTag = nextSymbol-1;
    if (!entry.second.empty()) {
      variables.emplace(entry.first, std::pair<uint32_t, uint32_t>{firstTag, lastTag});
    }
  }
  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto entry = *iter;
    auto range = variables.at(entry.first);
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KOREObjectSymbol *symbol = *iter;
      if (!symbol->isConcrete()) {
        if (symbol->isPolymorphic()) {
          symbol->firstTag = range.first;
          symbol->lastTag = range.second;
          auto decl = symbolDeclarations.at(symbol->getName());
          if (decl->getAttributes().count("sortInjection")) {
            injSymbol = symbol;
          }
        }
      }
    }
  }
}

// Pretty printer
void KOREObjectSortVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void KOREMetaSortVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "#" << name;
}

void KOREObjectCompositeSort::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (const KOREObjectSort *Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KOREMetaCompositeSort::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "#" << name << "{}";
}

void KOREObjectSymbol::print(std::ostream &Out, unsigned indent) const {
  print(Out, indent, true);
}

void KOREObjectSymbol::print(std::ostream &Out, unsigned indent, bool formal) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (const KOREObjectSort *Argument : (formal ? formalArguments : arguments)) {
    if (!isFirst)
      Out << ", ";
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KOREMetaSymbol::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "#" << name << "{";
  bool isFirst = true;
  for (const KOREMetaSort *Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KOREObjectVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void KOREMetaVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "#" << name;
}

void
KOREObjectVariablePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  name->print(Out);
  Out << " : ";
  sort->print(Out);
}

void KOREMetaVariablePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  name->print(Out);
  Out << " : ";
  sort->print(Out);
}

void
KOREObjectCompositePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  constructor->print(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREPattern *Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ")";
}

void KOREMetaCompositePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  constructor->print(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREPattern *Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ")";
}

static std::string escapeChar(char c) {
  std::string result;
  if (c == '\'' || c == '\\' || !isprint(c)) {
    result.push_back('\\');
    char code[3];
    snprintf(code, 3, "%02x", (unsigned char)c);
    result.push_back(code[0]);
    result.push_back(code[1]);
  } else {
    result.push_back(c);
  }
  return result;
}

static std::string escapeString(const std::string &str) {
  std::string result;
  for (char c : str) {
    if (c == '"' || c == '\\' || !isprint(c)) {
      result.push_back('\\');
      char code[3];
      snprintf(code, 3, "%02x", (unsigned char)c);
      result.push_back(code[0]);
      result.push_back(code[1]);
    } else {
      result.push_back(c);
    }
  }
  return result;
}

void KOREMetaStringPattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "\"" << escapeString(contents) << "\"";
}

void KOREMetaCharPattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "\'" << escapeChar(contents) << "\'";
}

static void printAttributeList(
  std::ostream &Out, const std::map<std::string, KOREObjectCompositePattern *> attributes,
  unsigned indent = 0) {

  std::string Indent(indent, ' ');
  Out << Indent << "[";
  bool isFirst = true;
  for (auto &Pattern : attributes) {
    if (!isFirst)
      Out << ",";
    Pattern.second->print(Out);
    isFirst = false;
  }
  Out << "]";
}

void KOREDeclaration::printSortVariables(std::ostream &Out) const {
  Out << "{";
  bool isFirst = true;
  for (const KOREObjectSortVariable *Variable : objectSortVariables) {
    if (!isFirst)
      Out << ",";
    Variable->print(Out);
    isFirst = false;
  }
  for (const KOREMetaSortVariable *Variable : metaSortVariables) {
    if (!isFirst)
      Out << ",";
    Variable->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KOREObjectCompositeSortDeclaration::print(
  std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-sort " : "sort ") << sortName;
  printSortVariables(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREObjectSymbolDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-symbol " : "symbol ")
      << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREObjectSort *Argument : symbol->getArguments()) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  symbol->getSort()->print(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREMetaSymbolDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "symbol " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREMetaSort *Argument : symbol->getArguments()) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  symbol->getSort()->print(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREObjectAliasDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "alias " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREObjectSort *Argument : symbol->getArguments()) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  symbol->getSort()->print(Out);
  Out << " where " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  isFirst = true;
  for (const KOREObjectVariablePattern *Variable : boundVariables) {
    if (!isFirst)
      Out << ",";
    Variable->print(Out);
    isFirst = false;
  }
  Out << ") := ";
  pattern->print(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREMetaAliasDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "alias " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (const KOREMetaSort *Argument : symbol->getArguments()) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  symbol->getSort()->print(Out);
  Out << " where " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  isFirst = true;
  for (const KOREMetaVariablePattern *Variable : boundVariables) {
    if (!isFirst)
      Out << ",";
    Variable->print(Out);
    isFirst = false;
  }
  Out << ") := ";
  pattern->print(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void KOREAxiomDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "axiom ";
  printSortVariables(Out);
  pattern->print(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREModuleImportDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "import " << moduleName;
  Out << " ";
  printAttributeList(Out, attributes);
}

void KOREModule::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "module " << name << "\n";
  bool isFirst = true;
  for (const KOREDeclaration *Declaration : declarations) {
    if (!isFirst)
      Out << "\n";
    Declaration->print(Out, indent + 2);
    Out << "\n";
    isFirst = false;
  }
  Out << Indent << "endmodule\n";
  printAttributeList(Out, attributes, indent);
}

void KOREDefinition::print(std::ostream &Out, unsigned indent) const {
  printAttributeList(Out, attributes, indent);
  Out << "\n";
  for (const KOREModule *Module : modules) {
    Out << "\n";
    Module->print(Out, indent);
    Out << "\n";
  }
}
