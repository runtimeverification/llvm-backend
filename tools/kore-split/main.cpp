#include "kllvm/ast/AST.h"
#include "kllvm/parser/location.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace kllvm;
using namespace kllvm::parser;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <pattern.kore>" << std::endl;
    return 1;
  }

  auto config = kore_pattern::load(argv[1]);
  if (auto *composite = dynamic_cast<kore_composite_pattern *>(config.get())) {
    if (composite->get_constructor()->get_name() != "\\or") {
      std::cerr << "pattern must be an \\or" << std::endl;
      return 1;
    }
    std::vector<sptr<kore_pattern>> args;
    flatten(composite, "\\or", args);

    for (unsigned i = 0; i < args.size(); i++) {
      std::string filename = argv[1] + std::string(".") + std::to_string(i + 1);
      std::ofstream out(filename.c_str());
      args[i]->print(out);
    }

    return 0;
  }
  std::cerr << "pattern must be an \\or" << std::endl;
  return 1;
}
