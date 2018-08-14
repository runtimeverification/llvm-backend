#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;

int main (int argc, char **argv) {
  // make a module
  llvm::LLVMContext Context;
  std::unique_ptr<llvm::Module> mod = newModule("test", Context);

  // add a main function
  llvm::Function *mainFunc =
    llvm::dyn_cast<llvm::Function>(
      mod->getOrInsertFunction(
        "main",
        llvm::Type::getInt32Ty(Context), // return type
        llvm::Type::getInt32Ty(Context), // argc type
        llvm::PointerType::getUnqual(    // argv type
          llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(Context)))));
  auto ArgIt = mainFunc->arg_begin();
  llvm::Argument &Argc = *ArgIt;
  Argc.setName("argc");
  ++ArgIt;
  llvm::Argument &Argv = *ArgIt;
  Argv.setName("argv");
  llvm::BasicBlock *block = llvm::BasicBlock::Create(Context, "entry", mainFunc);

  // add an access to argv[1]
  llvm::Value *FilenamePtr =
    llvm::GetElementPtrInst::CreateInBounds(
      llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(Context)),
      &Argv,
      {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Context), 1)},
      "filename_ptr",
      block);
  llvm::Value *Filename = new llvm::LoadInst(FilenamePtr, "filename", block);

  // add a call to the configuration parser
  llvm::Type *ParserReturnType = getValueType(SortCategory::Symbol, mod.get());
  llvm::Constant *ParserConfigFunc =
    mod->getOrInsertFunction("parseConfiguration", ParserReturnType,
                             llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(Context)));
  llvm::Value *RetVal =
    llvm::CallInst::Create(ParserConfigFunc, {Filename}, "ret", block);

  // add a return
  llvm::ReturnInst::Create(
    Context,
    llvm::CastInst::CreatePointerCast(RetVal, llvm::Type::getInt32Ty(Context), "", block),
    block);

  mod->print(llvm::outs(), nullptr);
  return 0;
}
