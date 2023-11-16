#include <kllvm/codegen/ApplyPasses.h>
#include <kllvm/codegen/Options.h>

#include "runtime/header.h"

#if LLVM_VERSION_MAJOR >= 17
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/SubtargetFeature.h>
#else
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Support/Host.h>
#endif

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Pass.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

#include <optional>

using namespace llvm;

namespace kllvm {

CodeGenOpt::Level get_opt_level() {
  switch (OptimizationLevel) {
  case opt_level::O0: return CodeGenOpt::None;
  case opt_level::O1: return CodeGenOpt::Less;
  case opt_level::O2: return CodeGenOpt::Default;
  case opt_level::O3: return CodeGenOpt::Aggressive;
  }
}

void apply_kllvm_opt_passes(llvm::Module &mod) {
  auto pm = legacy::PassManager();

  pm.add(createPromoteMemoryToRegisterPass());
  pm.add(createTailCallEliminationPass());

  pm.run(mod);
}

void generate_object_file(llvm::Module &mod, llvm::raw_ostream &os) {
  if (KeepFramePointer) {
    mod.setFramePointer(FramePointerKind::All);
  } else {
    mod.setFramePointer(FramePointerKind::None);
  }

  auto triple = BACKEND_TARGET_TRIPLE;
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
