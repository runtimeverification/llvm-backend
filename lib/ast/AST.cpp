#include "kllvm/ast/AST.h"

using namespace kllvm;

void KOREObjectCompositeSort::addArgument(KOREObjectSort *Argument) {
  arguments.push_back(Argument);
}

void KOREObjectSymbol::addArgument(KOREObjectSort *Argument) {
  arguments.push_back(Argument);
}

void KOREObjectSymbol::addSort(KOREObjectSort *Sort) {
  sort = Sort;
}

void KOREMetaSymbol::addArgument(KOREMetaSort *Argument) {
  arguments.push_back(Argument);
}

void KOREMetaSymbol::addSort(KOREMetaSort *Sort) {
  sort = Sort;
}

void KOREObjectCompositePattern::addArgument(KOREPattern *Argument) {
  arguments.push_back(Argument);
}

void KOREMetaCompositePattern::addArgument(KOREPattern *Argument) {
  arguments.push_back(Argument);
}

void KOREDeclaration::addAttribute(KOREPattern *Attribute) {
  attributes.push_back(Attribute);
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
  attributes.push_back(Attribute);
}

void KOREModule::addDeclaration(KOREDeclaration *Declaration) {
  declarations.push_back(Declaration);
}

void KOREDefinition::addModule(KOREModule *Module) {
  modules.push_back(Module);
}

void KOREDefinition::addAttribute(KOREPattern *Attribute) {
  attributes.push_back(Attribute);
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
  std::ostream &Out, const std::vector<KOREPattern *> attributes,
  unsigned indent = 0) {

  std::string Indent(indent, ' ');
  Out << Indent << "[";
  bool isFirst = true;
  for (const KOREPattern *Pattern : attributes) {
    if (!isFirst)
      Out << ",";
    Pattern->print(Out);
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
