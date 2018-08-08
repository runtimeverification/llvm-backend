#include "kllvm/ast/AST.h"

#include <unordered_set>

using namespace kllvm;

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
    for (int i = 0; i < arguments.size(); i++) {
      if (sort->arguments[i] != arguments[i]) return false;
    }
    return true;
  }
  return false;
}

void KOREObjectSymbol::addArgument(KOREObjectSort *Argument) {
  arguments.push_back(Argument);
}

void KOREObjectSymbol::addSort(KOREObjectSort *Sort) {
  sort = Sort;
}

bool KOREObjectSymbol::operator==(KOREObjectSymbol other) const {
  if (name != other.name || arguments.size() != other.arguments.size()) {
    return false;
  }
  for (int i = 0; i < arguments.size(); i++) {
    if (*arguments[i] != *other.arguments[i]) return false;
  }
  return true;
}

std::string KOREObjectSymbol::layoutString() const {
  std::string result;
  for (auto arg : arguments) {
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(arg);
    if (sort->getName() == "Map") {
      result.push_back('1');
    } else if (sort->getName() == "List") {
      result.push_back('2');
    } else if (sort->getName() == "Set") {
      result.push_back('3');
    } else if (sort->getName() == "Array") {
      result.push_back('2');
    } else if (sort->getName() == "Int") {
      result.push_back('4');
    } else if (sort->getName() == "Float") {
      result.push_back('5');
    } else if (sort->getName() == "StringBuffer") {
      result.push_back('6');
    } else if (sort->getName() == "Bool") {
      result.push_back('7');
    } else if (sort->getName() == "MInt") {
      assert(false && "not implemented yet: MInt");
    } else {
      result.push_back('0');
    }
  }
  return result;
}

