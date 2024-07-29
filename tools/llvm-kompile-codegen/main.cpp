#include <kllvm/ast/AST.h>
#include <kllvm/codegen/ApplyPasses.h>
#include <kllvm/codegen/CreateTerm.h>
#include <kllvm/codegen/Debug.h>
#include <kllvm/codegen/Decision.h>
#include <kllvm/codegen/DecisionParser.h>
#include <kllvm/codegen/EmitConfigParser.h>
#include <kllvm/codegen/Metadata.h>
#include <kllvm/codegen/Options.h>
#include <kllvm/parser/KOREParser.h>
#include <kllvm/parser/location.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <fmt/format.h>

#include <libgen.h>
#include <sys/stat.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using namespace llvm;
using namespace kllvm;
using namespace kllvm::parser;

namespace fs = std::filesystem;

cl::OptionCategory codegen_tool_cat("llvm-kompile-codegen options");

cl::opt<std::string> definition_path(
    cl::Positional, cl::desc("<definition.kore>"), cl::Required,
    cl::cat(codegen_tool_cat));

cl::opt<std::string> decision_tree(
    cl::Positional, cl::desc("<dt.yaml>"), cl::Required,
    cl::cat(codegen_tool_cat));

cl::opt<std::string> directory(
    cl::Positional, cl::desc("<dir>"), cl::Required, cl::cat(codegen_tool_cat));

cl::opt<std::string> output_file(
    "output", cl::desc("Output file path"), cl::init("-"),
    cl::cat(codegen_tool_cat));

cl::alias output_file_alias(
    "o", cl::desc("Alias for --output"), cl::aliasopt(output_file),
    cl::cat(codegen_tool_cat));

cl::opt<bool> mutable_bytes(
    "mutable-bytes",
    cl::desc("Enable unsound reference semantics for objects of sort Bytes"),
    cl::init(false), cl::cat(codegen_tool_cat));

cl::opt<bool> hidden_visibility(
    "hidden-visibility",
    cl::desc("Set visibility of all global symbols to hidden"), cl::init(false),
    cl::cat(codegen_tool_cat));

cl::opt<bool> safe_partial(
    "safe-partial",
    cl::desc("Do not terminate the process when a partial function is "
             "evaluated at an undefined input; rather throw a recoverable "
             "exception."),
    cl::init(false), cl::cat(codegen_tool_cat));

cl::opt<bool> profile_matching(
    "profile-matching",
    cl::desc("Instrument k_step functions with code to profile time spent "
             "matching when applying each rule."),
    cl::init(false), cl::cat(codegen_tool_cat));

namespace {

fs::path dt_dir() {
  return directory.getValue();
}

fs::path get_indexed_filename(
    std::map<std::string, std::string> const &index,
    kore_symbol_declaration *decl) {
  return dt_dir() / index.at(decl->get_symbol()->get_name());
}

std::map<std::string, std::string> read_index_file() {
  auto index = std::map<std::string, std::string>{};
  auto in = std::ifstream(dt_dir() / "index.txt");

  auto line = std::string{};
  while (std::getline(in, line)) {
    size_t delim = line.find('\t');
    index[line.substr(0, delim)] = line.substr(delim + 1);
  }

  return index;
}

template <typename F>
void perform_output(F &&action) {
  if (output_file == "-") {
    std::invoke(std::forward<F>(action), llvm::outs());
  } else {
    auto err = std::error_code{};
    auto os = raw_fd_ostream(output_file, err, sys::fs::FA_Write);

    if (err) {
      throw std::runtime_error(
          fmt::format("Error opening file {}: {}", output_file, err.message()));
    }

    std::invoke(std::forward<F>(action), os);
  }
}

void initialize_llvm() {
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}

void emit_metadata(llvm::Module &mod) {
  auto kompiled_dir = fs::absolute(definition_path.getValue()).parent_path();
  add_kompiled_dir_symbol(mod, kompiled_dir, debug);
  add_mutable_bytes_flag(mod, mutable_bytes, debug);
  add_safe_partial_flag(mod, safe_partial, debug);
  add_proof_hint_instrumentation_slow_flag(
      mod, proof_hint_instrumentation_slow, debug);
}

} // namespace

