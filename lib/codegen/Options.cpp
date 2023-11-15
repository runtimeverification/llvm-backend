#include <kllvm/codegen/Options.h>

using namespace llvm;

cl::OptionCategory CodegenLibCat("Internal codegen options");

cl::opt<bool> ProofHintInstrumentation(
    "proof-hint-instrumentation",
    llvm::cl::desc("Enable instrumentation for generation of proof hints"),
    llvm::cl::cat(CodegenLibCat));

cl::opt<bool> KeepFramePointer(
    "fno-omit-frame-pointer",
    cl::desc("Keep frame pointer in compiled code for debugging purposes"),
    cl::cat(CodegenLibCat));

cl::opt<opt_level> OptimizationLevel(
    cl::desc("Choose optimization level"),
    cl::values(
        clEnumVal(opt_level::O0, "No optimizations"),
        clEnumVal(opt_level::O1, "Enable trivial optimizations"),
        clEnumVal(opt_level::O2, "Enable default optimizations"),
        clEnumVal(opt_level::O3, "Enable expensive optimizations")),
    cl::cat(CodegenLibCat));
