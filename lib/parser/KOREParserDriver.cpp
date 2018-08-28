#include "kllvm/parser/KOREParserDriver.h"
#include "kllvm/ast/AST.h"

#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;


void KOREParserDriver::startDefinition() {
  currentDefinition = KOREDefinition::Create();
  pushState(ParsingDefinition);
}

KOREDefinition *KOREParserDriver::finishDefinition() {
  KOREDefinition *ParsedDefinition = currentDefinition;
  currentDefinition = nullptr;
  popState();
  return ParsedDefinition;
}

void KOREParserDriver::startModule(const std::string &Name) {
  currentModule = KOREModule::Create(Name);
  pushState(ParsingModule);
}

KOREModule *KOREParserDriver::finishModule() {
  KOREModule *ParsedModule = currentModule;
  currentModule = nullptr;
  popState();
  return ParsedModule;
}

void KOREParserDriver::startModuleImportDeclaration(const std::string &Name) {
  currentDeclaration = KOREModuleImportDeclaration::Create(Name);
  pushState(ParsingDeclaration);
}

KOREModuleImportDeclaration *KOREParserDriver::finishModuleImportDeclaration() {
  KOREModuleImportDeclaration *ParsedDeclaration =
    static_cast<KOREModuleImportDeclaration *>(currentDeclaration);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectSortDeclaration(
  const std::string &Name, bool isHooked) {
  currentDeclaration =
    KOREObjectCompositeSortDeclaration::Create(Name, isHooked);
  pushState(ParsingDeclaration);
}

KOREObjectCompositeSortDeclaration *
KOREParserDriver::finishObjectSortDeclaration() {
  KOREObjectCompositeSortDeclaration *ParsedDeclaration =
    static_cast<KOREObjectCompositeSortDeclaration *>(currentDeclaration);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startAxiomDeclaration() {
  currentDeclaration = KOREAxiomDeclaration::Create();
  pushState(ParsingDeclaration);
}

KOREAxiomDeclaration *
KOREParserDriver::finishAxiomDeclaration(KOREPattern *Pattern) {
  KOREAxiomDeclaration *ParsedDeclaration =
    static_cast<KOREAxiomDeclaration *>(currentDeclaration);
  ParsedDeclaration->addPattern(Pattern);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectSymbolDeclaration(
  const std::string &Name, bool isHooked) {
  currentDeclaration =
    KOREObjectSymbolDeclaration::Create(Name, isHooked);
  pushState(ParsingDeclaration);
}

KOREObjectSymbolDeclaration *
KOREParserDriver::finishObjectSymbolDeclaration(KOREObjectSort *Sort) {
  KOREObjectSymbolDeclaration *ParsedDeclaration =
    static_cast<KOREObjectSymbolDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startMetaSymbolDeclaration(const std::string &Name) {
  currentDeclaration = KOREMetaSymbolDeclaration::Create(Name);
  pushState(ParsingDeclaration);
}

KOREMetaSymbolDeclaration *
KOREParserDriver::finishMetaSymbolDeclaration(KOREMetaSort *Sort) {
  KOREMetaSymbolDeclaration *ParsedDeclaration =
    static_cast<KOREMetaSymbolDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectAliasDeclaration(const std::string &Name) {
  currentDeclaration = KOREObjectAliasDeclaration::Create(Name);
  pushState(ParsingDeclaration);
}

KOREObjectAliasDeclaration *KOREParserDriver::finishObjectAliasDeclaration(
  KOREObjectSort *Sort, KOREObjectPattern *Pattern) {
  KOREObjectAliasDeclaration *ParsedDeclaration =
    static_cast<KOREObjectAliasDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  ParsedDeclaration->addPattern(Pattern);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startMetaAliasDeclaration(const std::string &Name) {
  currentDeclaration = KOREMetaAliasDeclaration::Create(Name);
  pushState(ParsingDeclaration);
}

KOREMetaAliasDeclaration *KOREParserDriver::finishMetaAliasDeclaration(
  KOREMetaSort *Sort, KOREMetaPattern *Pattern) {
  KOREMetaAliasDeclaration *ParsedDeclaration =
    static_cast<KOREMetaAliasDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  ParsedDeclaration->addPattern(Pattern);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectPattern(const std::string &Name) {
  pushPattern(currentObjectPattern);
  currentObjectPattern = KOREObjectCompositePattern::Create(Name);
  pushState(ParsingObjectPattern);
}

KOREObjectCompositePattern *KOREParserDriver::finishObjectPattern() {
  KOREObjectCompositePattern *ParsedObjectPattern = currentObjectPattern;
  currentObjectPattern =
    static_cast<KOREObjectCompositePattern *>(getCurrentPattern());
  popPattern();
  popState();

  return ParsedObjectPattern;
}

void KOREParserDriver::startMetaPattern(const std::string &Name) {
  pushPattern(currentMetaPattern);
  currentMetaPattern = KOREMetaCompositePattern::Create(Name);
  pushState(ParsingMetaPattern);
}

KOREMetaCompositePattern *KOREParserDriver::finishMetaPattern() {
  KOREMetaCompositePattern *ParsedMetaPattern = currentMetaPattern;
  currentMetaPattern =
    static_cast<KOREMetaCompositePattern *>(getCurrentPattern());
  popPattern();
  popState();

  return ParsedMetaPattern;
}

void KOREParserDriver::startObjectSort(const std::string &Name) {
  pushSort(currentObjectSort);
  currentObjectSort = KOREObjectCompositeSort::Create(Name);
  pushState(ParsingObjectSort);
}

KOREObjectCompositeSort *KOREParserDriver::finishObjectSort() {
  KOREObjectCompositeSort *ParsedObjectSort = currentObjectSort;
  currentObjectSort = getCurrentSort();
  popSort();
  popState();

  return ParsedObjectSort;
}

void KOREParserDriver::addModule(KOREModule *Module) {
  currentDefinition->addModule(Module);
}

void KOREParserDriver::addDeclaration(KOREDeclaration *Declaration) {
  currentModule->addDeclaration(Declaration);
}

void
KOREParserDriver::addObjectSortVariable(KOREObjectSortVariable *SortVariable) {
  currentDeclaration->addObjectSortVariable(SortVariable);
}

void
KOREParserDriver::addMetaSortVariable(KOREMetaSortVariable *SortVariable) {
  currentDeclaration->addMetaSortVariable(SortVariable);
}

void KOREParserDriver::addPattern(KOREPattern *Pattern) {
  switch (getCurrentState()) {
  case ParsingDefinition:
    currentDefinition->addAttribute(Pattern);
    break;
  case ParsingModule:
    currentModule->addAttribute(Pattern);
    break;
  case ParsingDeclaration:
    currentDeclaration->addAttribute(Pattern);
    break;
  case ParsingObjectPattern:
    currentObjectPattern->addArgument(Pattern);
    break;
  case ParsingMetaPattern:
    currentMetaPattern->addArgument(Pattern);
    break;
  default:
    assert(false);
    break;
  }
}

void KOREParserDriver::addObjectSort(KOREObjectSort *Sort) {
  switch (getCurrentState()) {
  case ParsingDeclaration:
    static_cast<KOREObjectSymbolAliasDeclaration *>(currentDeclaration)->getSymbol()->addArgument(Sort);
    break;
  case ParsingObjectPattern:
    currentObjectPattern->getConstructor()->addFormalArgument(Sort);
    break;
  case ParsingObjectSort:
    currentObjectSort->addArgument(Sort);
    break;
  default:
    assert(false);
    break;
  }
}

void KOREParserDriver::addMetaSort(KOREMetaSort *Sort) {
  switch (getCurrentState()) {
  case ParsingDeclaration:
    static_cast<KOREMetaSymbolAliasDeclaration *>(currentDeclaration)->getSymbol()->addArgument(Sort);
    break;
  case ParsingMetaPattern:
    currentMetaPattern->getConstructor()->addArgument(Sort);
    break;
  default:
    assert(false);
    break;
  }
}

void
KOREParserDriver::addObjectVariable(KOREObjectVariablePattern *Variable) {
  static_cast<KOREObjectAliasDeclaration *>(currentDeclaration)->addVariable(Variable);
}

void
KOREParserDriver::addMetaVariable(KOREMetaVariablePattern *Variable) {
  static_cast<KOREMetaAliasDeclaration *>(currentDeclaration)->addVariable(Variable);
}

