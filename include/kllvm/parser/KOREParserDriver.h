#ifndef KOREPARSERDRIVER_H
#define KOREPARSERDRIVER_H

#include <stack>
#include <string>

namespace kllvm {
class KOREDefinition;
class KOREModule;
class KOREDeclaration;
class KOREModuleImportDeclaration;
class KORECompositeSortDeclaration;
class KOREAxiomDeclaration;
class KORESymbolDeclaration;
class KOREAliasDeclaration;
class KOREPattern;
class KOREPattern;
class KORECompositePattern;
class KORESort;
class KORECompositeSort;
class KORESortVariable;
class KOREVariablePattern;
} // end namespace kllvm

namespace kllvm {
namespace parser {

class KOREParserDriver {
private:
  KOREDefinition *currentDefinition                = nullptr;
  KOREModule *currentModule                        = nullptr;
  KOREDeclaration *currentDeclaration              = nullptr;
  KORECompositePattern *currentObjectPattern = nullptr;
  KORECompositeSort *currentObjectSort       = nullptr;

  enum State {
    ParsingDefinition,
    ParsingDeclaration,
    ParsingModule,
    ParsingObjectPattern,
    ParsingObjectSort,
  };

  std::stack<State> StateStack;

  inline State getCurrentState() { return StateStack.top(); }
  inline void pushState(State state) { StateStack.push(state); }
  inline void popState() { StateStack.pop(); }

  std::stack<KORECompositeSort *> SortStack;

  inline KORECompositeSort *getCurrentSort() { return SortStack.top(); }
  inline void pushSort(KORECompositeSort *sort) { SortStack.push(sort); }
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
  KORECompositeSortDeclaration *finishObjectSortDeclaration();

  void startAxiomDeclaration();
  KOREAxiomDeclaration *finishAxiomDeclaration(KOREPattern *Pattern);

  void
  startObjectSymbolDeclaration(const std::string &Name, bool isHooked = false);
  KORESymbolDeclaration *
  finishObjectSymbolDeclaration(KORESort *Sort);

  void startObjectAliasDeclaration(const std::string &Name);
  KOREAliasDeclaration *
  finishObjectAliasDeclaration(KORESort *Sort, KOREPattern *Pattern);

  void startObjectPattern(const std::string &Name);
  KORECompositePattern *finishObjectPattern();

  void startObjectSort(const std::string &Name);
  KORECompositeSort *finishObjectSort();

  void addModule(KOREModule *Module);
  void addDeclaration(KOREDeclaration *Declaration);
  void addObjectSortVariable(KORESortVariable *SortVariable);
  void addPattern(KOREPattern *Pattern);
  void addObjectSort(KORESort *Sort);
  void addObjectVariable(KOREVariablePattern *Variable);
};

} // end namespace parser
} // end namespace kllvm

#endif // KOREPARSERDRIVER_H
