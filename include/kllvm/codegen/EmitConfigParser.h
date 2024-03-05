#ifndef EMIT_CONFIG_PARSER_H
#define EMIT_CONFIG_PARSER_H

#include "kllvm/ast/AST.h"

#include "llvm/IR/Module.h"

namespace kllvm {

void emit_config_parser_functions(
    kore_definition *definition, llvm::Module *module);

}

#endif // EMIT_CONFIG_PARSER_H
