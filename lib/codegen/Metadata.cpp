#include <kllvm/codegen/Debug.h>
#include <kllvm/codegen/Metadata.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace kllvm {

static std::string KOMPILED_DIR = "kompiled_directory";
static std::string STRICT_BYTES = "enable_mutable_bytes";

void addKompiledDirSymbol(
    llvm::Module &mod, std::string const &dir, bool debug) {
  auto &ctx = mod.getContext();

  auto *str = llvm::ConstantDataArray::getString(ctx, dir, true);

  auto *global = mod.getOrInsertGlobal(KOMPILED_DIR, str->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(str);
  }

  if (debug) {
    initDebugGlobal(KOMPILED_DIR, getCharDebugType(), global_var);
  }
}

void addMutableBytesFlag(llvm::Module &mod, bool enabled, bool debug) {
  auto &ctx = mod.getContext();

  auto *i1_ty = llvm::Type::getInt1Ty(ctx);
  auto *enabled_cst = llvm::ConstantInt::getBool(ctx, enabled);

  auto *global = mod.getOrInsertGlobal(STRICT_BYTES, i1_ty);
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);

  if (!global_var->hasInitializer()) {
    global_var->setInitializer(enabled_cst);
  }

  if (debug) {
    initDebugGlobal(STRICT_BYTES, getBoolDebugType(), global_var);
  }
}

} // namespace kllvm
