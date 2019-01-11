#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"
#include "runtime/alloc.h"

#include <gmp.h>

#include "runtime/header.h"

using namespace kllvm;
using namespace kllvm::parser;

extern "C" void init_float(floating *result, const char *c_str) {
  std::string contents = std::string(c_str);
  size_t is_float = contents.find_first_of("fF");
  size_t prec, exp;
  if (is_float != std::string::npos) {
    prec = 24;
    exp = 8;
  } else {
    size_t has_prec = contents.find_first_of("pP");
    if (has_prec == std::string::npos) {
       prec = 53;
       exp = 11;
    } else {
      size_t exp = contents.find_first_of("xX");
      std::string prec_str = contents.substr(has_prec+1, exp-has_prec);
      std::string exp_str = contents.substr(exp+1);
      prec = atoll(prec_str.c_str());
      exp = atoll(exp_str.c_str());
    }
  }
  mpfr_t value;
  mpfr_init2(value, prec);
  size_t last = contents.find_first_of("fFdDpP");
  std::string str_value = contents.substr(0, last);
  mpfr_set_str(value, str_value.c_str(), 10, MPFR_RNDN);
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
  size_t size = ((headerVal.hdr & 0xff00000000) >> 32) * 8;
  
  if (size == 8) {
    return (block *) ((uint64_t)tag << 32 | 1);
  }

  block *Block = (block *) koreAlloc(size);
  Block->h = headerVal;

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
