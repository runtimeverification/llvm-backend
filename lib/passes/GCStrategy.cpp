//===- Extend GCStrategy of llvm/CodeGen/GCStrategy.h ---------------------===//
//
// We extend the base GCStrategy as follows:
// - use gc.safepoints instead of (default) gc.roots.
// - specify that the RewriteStatepointsForGC pass should rewrite the calls of
//   this function.
// - pointers with address space != 0 are pointing to GC-managed memory.
//===----------------------------------------------------------------------===//

// NOLINTBEGIN

#include "kllvm/codegen/GCStrategy.h"

#include "llvm/CodeGen/GCMetadata.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;
using namespace kllvm;

LLVMBackendGCStrategy::LLVMBackendGCStrategy() {
  UseStatepoints = true; // Use gc.statepoints
#if LLVM_VERSION_MAJOR != 15
  UseRS4GC = true; // Rewrite the calls of a function that has this GCStrategy
#endif
}

// Override
#if LLVM_VERSION_MAJOR == 15
llvm::Optional<bool> LLVMBackendGCStrategy::isGCManagedPointer(Type const *Ty) const {
#else
std::optional<bool>
LLVMBackendGCStrategy::isGCManagedPointer(Type const *Ty) const {
#endif
  // Return false for any non-pointer type
  if (!Ty->isPointerTy()) {
    return false;
  }
  // Any pointer with address space != 0 is to managed memory.
  PointerType const *PTy = dyn_cast<PointerType>(Ty);
  if (PTy->getAddressSpace()) {
    return true;
  }
  return false;
}

// Add LLVMBackendGCStrategy to the global GCRegistry
static GCRegistry::Add<LLVMBackendGCStrategy>
    X("gcs-llvm-backend", "GC Strategy for the LLVM Backend");

// NOLINTEND
