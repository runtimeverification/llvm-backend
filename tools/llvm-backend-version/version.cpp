#include "version.inc"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

cl::OptionCategory VersionCat("kprint options");

cl::opt<bool> LLVMVersion(
    "llvm", cl::desc("Print LLVM version rather than backend version"),
    cl::cat(VersionCat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&VersionCat});
  cl::ParseCommandLineOptions(argc, argv);

  if (LLVMVersion) {
    llvm::outs() << llvm_version << '\n';
  } else {
    llvm::outs() << llvm_backend_version << '\n';
  }
}
