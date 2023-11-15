#ifndef CODEGEN_OPTIONS_H
#define CODEGEN_OPTIONS_H

#include "llvm/Support/CommandLine.h"

enum class opt_level { O0, O1, O2, O3 };

extern llvm::cl::OptionCategory CodegenLibCat;

extern llvm::cl::opt<bool> Debug;
extern llvm::cl::opt<bool> NoOptimize;
extern llvm::cl::opt<bool> EmitObject;
extern llvm::cl::opt<bool> BinaryIR;
extern llvm::cl::opt<bool> ForceBinary;
extern llvm::cl::opt<bool> ProofHintInstrumentation;
extern llvm::cl::opt<bool> KeepFramePointer;
extern llvm::cl::opt<opt_level> OptimizationLevel;

namespace kllvm {

void validate_codegen_args(bool is_tty);

}

#endif
