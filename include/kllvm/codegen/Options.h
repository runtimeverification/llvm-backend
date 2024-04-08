#ifndef CODEGEN_OPTIONS_H
#define CODEGEN_OPTIONS_H

#include "llvm/Support/CommandLine.h"

enum class opt_level { O0, O1, O2, O3 };

extern llvm::cl::OptionCategory codegen_lib_cat;

extern llvm::cl::opt<bool> debug;
extern llvm::cl::opt<bool> no_optimize;
extern llvm::cl::opt<bool> emit_object;
extern llvm::cl::opt<bool> binary_ir;
extern llvm::cl::opt<bool> force_binary;
extern llvm::cl::opt<bool> proof_hint_instrumentation;
extern llvm::cl::opt<bool> proof_hint_instrumentation_slow;
extern llvm::cl::opt<bool> keep_frame_pointer;
extern llvm::cl::opt<opt_level> optimization_level;

namespace kllvm {

void validate_codegen_args(bool is_tty);

}

#endif