uint8_t KOREObjectSymbol::length() const {
  uint8_t length = 1;
  for (auto arg : arguments) {
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(arg);
    if (sort->getName() == "Map") {
      length += 3;
    } else if (sort->getName() == "List") {
      length += 7;
    } else if (sort->getName() == "Set") {
      length += 3;
    } else if (sort->getName() == "Array") {
      length += 7;
    } else if (sort->getName() == "Int") {
      length += 2;
    } else if (sort->getName() == "Float") {
      length += 4;
    } else if (sort->getName() == "StringBuffer") {
      length += 1;
    } else if (sort->getName() == "Bool") {
      length += 1;
    } else if (sort->getName() == "MInt") {
      assert(false && "not implemented yet: MInt");
    } else {
      length += 1;
    }
  }
  return length;
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

bool KOREObjectSymbol::isBuiltin() const {
  return name == "\\and" || name == "\\not" || name == "\\or" || name == "\\implies"
    || name == "\\iff" || name == "\\forall" || name == "\\exists" || name == "\\ceil"
    || name == "\\floor" || name == "\\equals" || name == "\\in" || name == "\\top"
    || name == "\\bottom" || name == "\\dv" || name == "\\rewrites" || name == "\\next";
}

void KOREObjectSymbol::instantiateSymbol(KOREObjectSymbolDeclaration *decl) {
  std::vector<KOREObjectSort *> instantiated;
  int i = 0;
  std::unordered_set<KOREObjectSortVariable, HashSort> vars;
  for (auto var : decl->getObjectSortVariables()) {
    vars.emplace(*var);
  }
  for (auto sort : decl->getSymbol()->getArguments()) {
    if (auto var = dynamic_cast<KOREObjectSortVariable *>(sort)) {
      if (vars.count(*var)) {
        instantiated.push_back(arguments[i++]);
      } else {
        instantiated.push_back(sort);
      }
    } else {
      instantiated.push_back(sort);
    }
  }
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

void KOREObjectCompositePattern::markSymbols(std::unordered_map<std::string, std::vector<KOREObjectSymbol *>> &map) {
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

void KOREMetaCompositePattern::addArgument(KOREPattern *Argument) {
  arguments.push_back(Argument);
}

void KOREMetaCompositePattern::markSymbols(std::unordered_map<std::string, std::vector<KOREObjectSymbol *>> &map) {
  for (KOREPattern *arg : arguments) {
    arg->markSymbols(map);
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
static const std::string CONSTRUCTOR = "constructor";

bool KOREAxiomDeclaration::isRequired() {
  return !attributes.count(ASSOC) && !attributes.count(COMM) && !attributes.count(IDEM) && !attributes.count(UNIT) && !attributes.count(FUNCTIONAL) && !attributes.count(CONSTRUCTOR);
}

KOREPattern *KOREAxiomDeclaration::getRightHandSide() const {
  if (auto top = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    if (top->getConstructor()->getName() == "\\implies" && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KOREObjectCompositePattern *>(top->getArguments()[1])) {
        if (andPattern->getConstructor()->getName() == "\\and" && andPattern->getArguments().size() == 2) {
          if (auto equals = dynamic_cast<KOREObjectCompositePattern *>(andPattern->getArguments()[0])) {
            if (equals->getConstructor()->getName() == "\\equals" && equals->getArguments().size() == 2) {
              return equals->getArguments()[1];
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
    }
  }
  assert(false && "could not compute right hand side of axiom");
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
  auto symbols = std::unordered_map<std::string, std::vector<KOREObjectSymbol *>>{};
  for (auto iter = axioms.begin(); iter != axioms.end();) {
    auto axiom = *iter;
    if (!axiom->isRequired()) {
      iter = axioms.erase(iter);
    } else {
      axiom->pattern->markSymbols(symbols);
      iter++;
    }
  }
  for (auto iter = symbols.begin(); iter != symbols.end(); iter++) {
    auto entry = *iter;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); iter++) {
      KOREObjectSymbol *symbol = *iter;
      auto decl = symbolDeclarations.lookup(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }
  uint32_t nextSymbol = 0;
  uint16_t nextLayout = 1;
  auto instantiations = std::unordered_map<KOREObjectSymbol, uint32_t, HashSymbol>{};
  auto layouts = std::unordered_map<std::string, uint16_t>{};
  auto variables = std::unordered_map<std::string, std::pair<uint32_t, uint32_t>>{};
  for (auto iter = symbols.begin(); iter != symbols.end(); iter++) {
    auto entry = *iter;
    uint32_t firstTag = nextSymbol;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); iter++) {
      KOREObjectSymbol *symbol = *iter;
      if (symbol->isConcrete()) {
        if (!instantiations.count(*symbol)) {
          instantiations.emplace(*symbol, nextSymbol++);
        }
        std::string layoutStr = symbol->layoutString();
        if (!layouts.count(layoutStr)) {
          layouts.emplace(layoutStr, nextLayout++);
        }
        symbol->firstTag = symbol->lastTag = instantiations.at(*symbol);
        symbol->layout = layouts.at(layoutStr);
      }
    }
    uint32_t lastTag = nextSymbol-1;
    if (!entry.second.empty()) {
      variables.emplace(entry.first, std::pair<uint32_t, uint32_t>{firstTag, lastTag});
    }
  }
  for (auto iter = symbols.begin(); iter != symbols.end(); iter++) {
    auto entry = *iter;
    auto range = variables.at(entry.first);
    for (auto iter = entry.second.begin(); iter != entry.second.end(); iter++) {
      KOREObjectSymbol *symbol = *iter;
      if (!symbol->isConcrete()) {
        assert(symbol->isPolymorphic() && "Unsupported polymorphism");
        symbol->firstTag = range.first;
        symbol->lastTag = range.second;
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
  std::ostream &Out, const llvm::StringMap<KOREObjectCompositePattern *> attributes,
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
  Out << Indent << (isHooked ? "hooked-sort " : "sort ") << sortName;
  printSortVariables(Out);
  Out << " ";
  printAttributeList(Out, attributes);
}

void
KOREObjectSymbolDeclaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (isHooked ? "hooked-symbol " : "symbol ")
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
