#include <kllvm/codegen/Debug.h>
#include <kllvm/codegen/Metadata.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace kllvm {

static std::string KOMPILED_DIR = "kompiled_directory";

void addKompiledDirSymbol(
    llvm::LLVMContext &Context, std::string const &dir, llvm::Module *mod,
    bool debug) {
  auto *Str = llvm::ConstantDataArray::getString(Context, dir, true);
  auto *global = mod->getOrInsertGlobal(KOMPILED_DIR, Str->getType());
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }

  if (debug) {
    initDebugGlobal(KOMPILED_DIR, getCharDebugType(), globalVar);
  }
}

} // namespace kllvm
