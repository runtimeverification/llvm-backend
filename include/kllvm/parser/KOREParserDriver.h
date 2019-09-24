#ifndef KOREPARSERDRIVER_H
#define KOREPARSERDRIVER_H

#include <stack>
#include <string>

namespace kllvm {
class KOREDefinition;
class KOREModule;
class KOREDeclaration;
class KOREModuleImportDeclaration;
class KOREObjectCompositeSortDeclaration;
class KOREAxiomDeclaration;
class KOREObjectSymbolDeclaration;
class KOREMetaSymbolDeclaration;
class KOREObjectAliasDeclaration;
class KOREMetaAliasDeclaration;
class KOREPattern;
class KOREObjectPattern;
class KOREMetaPattern;
class KOREObjectCompositePattern;
class KOREMetaCompositePattern;
class KOREObjectSort;
class KOREMetaSort;
class KOREObjectCompositeSort;
class KOREObjectSortVariable;
class KOREMetaSortVariable;
class KOREObjectVariablePattern;
class KOREMetaVariablePattern;
} // end namespace kllvm

namespace kllvm {
namespace parser {

class KOREParserDriver {
private:
  ptr<KOREDefinition> currentDefinition                = nullptr;
  ptr<KOREModule> currentModule                        = nullptr;
  ptr<KOREDeclaration> currentDeclaration              = nullptr;
  ptr<KOREObjectCompositePattern> currentObjectPattern = nullptr;
  ptr<KOREMetaCompositePattern> currentMetaPattern     = nullptr;
  ptr<KOREObjectCompositeSort> currentObjectSort       = nullptr;

  enum State {
    ParsingDefinition,
    ParsingDeclaration,
    ParsingModule,
    ParsingObjectPattern,
    ParsingMetaPattern,
    ParsingObjectSort,
  };

  std::stack<State> StateStack;

  inline State getCurrentState() { return StateStack.top(); }
  inline void pushState(State state) { StateStack.push(state); }
  inline void popState() { StateStack.pop(); }

  std::stack<ptr<KOREObjectCompositeSort>> SortStack;

  inline ptr<KOREObjectCompositeSort> &getCurrentSort() { return SortStack.top(); }
  inline void pushSort(ptr<KOREObjectCompositeSort> sort) { SortStack.push(std::move(sort)); }
  inline void popSort() { SortStack.pop(); }

  std::stack<ptr<KOREPattern>> PatternStack;
#
  inline ptr<KOREPattern> &getCurrentPattern() { return PatternStack.top(); }
  inline void pushPattern(ptr<KOREPattern> pattern) { PatternStack.push(std::move(pattern)); }
  inline void popPattern() { PatternStack.pop(); }

public:
  void startDefinition();
  ptr<KOREDefinition>finishDefinition();

  void startModule(const std::string &Name);
  ptr<KOREModule> finishModule();

  void startModuleImportDeclaration(const std::string &Name);
  ptr<KOREModuleImportDeclaration> finishModuleImportDeclaration();

  void
  startObjectSortDeclaration(const std::string &Name, bool isHooked = false);
  ptr<KOREObjectCompositeSortDeclaration> finishObjectSortDeclaration();

  void startAxiomDeclaration();
  ptr<KOREAxiomDeclaration> finishAxiomDeclaration(ptr<KOREPattern>Pattern);

  void
  startObjectSymbolDeclaration(const std::string &Name, bool isHooked = false);
  ptr<KOREObjectSymbolDeclaration>
  finishObjectSymbolDeclaration(ptr<KOREObjectSort> Sort);

  void startMetaSymbolDeclaration(const std::string &Name);
  ptr<KOREMetaSymbolDeclaration>
  finishMetaSymbolDeclaration(ptr<KOREMetaSort> Sort);

  void startObjectAliasDeclaration(const std::string &Name);
  ptr<KOREObjectAliasDeclaration>
  finishObjectAliasDeclaration(ptr<KOREObjectSort> Sort, ptr<KOREObjectPattern> Pattern);

  void startMetaAliasDeclaration(const std::string &Name);
  ptr<KOREMetaAliasDeclaration>
  finishMetaAliasDeclaration(ptr<KOREMetaSort> Sort, ptr<KOREMetaPattern> Pattern);

  void startObjectPattern(const std::string &Name);
  ptr<KOREObjectCompositePattern> finishObjectPattern();

  void startMetaPattern(const std::string &Name);
  ptr<KOREMetaCompositePattern> finishMetaPattern();

  void startObjectSort(const std::string &Name);
  ptr<KOREObjectCompositeSort> finishObjectSort();

  void addModule(ptr<KOREModule>Module);
  void addDeclaration(ptr<KOREDeclaration> Declaration);
  void addObjectSortVariable(ptr<KOREObjectSortVariable> SortVariable);
  void addMetaSortVariable(ptr<KOREMetaSortVariable> SortVariable);
  void addPattern(ptr<KOREPattern> Pattern);
  void addObjectSort(ptr<KOREObjectSort> Sort);
  void addMetaSort(ptr<KOREMetaSort> Sort);
  void addObjectVariable(ptr<KOREObjectVariablePattern> Variable);
  void addMetaVariable(ptr<KOREMetaVariablePattern> Variable);
};

} // end namespace parser
} // end namespace kllvm

#endif // KOREPARSERDRIVER_H
