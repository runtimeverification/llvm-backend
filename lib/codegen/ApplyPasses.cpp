#include <kllvm/codegen/ApplyPasses.h>

#if LLVM_VERSION_MAJOR >= 14
#include <llvm/MC/TargetRegistry.h>
#else
#include <llvm/Support/TargetRegistry.h>
#endif

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Pass.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Host.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

#include <optional>

using namespace llvm;

enum opt_level { O0, O1, O2, O3 };

extern cl::OptionCategory CodegenCat;

cl::opt<bool> KeepFramePointer(
    "fno-omit-frame-pointer",
    cl::desc("Keep frame pointer in compiled code for debugging purposes"),
    cl::cat(CodegenCat));

cl::opt<opt_level> OptimizationLevel(
    cl::desc("Choose optimization level"),
    cl::values(
        clEnumVal(O0, "No optimizations"),
        clEnumVal(O1, "Enable trivial optimizations"),
        clEnumVal(O2, "Enable default optimizations"),
        clEnumVal(O3, "Enable expensive optimizations")),
    cl::cat(CodegenCat));

namespace kllvm {

CodeGenOpt::Level get_opt_level() {
  switch (OptimizationLevel) {
  case O0: return CodeGenOpt::None;
  case O1: return CodeGenOpt::Less;
  case O2: return CodeGenOpt::Default;
  case O3: return CodeGenOpt::Aggressive;
  }
}

void apply_kllvm_opt_passes(llvm::Module &mod) {
  auto pm = legacy::PassManager();

  pm.add(createPromoteMemoryToRegisterPass());
  pm.add(createTailCallEliminationPass());

  pm.run(mod);
}

void generate_object_file(llvm::Module &mod, llvm::raw_ostream &os) {
  // The frame-pointer retention code in LLVM 12 and older is tied strongly to
  // the actual command-line flag used to specify it for code generation, rather
  // than being decoupled as it is in 13 and newer. Because LLVM 12 will be
  // deprecated for our purposes sooner rather than later, and is not the
  // default version for packaged versions of K, we simply disable the FP
  // feature.
#if LLVM_VERSION_MAJOR > 12
  if (KeepFramePointer) {
    mod.setFramePointer(FramePointerKind::All);
  } else {
    mod.setFramePointer(FramePointerKind::None);
  }
#endif

  auto triple = sys::getDefaultTargetTriple();
  mod.setTargetTriple(triple);

  auto error = std::string{};
  auto target = TargetRegistry::lookupTarget(triple, error);
  auto cpu = sys::getHostCPUName();

  auto features = SubtargetFeatures{};
  auto host_features = StringMap<bool>{};

  if (sys::getHostCPUFeatures(host_features)) {
    for (auto &feat : host_features) {
      features.AddFeature(feat.first(), feat.second);
    }
  }

  auto features_string = features.getString();
  auto options = TargetOptions{};

#if LLVM_VERSION_MAJOR >= 16
  std::optional<CodeModel::Model> model = std::nullopt;
#else
  Optional<CodeModel::Model> model = None;
#endif

  auto target_machine
      = std::unique_ptr<TargetMachine>(target->createTargetMachine(
          triple, cpu, features_string, options, Reloc::PIC_, model,
          get_opt_level()));

  auto pm = legacy::PassManager{};
  mod.setDataLayout(target_machine->createDataLayout());
  target_machine->addPassesToEmitFile(
      pm, (raw_pwrite_stream &)os, nullptr, CodeGenFileType::CGFT_ObjectFile,
      true, nullptr);

  pm.run(mod);
}

} // namespace kllvm
