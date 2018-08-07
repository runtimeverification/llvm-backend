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
  KOREDefinition *currentDefinition                = nullptr;
  KOREModule *currentModule                        = nullptr;
  KOREDeclaration *currentDeclaration              = nullptr;
  KOREObjectCompositePattern *currentObjectPattern = nullptr;
  KOREMetaCompositePattern *currentMetaPattern     = nullptr;
  KOREObjectCompositeSort *currentObjectSort       = nullptr;

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

  std::stack<KOREObjectCompositeSort *> SortStack;

  inline KOREObjectCompositeSort *getCurrentSort() { return SortStack.top(); }
  inline void pushSort(KOREObjectCompositeSort *sort) { SortStack.push(sort); }
  inline void popSort() { SortStack.pop(); }

  std::stack<KOREPattern *> PatternStack;

  inline KOREPattern *getCurrentPattern() { return PatternStack.top(); }
  inline void pushPattern(KOREPattern *pattern) { PatternStack.push(pattern); }
  inline void popPattern() { PatternStack.pop(); }

public:
  void startDefinition();
  KOREDefinition *finishDefinition();

  void startModule(const std::string &Name);
  KOREModule *finishModule();

  void startModuleImportDeclaration(const std::string &Name);
  KOREModuleImportDeclaration *finishModuleImportDeclaration();

  void
  startObjectSortDeclaration(const std::string &Name, bool isHooked = false);
  KOREObjectCompositeSortDeclaration *finishObjectSortDeclaration();

  void startAxiomDeclaration();
  KOREAxiomDeclaration *finishAxiomDeclaration(KOREPattern *Pattern);

  void
  startObjectSymbolDeclaration(const std::string &Name, bool isHooked = false);
  KOREObjectSymbolDeclaration *
  finishObjectSymbolDeclaration(KOREObjectSort *Sort);

  void startMetaSymbolDeclaration(const std::string &Name);
  KOREMetaSymbolDeclaration *
  finishMetaSymbolDeclaration(KOREMetaSort *Sort);

  void startObjectAliasDeclaration(const std::string &Name);
  KOREObjectAliasDeclaration *
  finishObjectAliasDeclaration(KOREObjectSort *Sort, KOREObjectPattern *Pattern);

  void startMetaAliasDeclaration(const std::string &Name);
  KOREMetaAliasDeclaration *
  finishMetaAliasDeclaration(KOREMetaSort *Sort, KOREMetaPattern *Pattern);

  void startObjectPattern(const std::string &Name);
  KOREObjectCompositePattern *finishObjectPattern();

  void startMetaPattern(const std::string &Name);
  KOREMetaCompositePattern *finishMetaPattern();

  void startObjectSort(const std::string &Name);
  KOREObjectCompositeSort *finishObjectSort();

  void addModule(KOREModule *Module);
  void addDeclaration(KOREDeclaration *Declaration);
  void addObjectSortVariable(KOREObjectSortVariable *SortVariable);
  void addMetaSortVariable(KOREMetaSortVariable *SortVariable);
  void addPattern(KOREPattern *Pattern);
  void addObjectSort(KOREObjectSort *Sort);
  void addMetaSort(KOREMetaSort *Sort);
  void addObjectVariable(KOREObjectVariablePattern *Variable);
  void addMetaVariable(KOREMetaVariablePattern *Variable);
};

} // end namespace parser
} // end namespace kllvm

#endif // KOREPARSERDRIVER_H
