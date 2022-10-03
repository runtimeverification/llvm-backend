#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/KOREScanner.h"

#include <fstream>
#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <pattern.kore>" << std::endl;
    return 1;
  }

  auto config = KOREPattern::load(argv[1]);
  if (auto composite = dynamic_cast<KORECompositePattern *>(config.get())) {
    if (composite->getConstructor()->getName() != "\\or") {
      std::cerr << "pattern must be an \\or" << std::endl;
      return 1;
    }
    std::vector<sptr<KOREPattern>> args;
    flatten(composite, "\\or", args);

    for (unsigned i = 0; i < args.size(); i++) {
      std::string filename = argv[1] + std::string(".") + std::to_string(i + 1);
      std::ofstream out(filename.c_str());
      args[i]->print(out);
    }

    return 0;
  } else {
    std::cerr << "pattern must be an \\or" << std::endl;
    return 1;
  }
}
