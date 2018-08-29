#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"

#include <gmp.h>

using namespace kllvm;
using namespace kllvm::parser;

extern "C" {
  // llvm: blockheader = type { i64 } 
  struct blockheader {
    uint64_t header;
  };

  // llvm: block = type { %blockheader, [0 x i64 *] }
  struct block {
    blockheader header;
    uint64_t *children[];
  };

  
  // llvm: string = type { %blockheader, [0 x i8] }
  struct string {
    blockheader header;
    char bytes[];
  };

  // llvm: map = type { i64, i8 *, i8 * }
  struct map {
    uint64_t a;
    void *b;
    void *c;
  };

  // llvm: set = type { i8 *, i8 *, i64 }
  struct set {
    void *a;
    void *b;
    uint64_t c;
  };

  // llvm: list = type { i64, i64, i8 *, i8 *, i8 *, i8 *, i8 * }
  struct list {
    uint64_t a;
    uint64_t b;
    void *c;
    void *d;
    void *e;
    void *f;
    char *g;
  };
 
  // This function is exported to be used by the interpreter 
  block *parseConfiguration(const char *filename);

  // The following functions have to be generated at kompile time
  // and linked with the interpreter.
  uint32_t getTagForSymbolName(const char *symbolname);
  struct blockheader getBlockHeaderForSymbol(uint32_t tag);
  bool isSymbolAFunction(uint32_t tag);
  void storeSymbolChildren(block *symbol, void *children[]);
  void *evaluateFunctionSymbol(uint32_t tag, void *arguments[]);
  void *getToken(const char *sortname, uint64_t len, const char *tokencontents);
}

static void *allocatePatternAsConfiguration(const KOREPattern *Pattern) {
  const auto constructor = dynamic_cast<const KOREObjectCompositePattern *>(Pattern);
  assert(constructor);

  const KOREObjectSymbol *symbol = constructor->getConstructor();
  assert(symbol->isConcrete() && "found sort variable in initial configuration");
  if (symbol->getName() == "\\dv") {
    const auto sort = dynamic_cast<KOREObjectCompositeSort *>(symbol->getFormalArguments()[0]);
    const auto strPattern =
      dynamic_cast<KOREMetaStringPattern *>(constructor->getArguments()[0]);
    std::string contents = strPattern->getContents();
    return getToken(sort->getName().c_str(), contents.size(), contents.c_str());
  }
  std::ostringstream Out;
  symbol->print(Out);
  uint32_t tag = getTagForSymbolName(Out.str().c_str());

  if (isSymbolAFunction(tag)) {
    std::vector<void *> arguments;
    for (const auto child : constructor->getArguments()) {
      arguments.push_back(allocatePatternAsConfiguration(child));
    }
    return evaluateFunctionSymbol(tag, &arguments[0]);
  }

  struct blockheader headerVal = getBlockHeaderForSymbol(tag);
  size_t size = ((headerVal.header & 0xff00000000) >> 32) * 8;
  
  if (size == 8) {
    return (block *) ((uint64_t)tag << 32 | 1);
  }

  block *Block = (block *) malloc(size);
  Block->header = headerVal;

  std::vector<void *> children;
  for (const auto child : constructor->getArguments()) {
    children.push_back(allocatePatternAsConfiguration(child));
  }
  storeSymbolChildren(Block, &children[0]);
  return Block;
}

block *parseConfiguration(const char *filename) {
  // Parse configuartion definition into a KOREDefinition.
  // A configuration definition should contain a single attribute named
  // "initial-configuration" that contains the initial configuation as
  // an object pattern and a single empty module with no attributes.
  KOREScanner scanner(filename);
  KOREParserDriver driver;
  KOREDefinition *definition;
  KOREParser parser(scanner, driver, &definition);
  parser.parse();
  definition->preprocess();

  // We expect the initial configuration as an attribute named "initial-configuration"
  assert(definition->getAttributes().count("initial-configuration"));
  const KOREObjectCompositePattern *InitialConfigurationAttribute =
    definition->getAttributes().lookup("initial-configuration");
  assert(InitialConfigurationAttribute->getArguments().size() > 0);
  const KOREPattern *InitialConfiguration =
    InitialConfigurationAttribute->getArguments()[0];

  //InitialConfiguration->print(std::cout);

  // Allocate the llvm KORE datastructures for the configuration
  return (block *) allocatePatternAsConfiguration(InitialConfiguration);
}
