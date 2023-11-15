#ifndef CODEGEN_OPTIONS_H
#define CODEGEN_OPTIONS_H

#include "llvm/Support/CommandLine.h"

extern llvm::cl::OptionCategory CodegenLibCat;

extern llvm::cl::opt<bool> ProofHintInstrumentation;

extern llvm::cl::opt<bool> KeepFramePointer;

enum class opt_level { O0, O1, O2, O3 };
extern llvm::cl::opt<opt_level> OptimizationLevel;

#endif
