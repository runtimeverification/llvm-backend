#include "llvm/CodeGen/GCMetadata.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GCStrategy.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

namespace {

class LLVM_LIBRARY_VISIBILITY KoreGC : public GCStrategy {
public:
  KoreGC() {
    UseStatepoints = true;
    // These options are all gc.root specific, we specify them so that the
    // gc.root lowering code doesn't run.
    NeededSafePoints = false;
    UsesMetadata = false;
  }

  Optional<bool> isGCManagedPointer(const Type *Ty) const override {
    // Method is only valid on pointer typed values.
    const PointerType *PT = cast<PointerType>(Ty);
    // For the sake of this example GC, we arbitrarily pick addrspace(1) as our
    // GC managed heap.  We know that a pointer into this heap needs to be
    // updated and that no other pointer does.  Note that addrspace(1) is used
    // only as an example, it has no special meaning, and is not reserved for
    // GC usage.
    return (PT->getAddressSpace() != 0);
  }
};

GCRegistry::Add<KoreGC>
X("koregc", "KORE garbage collector.");
} // end anonymous namespace
