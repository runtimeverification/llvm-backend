#include <kllvm/codegen/ApplyPasses.h>
#include <kllvm/codegen/MustTailDeadArgElimination.h>
#include <kllvm/codegen/Options.h>
#include <kllvm/codegen/RemoveDeadKFunctions.h>
#include <kllvm/codegen/SetVisibilityHidden.h>

#include "runtime/header.h"

#if LLVM_VERSION_MAJOR >= 17
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/SubtargetFeature.h>
#else
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Support/Host.h>
#endif

#include "llvm/IRReader/IRReader.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Linker/Linker.h>
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

#if LLVM_VERSION_MAJOR >= 18
#define CODEGEN_OPT_LEVEL CodeGenOptLevel
#define CODEGEN_OBJECT_FILE ObjectFile
#else
#define CODEGEN_OPT_LEVEL CodeGenOpt
#define CODEGEN_OBJECT_FILE CGFT_ObjectFile
#endif

auto get_opt_level() {
  switch (optimization_level) {
  case '0': return CODEGEN_OPT_LEVEL::None;
  case '1': return CODEGEN_OPT_LEVEL::Less;
  case '2': return CODEGEN_OPT_LEVEL::Default;
  case '3': return CODEGEN_OPT_LEVEL::Aggressive;
  default:
    throw std::runtime_error(
        fmt::format("Invalid optimization level: {}", optimization_level));
  }
}

auto get_pass_opt_level() {
  switch (optimization_level) {
  case '0': return OptimizationLevel::O0;
  case '1': return OptimizationLevel::O1;
  case '2': return OptimizationLevel::O2;
  case '3': return OptimizationLevel::O3;
  default:
    throw std::runtime_error(
        fmt::format("Invalid optimization level: {}", optimization_level));
  }
}

void apply_kllvm_opt_passes(llvm::Module &mod, bool hidden_visibility) {
  // Create the analysis managers.
  // These must be declared in this order so that they are destroyed in the
  // correct order due to inter-analysis-manager references.
  LoopAnalysisManager lam;
  FunctionAnalysisManager fam;
  CGSCCAnalysisManager cgam;
  ModuleAnalysisManager mam;

  // Create the new pass manager builder.
  // Take a look at the PassBuilder constructor parameters for more
  // customization, e.g. specifying a TargetMachine or various debugging
  // options.
  PassBuilder pb;

  // Register all the basic analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // register custom passes
  pb.registerPipelineStartEPCallback(
      [hidden_visibility](
          llvm::ModulePassManager &pm, OptimizationLevel level) {
        if (hidden_visibility) {
          pm.addPass(set_visibility_hidden());
        }
      });
  pb.registerScalarOptimizerLateEPCallback(
      [](llvm::FunctionPassManager &pm, OptimizationLevel level) {
        pm.addPass(remove_dead_k_functions());
      });
  pb.registerOptimizerEarlyEPCallback(
      [](llvm::ModulePassManager &pm, OptimizationLevel level) {
        pm.addPass(DeadArgumentEliminationPass());
        pm.addPass(
            llvm::createModuleToFunctionPassAdaptor(remove_dead_k_functions()));
      });

  // Create the pass manager.
  ModulePassManager mpm
      = pb.buildPerModuleDefaultPipeline(get_pass_opt_level());

  // Optimize the IR!
  mpm.run(mod, mam);
}

void apply_inline_pass(llvm::Module &mod) {
  // Create the analysis managers.
  // These must be declared in this order so that they are destroyed in the
  // correct order due to inter-analysis-manager references.
  LoopAnalysisManager lam;
  FunctionAnalysisManager fam;
  CGSCCAnalysisManager cgam;
  ModuleAnalysisManager mam;

  // Create the new pass manager builder.
  // Take a look at the PassBuilder constructor parameters for more
  // customization, e.g. specifying a TargetMachine or various debugging
  // options.
  PassBuilder pb;

  // Register all the basic analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // Create the pass manager.
  ModulePassManager mpm;

  // Add always inline pass
  mpm.addPass(AlwaysInlinerPass());

  // Run always inline pass
  mpm.run(mod, mam);
}

void generate_object_file(llvm::Module &mod, llvm::raw_ostream &os) {
  if (keep_frame_pointer) {
    mod.setFramePointer(FramePointerKind::All);
  } else {
    mod.setFramePointer(FramePointerKind::None);
  }

  auto const *triple = BACKEND_TARGET_TRIPLE;
  mod.setTargetTriple(triple);

  auto error = std::string{};
  auto const *target = TargetRegistry::lookupTarget(triple, error);
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
  options.GuaranteedTailCallOpt = true;

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
      pm, (raw_pwrite_stream &)os, nullptr,
      CodeGenFileType::CODEGEN_OBJECT_FILE, true, nullptr);

  pm.run(mod);
}

/* This function links the code generated module generated by
 * llvm-kompile-codegen with any llvm bitcode modules we wish to place in the
 * same translation unit as the code generated code during optimization. This
 * is done currently with only a single file: runtime/lto/alloc.cpp. We do this
 * so that inlining can occur across the functions in each file.
 */
void do_bitcode_linking(llvm::Module &mod, char *bc, unsigned bc_len) {
  Linker linker(mod);
  llvm::SMDiagnostic err;
  auto cpp_mod = llvm::parseIR(
      *llvm::MemoryBuffer::getMemBuffer(std::string(bc, bc_len)), err,
      mod.getContext());
  bool error = linker.linkInModule(std::move(cpp_mod));
  if (error) {
    throw std::runtime_error(
        "Bitcode linking failed. Please report this as a bug.");
  }
}

} // namespace kllvm
