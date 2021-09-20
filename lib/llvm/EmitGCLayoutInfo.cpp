#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Statepoint.h"
#include "llvm/Support/raw_ostream.h"
#include "kllvm/ast/AST.h"
#include <cstdlib>
#include <vector>

using namespace llvm;
using namespace kllvm;

#define DEBUG_TYPE "EmitGCLayoutInfo"

namespace {
  struct EmitGCLayoutInfo : public ModulePass {
    static char ID;
    EmitGCLayoutInfo() : ModulePass(ID) {}

    void error(Type *Ty) {
      llvm::errs() << "Could not identify garbage collection information! This is a bug in the llvm backend of K\n";
      Ty->print(llvm::errs());
      abort();
    }

    bool runOnModule(Module &M) override {
      unsigned int nextID = 0;
      unsigned int numRelocations = 0;
      std::map<std::pair<unsigned int, unsigned int>, ValueType> cats;
      auto &Ctx = M.getContext();
      for (Function &F : M.functions()) {
        for (BasicBlock &BB : F.getBasicBlockList()) {
          for (Instruction &I : BB.getInstList()) {
#if __clang_major__ >= 11
            if (auto *GCSI = dyn_cast<GCStatepointInst>(&I)) {
              auto S = GCSI;
              constexpr auto IDPos = GCStatepointInst::IDPos;
#else
            auto SImpl = Statepoint(&I);
            if (SImpl) {
              auto GCSI = SImpl.getCall();
              auto S = &SImpl;
              constexpr auto IDPos = Statepoint::IDPos;
#endif
              unsigned int id = nextID++;
              GCSI->setArgOperand(IDPos, ConstantInt::get(GCSI->getArgOperand(IDPos)->getType(), id));
              unsigned int nrelocs = S->gc_args_end() - S->gc_args_begin();
              unsigned int i = nrelocs - 1;
              for (auto &Arg : S->gc_args()) {
                auto *Ty = Arg->getType()->getPointerElementType();
                if (Ty->isIntegerTy()) {
                  i--;
                  continue;
                }
                if (!Ty->isStructTy()) {
                  error(Ty);
                }
                auto StructTy = llvm::cast<StructType>(Ty);
                if (!StructTy->hasName()) {
                  error(StructTy);
                }
                std::string name = StructTy->getName().str();
                ValueType cat;
                cat.bits = 0;
                if (name == "map") {
                  cat.cat = SortCategory::Map;
                } else if (name == "list") {
                  cat.cat = SortCategory::List;
                } else if (name == "set") {
                  cat.cat = SortCategory::Set;
                } else if (name == "mpz") {
                  cat.cat = SortCategory::Int;
                } else if (name == "floating") {
                  cat.cat = SortCategory::Float;
                } else if (name == "stringbuffer") {
                  cat.cat = SortCategory::StringBuffer;
                } else if (name == "block") {
                  cat.cat = SortCategory::Symbol;
                } else {
                  error(StructTy);
                }
                cats[std::make_pair(id, i)] = cat;
                i--;
              }
              if (nrelocs > numRelocations) {
                numRelocations = nrelocs;
              }
            }
          }
        }
      }

      unsigned int numRecords = nextID;
      std::vector<char> table;
      for (unsigned int i = 0; i < numRecords; i++) {
        for (unsigned int j = 0; j < numRelocations; j++) {
          table.push_back((char)cats[std::make_pair(i, j)].cat);
        }
      }

      auto Arr = ConstantDataArray::get(Ctx, table);
      Constant *C = M.getOrInsertGlobal("gc_stackmap_layoutinfo", Arr->getType());
      auto G = cast<GlobalVariable>(C);
      if (!G->hasInitializer()) {
        G->setInitializer(Arr);
      }
      Constant *C2 = M.getOrInsertGlobal("gc_stackmap_num_relocations", Type::getInt32Ty(Ctx));
      auto G2 = cast<GlobalVariable>(C2);
      if (!G2->hasInitializer()) {
        G2->setInitializer(ConstantInt::get(Type::getInt32Ty(Ctx), numRelocations));
      }

      return true;
    }
  };
}

char EmitGCLayoutInfo::ID = 0;

static RegisterPass<EmitGCLayoutInfo> X("emit-gc-layout-info", "Emit Layout Info for Garbage Collection",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
