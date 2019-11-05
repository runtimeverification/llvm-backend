#include "kllvm/ast/AST.h"

#include <unordered_set>
#include <unordered_map>

using namespace kllvm;

size_t kllvm::hash_value(const kllvm::KORESort &s) {
  return HashSort{}(s);
}

bool KORESortVariable::operator==(const KORESort &other) const {
  if (auto var = dynamic_cast<const KORESortVariable *>(&other)) {
    return var->name == name;
  }
  return false;
}

void KORECompositeSort::addArgument(sptr<KORESort> Argument) {
  arguments.push_back(Argument);
}

bool KORECompositeSort::operator==(const KORESort &other) const {
  if (auto sort = dynamic_cast<const KORECompositeSort *>(&other)) {
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

sptr<KORESort> KORECompositeSort::substitute(const substitution &subst) {
  bool dirty = false;
  std::vector<sptr<KORESort>> newArgs;
  for (auto &arg : arguments) {
    auto newArg = arg->substitute(subst);
    if (newArg != arg) {
      dirty = true;
    }
    newArgs.push_back(newArg);
  }
  if (dirty) {
    sptr<KORECompositeSort> retval = Create(name);
    retval->arguments = newArgs;
    return retval;
  }
  return shared_from_this();
}

ValueType KORECompositeSort::getCategory(KOREDefinition *definition) {
  if (category.cat != SortCategory::Uncomputed)
    return category;
  std::string name = getHook(definition);
  category = getCategory(name);
  return category;
}

std::string KORECompositeSort::getHook(KOREDefinition *definition) {
  auto &att = definition->getSortDeclarations().at(this->getName())->getAttributes();
  if (!att.count("hook")) {
    return "STRING.String";
  }
  auto &hookAtt = att.at("hook");
  assert(hookAtt->getArguments().size() == 1);
  auto strPattern = dynamic_cast<KOREStringPattern *>(hookAtt->getArguments()[0].get());
  return strPattern->getContents();
}

ValueType KORECompositeSort::getCategory(std::string name) {
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

void KORESymbol::addArgument(sptr<KORESort> Argument) {
  arguments.push_back(Argument);
}

void KORESymbol::addFormalArgument(sptr<KORESort> Argument) {
  formalArguments.push_back(Argument);
}

void KORESymbol::addSort(sptr<KORESort> Sort) {
  sort = Sort;
}

bool KORESymbol::operator==(const KORESymbol &other) const {
  if (name != other.name || arguments.size() != other.arguments.size()) {
    return false;
  }
  for (int i = 0; i < arguments.size(); ++i) {
    if (*arguments[i] != *other.arguments[i]) return false;
  }
  return true;
}

std::string KORESymbol::layoutString(KOREDefinition *definition) const {
  std::string result;
  for (auto arg : arguments) {
    auto sort = dynamic_cast<KORECompositeSort *>(arg.get());
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

bool KORESymbol::isConcrete() const {
  for (auto sort : arguments) {
    if (!sort->isConcrete()) {
      return false;
    }
  }
  return true;
}

bool KORESymbol::isPolymorphic() const {
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

bool KORESymbol::isBuiltin() const {
  return BUILTINS.count(name);
}

void KORESymbol::instantiateSymbol(KORESymbolDeclaration *decl) {
  std::vector<sptr<KORESort>> instantiated;
  int i = 0;
  KORESort::substitution vars;
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

std::string KOREVariable::getName() const {
  return name;
}

std::string KOREVariablePattern::getName() const {
  return name->getName();
}

void KORECompositePattern::addArgument(ptr<KOREPattern> Argument) {
  arguments.push_back(std::move(Argument));
}

void KORECompositePattern::markSymbols(std::map<std::string, std::vector<KORESymbol *>> &map) {
  if (!constructor->isBuiltin()) {
    if (!map.count(constructor->getName())) {
      map.emplace(constructor->getName(), std::vector<KORESymbol *>{});
    }
    map.at(constructor->getName()).push_back(constructor.get());
  }
  for (auto &arg : arguments) {
    arg->markSymbols(map);
  }
}

void KORECompositePattern::markVariables(std::map<std::string, KOREVariablePattern *> &map) {
  for (auto &arg : arguments) {
    arg->markVariables(map);
  }
}

void KOREDeclaration::addAttribute(ptr<KORECompositePattern> Attribute) {
  std::string name = Attribute->getConstructor()->getName();
  attributes.insert({name, std::move(Attribute)});
}

void
KOREDeclaration::addObjectSortVariable(sptr<KORESortVariable> SortVariable) {
  objectSortVariables.push_back(SortVariable);
}

void KOREAxiomDeclaration::addPattern(ptr<KOREPattern> Pattern) {
  pattern = std::move(Pattern);
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
  if (auto top = dynamic_cast<KORECompositePattern *>(pattern.get())) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto bottomPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[0].get())) {
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
  if (auto top = dynamic_cast<KORECompositePattern *>(pattern.get())) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[1].get())) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto bottomPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[0].get())) {
            if (bottomPattern->getConstructor()->getName() == "\\bottom" && bottomPattern->getArguments().empty()) {
              if (auto andPattern2 = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[1].get())) {
                if (andPattern2->getConstructor()->getName() == "\\and" && andPattern2->getArguments().size() == 2) {
                  if (auto rewrites = dynamic_cast<KORECompositePattern *>(andPattern2->getArguments()[1].get())) {
                    if (rewrites->getConstructor()->getName() == "\\rewrites" && rewrites->getArguments().size() == 2) {
                      return rewrites->getArguments()[1].get();
                    }
                  }
                } else if (andPattern2->getConstructor()->getName() == "\\rewrites" && andPattern2->getArguments().size() == 2) {
                  if (auto andPattern3 = dynamic_cast<KORECompositePattern *>(andPattern2->getArguments()[1].get())) {
                    if (andPattern3->getConstructor()->getName() == "\\and" && andPattern3->getArguments().size() == 2) {
                      return andPattern3->getArguments()[1].get();
                    }
                  }
                }
              }
            } else if (auto equals = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[0].get())) {
              if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                return equals->getArguments()[1].get();
              }
            }
          }
        } else if (andPattern->getConstructor()->getName() == "\\rewrites" && andPattern->getArguments().size() == 2) {
          if (auto andPattern2 = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[1].get())) {
            if (andPattern2->getConstructor()->getName() == "\\and" && andPattern2->getArguments().size() == 2) {
              return andPattern2->getArguments()[1].get();
            }
          }
        }
      }
    } else if (top->getConstructor()->getName() == "\\and" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[1].get())) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto rewrites = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[1].get())) {
            if (rewrites->getConstructor()->getName() == "\\rewrites" && rewrites->getArguments().size() == 2) {
              return rewrites->getArguments()[1].get();
            }
          }
        }
      }
    } else if (top->getConstructor()->getName() == "\\equals" && top->getArguments().size() == 2) {
      return top->getArguments()[1].get();
    } else if (top->getConstructor()->getName() == "\\rewrites" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[1].get())) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          return andPattern->getArguments()[1].get();
        }
      }
    }
  }
  assert(false && "could not compute right hand side of axiom");
  abort();
}

