#include <kllvm/codegen/Debug.h>
#include <kllvm/codegen/Metadata.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace kllvm {

namespace {

std::string kompiled_dir = "kompiled_directory";
std::string strict_bytes = "enable_mutable_bytes";
std::string safe_partial = "safe_partial";
std::string proof_hint_instrumentation_slow = "proof_hint_instrumentation_slow";

void add_boolean_flag(
    llvm::Module &mod, std::string const &name, bool enabled, bool debug) {
  auto &ctx = mod.getContext();

  auto *i1_ty = llvm::Type::getInt1Ty(ctx);
  auto *enabled_cst = llvm::ConstantInt::getBool(ctx, enabled);

  auto *global = mod.getOrInsertGlobal(name, i1_ty);
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
  global_var->setConstant(true);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(enabled_cst);
  }

  if (debug) {
    init_debug_global(strict_bytes, get_bool_debug_type(), global_var);
  }
}

} // namespace

void add_kompiled_dir_symbol(
    llvm::Module &mod, std::string const &dir, bool debug) {
  auto &ctx = mod.getContext();

  auto *str = llvm::ConstantDataArray::getString(ctx, dir, true);

  auto *global = mod.getOrInsertGlobal(kompiled_dir, str->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
  global_var->setConstant(true);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(str);
  }

  if (debug) {
    init_debug_global(kompiled_dir, get_char_debug_type(), global_var);
  }
}

void add_mutable_bytes_flag(llvm::Module &mod, bool enabled, bool debug) {
  add_boolean_flag(mod, strict_bytes, enabled, debug);
}

void add_safe_partial_flag(llvm::Module &mod, bool enabled, bool debug) {
  add_boolean_flag(mod, safe_partial, enabled, debug);
}

void add_proof_hint_instrumentation_slow_flag(
    llvm::Module &mod, bool enabled, bool debug) {
  add_boolean_flag(mod, proof_hint_instrumentation_slow, enabled, debug);
}

} // namespace kllvm
