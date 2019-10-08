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
    KORECompositeSortDeclaration::Create(Name, isHooked);
  pushState(ParsingDeclaration);
}

KORECompositeSortDeclaration *
KOREParserDriver::finishObjectSortDeclaration() {
  KORECompositeSortDeclaration *ParsedDeclaration =
    static_cast<KORECompositeSortDeclaration *>(currentDeclaration);
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
    KORESymbolDeclaration::Create(Name, isHooked);
  pushState(ParsingDeclaration);
}

KORESymbolDeclaration *
KOREParserDriver::finishObjectSymbolDeclaration(KORESort *Sort) {
  KORESymbolDeclaration *ParsedDeclaration =
    static_cast<KORESymbolDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectAliasDeclaration(const std::string &Name) {
  currentDeclaration = KOREAliasDeclaration::Create(Name);
  pushState(ParsingDeclaration);
}

KOREAliasDeclaration *KOREParserDriver::finishObjectAliasDeclaration(
  KORESort *Sort, KOREPattern *Pattern) {
  KOREAliasDeclaration *ParsedDeclaration =
    static_cast<KOREAliasDeclaration *>(currentDeclaration);
  ParsedDeclaration->getSymbol()->addSort(Sort);
  ParsedDeclaration->addPattern(Pattern);
  currentDeclaration = nullptr;
  popState();
  return ParsedDeclaration;
}

void KOREParserDriver::startObjectPattern(const std::string &Name) {
  pushPattern(currentObjectPattern);
  currentObjectPattern = KORECompositePattern::Create(Name);
  pushState(ParsingObjectPattern);
}

KORECompositePattern *KOREParserDriver::finishObjectPattern() {
  KORECompositePattern *ParsedObjectPattern = currentObjectPattern;
  currentObjectPattern =
    static_cast<KORECompositePattern *>(getCurrentPattern());
  popPattern();
  popState();

  return ParsedObjectPattern;
}

void KOREParserDriver::startObjectSort(const std::string &Name) {
  pushSort(currentObjectSort);
  currentObjectSort = KORECompositeSort::Create(Name);
  pushState(ParsingObjectSort);
}

KORECompositeSort *KOREParserDriver::finishObjectSort() {
  KORECompositeSort *ParsedObjectSort = currentObjectSort;
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
KOREParserDriver::addObjectSortVariable(KORESortVariable *SortVariable) {
  currentDeclaration->addObjectSortVariable(SortVariable);
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
  default:
    abort();
    break;
  }
}

void KOREParserDriver::addObjectSort(KORESort *Sort) {
  switch (getCurrentState()) {
  case ParsingDeclaration:
    static_cast<KORESymbolAliasDeclaration *>(currentDeclaration)->getSymbol()->addArgument(Sort);
    break;
  case ParsingObjectPattern:
    currentObjectPattern->getConstructor()->addFormalArgument(Sort);
    break;
  case ParsingObjectSort:
    currentObjectSort->addArgument(Sort);
    break;
  default:
    abort();
    break;
  }
}

void
KOREParserDriver::addObjectVariable(KOREVariablePattern *Variable) {
  static_cast<KOREAliasDeclaration *>(currentDeclaration)->addVariable(Variable);
}
