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

void addBooleanFlag(
    llvm::Module &mod, std::string const &name, bool enabled, bool debug) {
  auto &ctx = mod.getContext();

  auto *i1_ty = llvm::Type::getInt1Ty(ctx);
  auto *enabled_cst = llvm::ConstantInt::getBool(ctx, enabled);

  auto *global = mod.getOrInsertGlobal(name, i1_ty);
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(enabled_cst);
  }

  if (debug) {
    initDebugGlobal(strict_bytes, getBoolDebugType(), global_var);
  }
}

} // namespace

void addKompiledDirSymbol(
    llvm::Module &mod, std::string const &dir, bool debug) {
  auto &ctx = mod.getContext();

  auto *str = llvm::ConstantDataArray::getString(ctx, dir, true);

  auto *global = mod.getOrInsertGlobal(kompiled_dir, str->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(str);
  }

  if (debug) {
    initDebugGlobal(kompiled_dir, getCharDebugType(), global_var);
  }
}

void addMutableBytesFlag(llvm::Module &mod, bool enabled, bool debug) {
  addBooleanFlag(mod, strict_bytes, enabled, debug);
}

void addSafePartialFlag(llvm::Module &mod, bool enabled, bool debug) {
  addBooleanFlag(mod, safe_partial, enabled, debug);
}

} // namespace kllvm
