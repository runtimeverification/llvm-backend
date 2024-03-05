#include "version.inc"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

cl::OptionCategory version_cat("kprint options");

cl::opt<bool> llvm_library_version(
    "llvm", cl::desc("Print LLVM version rather than backend version"),
    cl::cat(version_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&version_cat});
  cl::ParseCommandLineOptions(argc, argv);

  if (llvm_library_version) {
    llvm::outs() << llvm_version << '\n';
  } else {
    llvm::outs() << llvm_backend_version << '\n';
  }
}
