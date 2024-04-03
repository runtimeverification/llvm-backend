#include <kllvm/codegen/Options.h>

using namespace llvm;

cl::OptionCategory codegen_lib_cat("Internal codegen options");

cl::opt<bool> proof_hint_instrumentation(
    "proof-hint-instrumentation",
    llvm::cl::desc("Enable instrumentation for generation of proof hints"),
    llvm::cl::cat(codegen_lib_cat));

cl::opt<bool> proof_hint_instrumentation_slow(
    "proof-hint-instrumentation-slow",
    llvm::cl::desc("Enable instrumentation for generation of proof hints that "
                   "contain function argument KORE terms as part of the trace"),
    llvm::cl::cat(codegen_lib_cat));

cl::opt<bool> keep_frame_pointer(
    "fno-omit-frame-pointer",
    cl::desc("Keep frame pointer in compiled code for debugging purposes"),
    cl::cat(codegen_lib_cat));

cl::opt<opt_level> optimization_level(
    cl::desc("Choose optimization level"),
    cl::values(
        clEnumVal(opt_level::O0, "No optimizations"),
        clEnumVal(opt_level::O1, "Enable trivial optimizations"),
        clEnumVal(opt_level::O2, "Enable default optimizations"),
        clEnumVal(opt_level::O3, "Enable expensive optimizations")),
    cl::cat(codegen_lib_cat));

cl::opt<bool> debug(
    "debug", cl::desc("Enable debug information"), cl::ZeroOrMore,
    cl::cat(codegen_lib_cat));

cl::opt<bool> no_optimize(
    "no-optimize",
    cl::desc("Don't run optimization passes before producing output"),
    cl::cat(codegen_lib_cat));

cl::opt<bool> emit_object(
    "emit-object",
    cl::desc("Directly emit an object file to avoid separately invoking llc"),
    cl::cat(codegen_lib_cat));

cl::opt<bool> binary_ir(
    "binary-ir", cl::desc("Emit binary IR rather than text"),
    cl::cat(codegen_lib_cat));

cl::opt<bool> force_binary(
    "f", cl::desc("Force binary bitcode output to stdout"), cl::Hidden,
    cl::cat(codegen_lib_cat));

namespace kllvm {

void validate_codegen_args(bool is_tty) {
  if (emit_object && (binary_ir || no_optimize)) {
    throw std::runtime_error(
        "Cannot specify --emit-object with --binary-ir or --no-optimize");
  }

  if ((emit_object || binary_ir) && is_tty && !force_binary) {
    throw std::runtime_error(
        "Not printing binary file to stdout; use -o to specify output path "
        "or force binary with -f\n");
  }
}

} // namespace kllvm
