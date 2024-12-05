//===- Extend GCStrategy of llvm/CodeGen/GCStrategy.h ---------------------===//
//
// We extend the base GCStrategy as follows:
// - use gc.safepoints instead of (default) gc.roots.
// - specify that the RewriteStatepointsForGC pass should rewrite the calls of
//   this function.
// - pointers with address space != 0 are pointing to GC-managed memory.
//===----------------------------------------------------------------------===//

// NOLINTBEGIN

#ifndef LLVM_BACKEND_GC_STRATEGY_H
#define LLVM_BACKEND_GC_STRATEGY_H

#include "llvm/IR/GCStrategy.h"
#include "llvm/IR/Type.h"

namespace kllvm {

/// The GCStrategy for the LLVM Backend
class LLVMBackendGCStrategy: public llvm::GCStrategy {
public:
  LLVMBackendGCStrategy();

  // Override
  std::optional<bool> isGCManagedPointer(const llvm::Type *Ty) const override;
};

} // end anonymous namespace

#endif // LLVM_BACKEND_GC_STRATEGY_H

// NOLINTEND