KOREPattern *KOREAxiomDeclaration::getRequires() const {
  if (auto top = dynamic_cast<KORECompositePattern *>(pattern.get())) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto equals = dynamic_cast<KORECompositePattern *>(top->getArguments()[0].get())) {
        if (equals->getConstructor()->getName() == "\\and" && equals->getArguments().size() == 2) {
          equals = dynamic_cast<KORECompositePattern *>(equals->getArguments()[1].get());
          assert(equals);
        }
        if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
          return equals->getArguments()[0].get();
        } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
          return nullptr;
        } else if (equals->getConstructor()->getName() == "\\bottom" && equals->getArguments().empty()) {
          // strategy axiom hack
          if (auto trueTop = dynamic_cast<KORECompositePattern *>(top->getArguments()[1].get())) {
            if (trueTop->getConstructor()->getName() == "\\and" && trueTop->getArguments().size() == 2) {
              if (auto equals = dynamic_cast<KORECompositePattern *>(trueTop->getArguments()[0].get())) {
                if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                  return equals->getArguments()[0].get();
                } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
                  return nullptr;
                }
              }
            } else if (trueTop->getConstructor()->getName() == "\\rewrites" && trueTop->getArguments().size() == 2) {
              if (auto andPattern = dynamic_cast<KORECompositePattern *>(trueTop->getArguments()[0].get())) {
                if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
                  if (auto equals = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[0].get())) {
                    if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
                      return equals->getArguments()[0].get();
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
      if (auto equals = dynamic_cast<KORECompositePattern *>(top->getArguments()[0].get())) {
        if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
          return equals->getArguments()[0].get();
        } else if (equals->getConstructor()->getName() == "\\top" && equals->getArguments().empty()) {
          return nullptr;
        }
      }
    } else if (top->getConstructor()->getName() == "\\equals" && top->getArguments().size() == 2) {
      return nullptr;
    } else if (top->getConstructor()->getName() == "\\rewrites" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KORECompositePattern *>(top->getArguments()[0].get())) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto equals = dynamic_cast<KORECompositePattern *>(andPattern->getArguments()[0].get())) {
            if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
              return equals->getArguments()[0].get();
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
KOREAliasDeclaration::addVariables(ptr<KORECompositePattern> Variables) {
  boundVariables = std::move(Variables);
}

void KOREAliasDeclaration::addPattern(ptr<KOREPattern> Pattern) {
  pattern = std::move(Pattern);
}

bool KORESymbolDeclaration::isAnywhere() {
  return getAttributes().count("anywhere");
}

void KOREModule::addAttribute(ptr<KORECompositePattern> Attribute) {
  std::string name = Attribute->getConstructor()->getName();
  attributes.insert({name, std::move(Attribute)});
}

void KOREModule::addDeclaration(ptr<KOREDeclaration> Declaration) {
  declarations.push_back(std::move(Declaration));
}

void KOREDefinition::addModule(ptr<KOREModule> Module) {
  for (auto &decl : Module->getDeclarations()) {
    if (auto sortDecl = dynamic_cast<KORECompositeSortDeclaration *>(decl.get())) {
      sortDeclarations.insert({sortDecl->getName(), sortDecl});
      auto sort = KORECompositeSort::Create(sortDecl->getName());
      hookedSorts[sort->getHook(this)] = sort;
    } else if (auto symbolDecl = dynamic_cast<KORESymbolDeclaration *>(decl.get())) {
      symbolDeclarations.insert({symbolDecl->getSymbol()->getName(), symbolDecl});
    } else if (auto axiom = dynamic_cast<KOREAxiomDeclaration *>(decl.get())) {
      axioms.push_back(axiom);
    }
  }
  modules.push_back(std::move(Module));
}

void KOREDefinition::addAttribute(ptr<KORECompositePattern> Attribute) {
  std::string name = Attribute->getConstructor()->getName();
  attributes.insert({name, std::move(Attribute)});
}

void KOREDefinition::preprocess() {
  auto symbols = std::map<std::string, std::vector<KORESymbol *>>{};
  unsigned nextOrdinal = 0;
  for (auto iter = symbolDeclarations.begin(); iter != symbolDeclarations.end(); ++iter) {
    auto decl = *iter;
    if (decl.second->getAttributes().count("freshGenerator")) {
      auto sort = decl.second->getSymbol()->getSort();
      if (sort->isConcrete()) {
        freshFunctions[dynamic_cast<KORECompositeSort *>(sort.get())->getName()] = decl.second->getSymbol();
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
    auto &declarations = (*moditer)->getDeclarations();
    for (auto iter = declarations.begin(); iter != declarations.end(); ++iter) {
      KORESymbolDeclaration * decl = dynamic_cast<KORESymbolDeclaration *>(iter->get());
      if (decl == nullptr) {
        continue;
      }
      if (decl->isHooked() && decl->getObjectSortVariables().empty()) {
        KORESymbol * symbol = decl->getSymbol();
        symbols.emplace(symbol->getName(), std::vector<KORESymbol *>{symbol});
      }
    }
  }
  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto entry = *iter;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KORESymbol *symbol = *iter;
      auto decl = symbolDeclarations.at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }
  uint32_t nextSymbol = 0;
  uint16_t nextLayout = 1;
  auto instantiations = std::unordered_map<KORESymbol, uint32_t, HashSymbol>{};
  auto layouts = std::unordered_map<std::string, uint16_t>{};
  auto variables = std::unordered_map<std::string, std::pair<uint32_t, uint32_t>>{};
  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto entry = *iter;
    uint32_t firstTag = nextSymbol;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KORESymbol *symbol = *iter;
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
      KORESymbol *symbol = *iter;
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
void KORESortVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void KORECompositeSort::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (auto &Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KORESymbol::print(std::ostream &Out, unsigned indent) const {
  print(Out, indent, true);
}

void KORESymbol::print(std::ostream &Out, unsigned indent, bool formal) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (auto &Argument : (formal ? formalArguments : arguments)) {
    if (!isFirst)
      Out << ", ";
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KOREVariable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void
KOREVariablePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  name->print(Out);
  Out << " : ";
  sort->print(Out);
}

void
KORECompositePattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  constructor->print(Out);
  Out << "(";
  bool isFirst = true;
  for (auto &Argument : arguments) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ")";
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

void KOREStringPattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "\"" << escapeString(contents) << "\"";
}

static void printAttributeList(
  std::ostream &Out, const std::map<std::string, ptr<KORECompositePattern>> &attributes,
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
  for (auto &Variable : objectSortVariables) {
    if (!isFirst)
      Out << ",";
    Variable->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void KORECompositeSortDeclaration::print(
  std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-sort " : "sort ") << sortName;
  printSortVariables(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KORESymbolDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-symbol " : "symbol ")
      << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (auto &Argument : symbol->getArguments()) {
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
KOREAliasDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "alias " << symbol->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (auto &Argument : symbol->getArguments()) {
    if (!isFirst)
      Out << ",";
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  symbol->getSort()->print(Out);
  Out << " where ";
  boundVariables->print(Out);
  Out << " := ";
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
  for (auto &Declaration : declarations) {
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
  for (auto &Module : modules) {
    Out << "\n";
    Module->print(Out, indent);
    Out << "\n";
  }
}
