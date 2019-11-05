#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"
#include "runtime/alloc.h"

#include <gmp.h>

#include "runtime/header.h"

using namespace kllvm;
using namespace kllvm::parser;

extern "C" {
  void init_float(floating *result, const char *c_str) {
    std::string contents = std::string(c_str);
    init_float2(result, contents);
  }
}

static void *allocatePatternAsConfiguration(const KOREPattern *Pattern) {
  const auto constructor = dynamic_cast<const KORECompositePattern *>(Pattern);
  assert(constructor);

  const KORESymbol *symbol = constructor->getConstructor();
  assert(symbol->isConcrete() && "found sort variable in initial configuration");
  if (symbol->getName() == "\\dv") {
    const auto sort = dynamic_cast<KORECompositeSort *>(symbol->getFormalArguments()[0].get());
    const auto strPattern =
      dynamic_cast<KOREStringPattern *>(constructor->getArguments()[0].get());
    std::string contents = strPattern->getContents();
    return getToken(sort->getName().c_str(), contents.size(), contents.c_str());
  }
  std::ostringstream Out;
  symbol->print(Out);
  uint32_t tag = getTagForSymbolName(Out.str().c_str());

  if (isSymbolAFunction(tag)) {
    std::vector<void *> arguments;
    for (const auto &child : constructor->getArguments()) {
      arguments.push_back(allocatePatternAsConfiguration(child.get()));
    }
    return evaluateFunctionSymbol(tag, &arguments[0]);
  }

  struct blockheader headerVal = getBlockHeaderForSymbol(tag);
  size_t size = size_hdr(headerVal.hdr);
  
  if (size == 8) {
    return (block *) ((uint64_t)tag << 32 | 1);
  }

  std::vector<void *> children;
  for (const auto &child : constructor->getArguments()) {
    children.push_back(allocatePatternAsConfiguration(child.get()));
  }

  if (symbol->getName() == "inj") {
    uint16_t layout_code = layout_hdr(headerVal.hdr);
    layout *data = getLayoutData(layout_code);
    if (data->args[0].cat == SYMBOL_LAYOUT) {
      block *child = (block *)children[0];
      if (!((uint64_t)child & 1) && layout(child) != 0) {
        uint32_t tag = tag_hdr(child->h.hdr);
	if (tag >= first_inj_tag && tag <= last_inj_tag) {
          return child;
	}
      }
    }
  }

  block *Block = (block *) koreAlloc(size);
  Block->h = headerVal;

  storeSymbolChildren(Block, &children[0]);
  if (isSymbolABinder(tag)) {
    Block = debruijnize(Block);
  }
  return Block;
}

block *parseConfiguration(const char *filename) {
  // Parse initial configuration as a KOREPattern
  KOREParser parser(filename);
  ptr<KOREPattern> InitialConfiguration = parser.pattern();

  //InitialConfiguration->print(std::cout);

  // Allocate the llvm KORE datastructures for the configuration
  return (block *) allocatePatternAsConfiguration(InitialConfiguration.get());
}
