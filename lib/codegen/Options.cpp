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

cl::opt<bool> Debug(
    "debug", cl::desc("Enable debug information"), cl::ZeroOrMore,
    cl::cat(CodegenLibCat));

cl::opt<bool> NoOptimize(
    "no-optimize",
    cl::desc("Don't run optimization passes before producing output"),
    cl::cat(CodegenLibCat));

cl::opt<bool> EmitObject(
    "emit-object",
    cl::desc("Directly emit an object file to avoid separately invoking llc"),
    cl::cat(CodegenLibCat));

cl::opt<bool> BinaryIR(
    "binary-ir", cl::desc("Emit binary IR rather than text"),
    cl::cat(CodegenLibCat));

cl::opt<bool> ForceBinary(
    "f", cl::desc("Force binary bitcode output to stdout"), cl::Hidden,
    cl::cat(CodegenLibCat));

namespace kllvm {

void validate_codegen_args(bool is_tty) {
  if (EmitObject && (BinaryIR || NoOptimize)) {
    throw std::runtime_error(
        "Cannot specify --emit-object with --binary-ir or --no-optimize");
  }

  if ((EmitObject || BinaryIR) && is_tty && !ForceBinary) {
    throw std::runtime_error(
        "Not printing binary file to stdout; use -o to specify output path "
        "or force binary with -f\n");
  }
}

} // namespace kllvm