// NOLINTNEXTLINE(*-cognitive-complexity)
int main(int argc, char **argv) {
  initialize_llvm();

  cl::HideUnrelatedOptions({&codegen_tool_cat, &codegen_lib_cat});
  cl::ParseCommandLineOptions(argc, argv);

  validate_codegen_args(output_file == "-");

  kore_parser parser(definition_path.getValue());
  ptr<kore_definition> definition = parser.definition();
  definition->preprocess();

  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> mod = new_module("definition", context);

  if (debug) {
    init_debug_info(mod.get(), definition_path);
  }

  emit_metadata(*mod);

  for (auto *axiom : definition->get_axioms()) {
    make_side_condition_function(axiom, definition.get(), mod.get());
    if (!axiom->is_top_axiom()) {
      make_apply_rule_function(axiom, definition.get(), mod.get());
    } else {
      auto dt_filename
          = dt_dir() / fmt::format("dt_{}.yaml", axiom->get_ordinal());
      if (fs::exists(dt_filename) && !proof_hint_instrumentation) {
        auto residuals = parse_yaml_specialdecision_tree(
            mod.get(), dt_filename, definition->get_all_symbols(),
            definition->get_hooked_sorts());
        make_apply_rule_function(
            axiom, definition.get(), mod.get(), residuals.residuals);
        make_step_function(
            axiom, definition.get(), mod.get(), residuals, profile_matching);
      } else {
        make_apply_rule_function(axiom, definition.get(), mod.get(), true);
      }

      auto match_filename
          = dt_dir() / fmt::format("match_{}.yaml", axiom->get_ordinal());
      if (fs::exists(match_filename)) {
        auto *dt = parse_yamldecision_tree(
            mod.get(), match_filename, definition->get_all_symbols(),
            definition->get_hooked_sorts());
        make_match_reason_function(definition.get(), mod.get(), axiom, dt);
      }
    }
  }

  emit_config_parser_functions(definition.get(), mod.get());

  auto *dt = parse_yamldecision_tree(
      mod.get(), decision_tree, definition->get_all_symbols(),
      definition->get_hooked_sorts());
  make_step_function(definition.get(), mod.get(), dt, false, profile_matching);
  auto *dt_search = parse_yamldecision_tree(
      mod.get(), dt_dir() / "dt-search.yaml", definition->get_all_symbols(),
      definition->get_hooked_sorts());
  make_step_function(definition.get(), mod.get(), dt_search, true, false);

  auto index = read_index_file();
  for (auto const &entry : definition->get_symbols()) {
    auto *symbol = entry.second;
    auto *decl = definition->get_symbol_declarations().at(symbol->get_name());
    if (decl->attributes().contains(attribute_set::key::Function)
        && !decl->is_hooked()) {
      auto filename = get_indexed_filename(index, decl);
      auto *func_dt = parse_yamldecision_tree(
          mod.get(), filename, definition->get_all_symbols(),
          definition->get_hooked_sorts());
      make_eval_function(
          decl->get_symbol(), definition.get(), mod.get(), func_dt);
    } else if (decl->is_anywhere()) {
      auto filename = get_indexed_filename(index, decl);
      auto *func_dt = parse_yamldecision_tree(
          mod.get(), filename, definition->get_all_symbols(),
          definition->get_hooked_sorts());

      make_anywhere_function(
          definition->get_all_symbols().at(ast_to_string(*decl->get_symbol())),
          definition.get(), mod.get(), func_dt);
    }
  }

  if (debug) {
    finalize_debug_info();
  }

  do_bitcode_linking(*mod);

  if (!no_optimize) {
    apply_kllvm_opt_passes(*mod, hidden_visibility);
  }

  perform_output([&](auto &os) {
    if (emit_object) {
      generate_object_file(*mod, os);
    } else {
      if (binary_ir) {
        WriteBitcodeToFile(*mod, os);
      } else {
        mod->print(os, nullptr);
      }
    }
  });

  return 0;
}
