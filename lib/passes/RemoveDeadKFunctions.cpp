#include <kllvm/codegen/RemoveDeadKFunctions.h>

#include "llvm/IR/InstIterator.h"
#include "llvm/Transforms/Utils/AssumeBundleBuilder.h"
#include "llvm/Transforms/Utils/Local.h"

using namespace llvm;

namespace kllvm {

static bool is_dead(Instruction *inst) {
  if (auto *ci = dyn_cast<CallInst>(inst)) {
    if (!ci->use_empty()) {
      return false;
    }
    if (ci->paramHasAttr(0, llvm::Attribute::StructRet)) {
      if ((*ci->arg_begin())->getNumUses() > 1) {
        return false;
      }
    }
    if (auto *f = ci->getCalledFunction()) {
      if (f->hasFnAttribute("kllvm-pure")) {
        return true;
      }
    }
  }
  return false;
}

static bool remove_dead_instruction(
    Instruction *inst, SmallSetVector<Instruction *, 16> &work_list,
    TargetLibraryInfo const *tli) {
  if (is_dead(inst) || isInstructionTriviallyDead(inst, tli)) {
    salvageDebugInfo(*inst);
    salvageKnowledge(inst);

    // Null out all of the instruction's operands to see if any operand becomes
    // dead as we go.
    for (unsigned i = 0, e = inst->getNumOperands(); i != e; ++i) {
      Value *op_v = inst->getOperand(i);
      inst->setOperand(i, nullptr);

      if (!op_v->use_empty() || inst == op_v) {
        continue;
      }

      // If the operand is an instruction that became dead as we nulled out the
      // operand, and if it is 'trivially' dead, delete it in a future loop
      // iteration.
      if (Instruction *op_i = dyn_cast<Instruction>(op_v)) {
        if (is_dead(op_i) || isInstructionTriviallyDead(op_i, tli)) {
          work_list.insert(op_i);
        }
      }
    }

    inst->eraseFromParent();
    return true;
  }
  return false;
}

bool run_remove_dead_k_functions(Function &f, TargetLibraryInfo *tli) {
  bool dirty = false;
  SmallSetVector<Instruction *, 16> work_list;
  for (Instruction &i : llvm::make_early_inc_range(instructions(f))) {
    if (!work_list.count(&i)) {
      dirty |= remove_dead_instruction(&i, work_list, tli);
    }
  }

  while (!work_list.empty()) {
    Instruction *i = work_list.pop_back_val();
    dirty |= remove_dead_instruction(i, work_list, tli);
  }
  return dirty;
}

} // namespace kllvm

using namespace kllvm;

char legacy_remove_dead_k_functions::ID = 0;

static RegisterPass<legacy_remove_dead_k_functions>
    x("remove-dead-k-functions",
      "Remove side-effecting k functions that are actually dead",
      false /* Only looks at CFG */, false /* Analysis Pass */);
