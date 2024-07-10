#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/Util.h"

#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <fmt/format.h>

#include <cstdint>
#include <cstdlib>
#include <iosfwd>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config/macros.h"

namespace kllvm {

static llvm::Constant *get_symbol_name_ptr(
    kore_symbol *symbol, llvm::BasicBlock *set_block_name, llvm::Module *module,
    bool use_symbol_name) {
  llvm::LLVMContext &ctx = module->getContext();
  if (use_symbol_name) {
    auto name = ast_to_string(*symbol);
    if (set_block_name) {
      set_block_name->setName(name);
    }
    auto *str = llvm::ConstantDataArray::getString(ctx, name, true);
    auto *global = module->getOrInsertGlobal(
        fmt::format("sym_name_{}", name), str->getType());
    auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
      global_var->setInitializer(str);
    }
    llvm::Constant *zero
        = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        str->getType(), global_var, indices);
    return ptr;
  }
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), symbol->get_tag());
}

static llvm::Function *get_strcmp(llvm::Module *module) {
  llvm::LLVMContext &ctx = module->getContext();
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  auto *type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(ctx), {ptr_ty, ptr_ty}, false);
  return get_or_insert_function(module, "strcmp", type);
}

static void emit_get_tag_for_symbol_name(
    kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &ctx = module->getContext();
  auto *type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(ctx), {llvm::PointerType::getUnqual(ctx)}, false);
  auto *func = get_or_insert_function(
      module, "get_tag_for_symbol_name_internal", type);
  auto *current_block = llvm::BasicBlock::Create(ctx, "");
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(ctx), definition->get_symbols().size(), "phi",
      merge_block);
  auto const &syms = definition->get_all_symbols();
  llvm::Function *strcmp = get_strcmp(module);
  for (auto const &entry : syms) {
    uint32_t tag = entry.second->get_tag();
    auto *symbol = entry.second;
    current_block->insertInto(func);
    auto *ptr = get_symbol_name_ptr(symbol, current_block, module, true);
    auto *compare = llvm::CallInst::Create(
        strcmp, {func->arg_begin(), ptr}, "", current_block);
    auto *icmp = new llvm::ICmpInst(
        *current_block, llvm::CmpInst::ICMP_EQ, compare,
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0));
    auto *false_block = llvm::BasicBlock::Create(ctx, "");
    llvm::BranchInst::Create(merge_block, false_block, icmp, current_block);
    phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), tag),
        current_block);
    current_block = false_block;
  }
  phi->addIncoming(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), ERROR_TAG),
      current_block);
  llvm::BranchInst::Create(merge_block, current_block);
  current_block->insertInto(func);
  llvm::ReturnInst::Create(ctx, phi, merge_block);
  merge_block->insertInto(func);
}

static std::string string_struct = "string";
static std::string layout_struct = "layout";
static std::string layoutitem_struct = "layoutitem";

static void emit_data_table_for_symbol(
    std::string const &name, llvm::Type *ty, llvm::DIType *dity,
    kore_definition *definition, llvm::Module *module,
    llvm::Constant *getter(kore_definition *, llvm::Module *, kore_symbol *)) {
  llvm::LLVMContext &ctx = module->getContext();
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(ctx));
  auto *func = get_or_insert_function(
      module, name, llvm::FunctionType::get(ty, arg_types, false));
  init_debug_function(
      name, name, get_debug_function_type(dity, {get_int_debug_type()}),
      definition, func);
  auto *entry_block = llvm::BasicBlock::Create(ctx, "entry", func);
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *icmp = new llvm::ICmpInst(
      *entry_block, llvm::CmpInst::ICMP_ULE, func->arg_begin(),
      llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(ctx), syms.rbegin()->first));
  llvm::BranchInst::Create(merge_block, stuck, icmp, entry_block);
  auto *table_type = llvm::ArrayType::get(ty, syms.size());
  auto *table = module->getOrInsertGlobal("table_" + name, table_type);
  auto *global_var = llvm::cast<llvm::GlobalVariable>(table);
  init_debug_global(
      "table_" + name,
      get_array_debug_type(
          dity, syms.size(), llvm::DataLayout(module).getABITypeAlign(ty)),
      global_var);
  std::vector<llvm::Constant *> values;
  for (auto entry : syms) {
    auto *symbol = entry.second;
    auto *val = getter(definition, module, symbol);
    values.push_back(val);
  }
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(llvm::ConstantArray::get(table_type, values));
  }
  auto *offset = new llvm::ZExtInst(
      func->arg_begin(), llvm::Type::getInt64Ty(ctx), "", merge_block);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  auto *retval = llvm::GetElementPtrInst::Create(
      table_type, global_var, {zero, offset}, "", merge_block);
  merge_block->insertInto(func);
  auto *load = new llvm::LoadInst(ty, retval, "", merge_block);
  llvm::ReturnInst::Create(ctx, load, merge_block);
  add_abort(stuck, module);
  stuck->insertInto(func);
}

static void emit_data_for_symbol(
    std::string const &name, llvm::Type *ty, llvm::DIType *dity,
    kore_definition *definition, llvm::Module *module, bool is_eval,
    std::pair<llvm::Value *, llvm::BasicBlock *> getter(
        kore_definition *, llvm::Module *, kore_symbol *,
        llvm::Instruction *)) {
  llvm::LLVMContext &ctx = module->getContext();
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(ctx));
  if (is_eval) {
    auto *ty = llvm::PointerType::getUnqual(ctx);
    arg_types.push_back(ty);
  }
  auto *func = get_or_insert_function(
      module, name, llvm::FunctionType::get(ty, arg_types, false));
  if (!is_eval) {
    init_debug_function(
        name, name, get_debug_function_type(dity, {get_int_debug_type()}),
        definition, func);
  }
  auto *entry_block = llvm::BasicBlock::Create(ctx, "entry", func);
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *switch_inst = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, syms.size(), entry_block);
  auto *phi = llvm::PHINode::Create(
      ty, definition->get_symbols().size(), "phi", merge_block);
  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto *symbol = entry.second;
    auto *decl = definition->get_symbol_declarations().at(symbol->get_name());
    bool is_func = decl->attributes().contains(attribute_set::key::Function)
                   || decl->attributes().contains(attribute_set::key::Anywhere);
    if (is_eval && !is_func) {
      continue;
    }
    if (is_eval && !symbol->get_sort()->is_concrete()) {
      continue;
    }
    auto *case_block
        = llvm::BasicBlock::Create(ctx, "tag" + std::to_string(tag), func);
    auto *branch = llvm::BranchInst::Create(merge_block, case_block);
    auto pair = getter(definition, module, symbol, branch);
    phi->addIncoming(pair.first, pair.second);
    switch_inst->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), tag), case_block);
  }
  llvm::ReturnInst::Create(ctx, phi, merge_block);
  merge_block->insertInto(func);
  add_abort(stuck, module);
  stuck->insertInto(func);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> get_header(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::Instruction *inst) {
  auto *block_type = get_block_type(module, definition, symbol);
  return std::make_pair(
      get_block_header(module, definition, symbol, block_type),
      inst->getParent());
}

static void
emit_get_block_header_for_symbol(kore_definition *def, llvm::Module *mod) {
  emit_data_for_symbol(
      "get_block_header_for_symbol",
      llvm::StructType::getTypeByName(mod->getContext(), blockheader_struct),
      get_forward_decl(blockheader_struct), def, mod, false, get_header);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> get_function(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  auto *decl = def->get_symbol_declarations().at(symbol->get_name());
  bool res = decl->attributes().contains(attribute_set::key::Function)
             || decl->attributes().contains(attribute_set::key::Anywhere);
  return std::make_pair(
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res),
      inst->getParent());
}

static void emit_is_symbol_a_function(kore_definition *def, llvm::Module *mod) {
  emit_data_for_symbol(
      "is_symbol_a_function", llvm::Type::getInt1Ty(mod->getContext()),
      get_bool_debug_type(), def, mod, false, get_function);
}

static llvm::Constant *
get_binder(kore_definition *def, llvm::Module *mod, kore_symbol *symbol) {
  auto *decl = def->get_symbol_declarations().at(symbol->get_name());
  bool res = decl->attributes().contains(attribute_set::key::Binder);
  return llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res);
}

static void emit_is_symbol_a_binder(kore_definition *def, llvm::Module *mod) {
  emit_data_table_for_symbol(
      "is_symbol_a_binder", llvm::Type::getInt1Ty(mod->getContext()),
      get_bool_debug_type(), def, mod, get_binder);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> get_injection(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  llvm::Constant *tag
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod->getContext()), 0);
  for (auto sym : def->get_symbols()) {
    if (sym.second->get_name() == "inj"
        && *sym.second->get_arguments()[0] == *symbol->get_sort()) {
      tag = llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(mod->getContext()), sym.first);
      break;
    }
  }
  return std::make_pair(tag, inst->getParent());
}

static void
emit_get_injection_for_sort_of_tag(kore_definition *def, llvm::Module *mod) {
  emit_data_for_symbol(
      "get_injection_for_sort_of_tag",
      llvm::Type::getInt32Ty(mod->getContext()), get_int_debug_type(), def, mod,
      false, get_injection);
}

static llvm::Constant *
get_arity(kore_definition *, llvm::Module *mod, kore_symbol *symbol) {
  return llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(mod->getContext()),
      symbol->get_arguments().size());
}

static void emit_get_symbol_arity(kore_definition *def, llvm::Module *mod) {
  emit_data_table_for_symbol(
      "get_symbol_arity", llvm::Type::getInt32Ty(mod->getContext()),
      get_int_debug_type(), def, mod, get_arity);
}

static llvm::Value *get_arg_value(
    llvm::Value *arguments_array, int idx, llvm::BasicBlock *case_block,
    value_type cat, llvm::Module *mod) {
  llvm::LLVMContext &ctx = mod->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);

  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);

  auto *addr = llvm::GetElementPtrInst::Create(
      llvm::ArrayType::get(ptr_ty, 0), arguments_array,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), idx)}, "",
      case_block);
  llvm::Value *arg = new llvm::LoadInst(ptr_ty, addr, "", case_block);

  switch (cat.cat) {
  case sort_category::Bool:
  case sort_category::MInt: {
    auto *val_ty = getvalue_type(cat, mod);
    auto *cast = new llvm::BitCastInst(arg, ptr_ty, "", case_block);
    auto *load = new llvm::LoadInst(val_ty, cast, "", case_block);
    arg = load;
    break;
  }
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    arg = new llvm::BitCastInst(arg, ptr_ty, "", case_block);
    break;
  case sort_category::Int:
  case sort_category::Float:
  case sort_category::StringBuffer:
  case sort_category::Symbol:
  case sort_category::Variable:
    arg = new llvm::BitCastInst(arg, getvalue_type(cat, mod), "", case_block);
    break;
  case sort_category::MapIter:
  case sort_category::SetIter:
  case sort_category::Uncomputed: abort();
  }
  return arg;
}

static std::pair<llvm::Value *, llvm::BasicBlock *> get_eval(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  llvm::LLVMContext &ctx = mod->getContext();
  llvm::BasicBlock *case_block = inst->getParent();
  inst->removeFromParent();
  llvm::Function *func = case_block->getParent();
  llvm::Value *arguments_array = func->arg_begin() + 1;
  int idx = 0;
  llvm::StringMap<llvm::Value *> subst;
  auto pattern = kore_composite_pattern::create(symbol);
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat
        = dynamic_cast<kore_composite_sort *>(sort.get())->get_category(def);
    llvm::Value *arg
        = get_arg_value(arguments_array, idx, case_block, cat, mod);
    std::string name = "_" + std::to_string(idx++);
    subst.insert({name, arg});
    pattern->add_argument(kore_variable_pattern::create(name, sort));
  }
  create_term creator(subst, def, case_block, mod, false);
  llvm::Value *result = creator(pattern.get()).first;
  llvm::Value *retval = nullptr;
  value_type cat = dynamic_cast<kore_composite_sort *>(symbol->get_sort().get())
                       ->get_category(def);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  switch (cat.cat) {
  case sort_category::Int:
  case sort_category::Float:
  case sort_category::StringBuffer:
  case sort_category::Symbol:
  case sort_category::Variable:
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    retval = new llvm::BitCastInst(
        result, ptr_ty, "", creator.get_current_block());
    break;
  case sort_category::Bool:
  case sort_category::MInt: {
    auto *malloc = create_malloc(
        creator.get_current_block(),
        llvm::ConstantExpr::getSizeOf(result->getType()),
        get_or_insert_function(mod, "malloc", ptr_ty, ptr_ty));
    new llvm::StoreInst(result, malloc, creator.get_current_block());
    retval = new llvm::BitCastInst(
        malloc, ptr_ty, "", creator.get_current_block());
    break;
  }
  case sort_category::MapIter:
  case sort_category::SetIter:
  case sort_category::Uncomputed: abort();
  }
  inst->insertAfter(&creator.get_current_block()->back());
  return std::make_pair(retval, creator.get_current_block());
}

static void
emit_evaluate_function_symbol(kore_definition *def, llvm::Module *mod) {
  emit_data_for_symbol(
      "evaluate_function_symbol",
      llvm::PointerType::getUnqual(mod->getContext()), nullptr, def, mod, true,
      get_eval);
}

static void
emit_get_tag_for_fresh_sort(kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &ctx = module->getContext();
  auto *func = get_or_insert_function(
      module, "get_tag_for_fresh_sort", llvm::Type::getInt32Ty(ctx),
      llvm::PointerType::getUnqual(ctx));
  auto *current_block = llvm::BasicBlock::Create(ctx, "");
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(ctx), definition->get_sort_declarations().size(),
      "phi", merge_block);
  auto const &sorts = definition->get_sort_declarations();
  llvm::Function *strcmp = get_strcmp(module);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
  bool has_case = false;
  for (auto const &entry : sorts) {
    std::string name = entry.first;
    if (!definition->get_fresh_functions().contains(name)) {
      continue;
    }
    has_case = true;
    current_block->insertInto(func);
    current_block->setName("is_" + name);
    auto *str = llvm::ConstantDataArray::getString(ctx, name, true);
    auto *global
        = module->getOrInsertGlobal("sort_name_" + name, str->getType());
    auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
      global_var->setInitializer(str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        str->getType(), global_var, indices);
    auto *compare = llvm::CallInst::Create(
        strcmp, {func->arg_begin(), ptr}, "", current_block);
    auto *icmp = new llvm::ICmpInst(
        *current_block, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto *false_block = llvm::BasicBlock::Create(ctx, "");
    auto *case_block = llvm::BasicBlock::Create(ctx, name, func);
    llvm::BranchInst::Create(case_block, false_block, icmp, current_block);
    auto *symbol = definition->get_fresh_functions().at(name);
    phi->addIncoming(
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(ctx), definition->get_all_symbols()
                                             .at(ast_to_string(*symbol))
                                             ->get_tag()),
        case_block);
    llvm::BranchInst::Create(merge_block, case_block);
    current_block = false_block;
  }
  current_block->insertInto(func);
  add_abort(current_block, module);
  if (has_case) {
    llvm::ReturnInst::Create(ctx, phi, merge_block);
    merge_block->insertInto(func);
  }
}

static void emit_get_token(kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &ctx = module->getContext();
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  auto *get_token_type = llvm::FunctionType::get(
      ptr_ty, {ptr_ty, llvm::Type::getInt64Ty(ctx), ptr_ty}, false);
  auto *func = get_or_insert_function(module, "get_token", get_token_type);
  auto *current_block = llvm::BasicBlock::Create(ctx, "");
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *phi = llvm::PHINode::Create(
      ptr_ty, definition->get_sort_declarations().size(), "phi", merge_block);
  auto const &sorts = definition->get_sort_declarations();
  llvm::Function *strcmp = get_strcmp(module);
  llvm::Function *string_equal = get_or_insert_function(
      module, "string_equal", llvm::Type::getInt1Ty(ctx), ptr_ty, ptr_ty,
      llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
  for (auto const &entry : sorts) {
    std::string name = entry.first;
    if (!entry.second->get_object_sort_variables().empty()) {
      // TODO: MINT in initial configuration
      continue;
    }
    auto sort = kore_composite_sort::create(name);
    value_type cat = sort->get_category(definition);
    if (cat.cat == sort_category::Symbol
        || cat.cat == sort_category::Variable) {
      continue;
    }
    current_block->insertInto(func);
    current_block->setName("is_" + name);
    auto *str = llvm::ConstantDataArray::getString(ctx, name, true);
    auto *global
        = module->getOrInsertGlobal("sort_name_" + name, str->getType());
    auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
      global_var->setInitializer(str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        str->getType(), global_var, indices);
    auto *compare = llvm::CallInst::Create(
        strcmp, {func->arg_begin(), ptr}, "", current_block);
    auto *icmp = new llvm::ICmpInst(
        *current_block, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto *false_block = llvm::BasicBlock::Create(ctx, "");
    auto *case_block = llvm::BasicBlock::Create(ctx, name, func);
    llvm::BranchInst::Create(case_block, false_block, icmp, current_block);
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
    case sort_category::StringBuffer:
    case sort_category::MInt:
      // TODO: tokens
      add_abort(case_block, module);
      break;
    case sort_category::Bool: {
      auto *str = llvm::ConstantDataArray::getString(ctx, "true", false);
      auto *global = module->getOrInsertGlobal("bool_true", str->getType());
      auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
      if (!global_var->hasInitializer()) {
        global_var->setInitializer(str);
      }
      auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
          str->getType(), global_var, indices);
      auto *len = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 4);
      auto *compare = llvm::CallInst::Create(
          string_equal,
          {func->arg_begin() + 2, ptr, func->arg_begin() + 1, len}, "",
          case_block);
      auto *malloc = create_malloc(
          case_block, llvm::ConstantExpr::getSizeOf(compare->getType()),
          get_or_insert_function(module, "malloc", ptr_ty, ptr_ty));
      new llvm::StoreInst(compare, malloc, case_block);
      auto *result = new llvm::BitCastInst(malloc, ptr_ty, "", case_block);
      phi->addIncoming(result, case_block);
      llvm::BranchInst::Create(merge_block, case_block);
      break;
    }
    case sort_category::Float: {
      llvm::Type *float_ty
          = llvm::StructType::getTypeByName(module->getContext(), float_struct);
      llvm::Value *term
          = allocate_term(float_ty, case_block, "kore_alloc_floating");
      llvm::Function *init_float = get_or_insert_function(
          module, "init_float", llvm::Type::getVoidTy(ctx), ptr_ty, ptr_ty);
      llvm::CallInst::Create(
          init_float, {term, func->arg_begin() + 2}, "", case_block);
      auto *cast = new llvm::BitCastInst(term, ptr_ty, "", case_block);
      phi->addIncoming(cast, case_block);
      llvm::BranchInst::Create(merge_block, case_block);
      break;
    }
    case sort_category::Int: {
      auto const &third_arg = func->arg_begin() + 2;
      llvm::Value *first_char = new llvm::LoadInst(
          llvm::Type::getInt8Ty(ctx), third_arg, "", case_block);
      llvm::Constant *ascii_plus
          = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx), 43);
      auto *icmp_first = new llvm::ICmpInst(
          *case_block, llvm::CmpInst::ICMP_EQ, first_char, ascii_plus);
      auto *if_is_plus = llvm::BasicBlock::Create(ctx, "if_is_plus", func);
      auto *else_no_plus = llvm::BasicBlock::Create(ctx, "else_no_plus", func);
      llvm::BranchInst::Create(
          if_is_plus, else_no_plus, icmp_first, case_block);
      llvm::Constant *one
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1);
      llvm::Value *pruned = llvm::GetElementPtrInst::CreateInBounds(
          llvm::Type::getInt8Ty(ctx), func->arg_begin() + 2, {one}, "",
          if_is_plus);
      llvm::BranchInst::Create(else_no_plus, if_is_plus);
      auto *phi_str = llvm::PHINode::Create(ptr_ty, 2, "", else_no_plus);
      phi_str->addIncoming(func->arg_begin() + 2, case_block);
      phi_str->addIncoming(pruned, if_is_plus);
      case_block = else_no_plus;
      llvm::Type *int_ty
          = llvm::StructType::getTypeByName(module->getContext(), int_struct);
      llvm::Value *term
          = allocate_term(int_ty, case_block, "kore_alloc_integer");
      llvm::Function *mpz_init_set = get_or_insert_function(
          module, "__gmpz_init_set_str", llvm::Type::getInt32Ty(ctx), ptr_ty,
          ptr_ty, llvm::Type::getInt32Ty(ctx));
      auto *call = llvm::CallInst::Create(
          mpz_init_set,
          {term, phi_str,
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 10)},
          "", case_block);
      auto *icmp = new llvm::ICmpInst(
          *case_block, llvm::CmpInst::ICMP_EQ, call, zero32);
      auto *abort_block = llvm::BasicBlock::Create(ctx, "invalid_int", func);
      add_abort(abort_block, module);
      auto *cast = new llvm::BitCastInst(term, ptr_ty, "", case_block);
      llvm::BranchInst::Create(merge_block, abort_block, icmp, case_block);
      phi->addIncoming(cast, case_block);
      break;
    }
    case sort_category::Variable:
    case sort_category::Symbol: break;
    case sort_category::MapIter:
    case sort_category::SetIter:
    case sort_category::Uncomputed: abort();
    }
    current_block = false_block;
  }
  current_block->setName("symbol");
  current_block->insertInto(func);
  auto *string_type
      = llvm::StructType::getTypeByName(module->getContext(), string_struct);
  auto *len = llvm::BinaryOperator::Create(
      llvm::Instruction::Add, func->arg_begin() + 1,
      llvm::ConstantExpr::getSizeOf(string_type), "", current_block);
  llvm::Value *block
      = allocate_term(string_type, len, current_block, "kore_alloc_token");
  auto *hdr_ptr = llvm::GetElementPtrInst::CreateInBounds(
      string_type, block, {zero, zero32, zero32}, "", current_block);
  auto *block_size
      = module->getOrInsertGlobal("BLOCK_SIZE", llvm::Type::getInt64Ty(ctx));
  auto *block_size_val = new llvm::LoadInst(
      llvm::Type::getInt64Ty(ctx), block_size, "", current_block);
  auto *block_alloc_size = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, block_size_val,
      llvm::ConstantExpr::getSizeOf(ptr_ty), "", current_block);
  auto *icmp = new llvm::ICmpInst(
      *current_block, llvm::CmpInst::ICMP_UGT, len, block_alloc_size);
  auto *mask = llvm::SelectInst::Create(
      icmp,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), NOT_YOUNG_OBJECT_BIT),
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0), "",
      current_block);
  auto *hdr_ored = llvm::BinaryOperator::Create(
      llvm::Instruction::Or, func->arg_begin() + 1, mask, "", current_block);
  new llvm::StoreInst(hdr_ored, hdr_ptr, current_block);
  llvm::Function *memcpy = get_or_insert_function(
      module, "memcpy", ptr_ty, ptr_ty, ptr_ty, llvm::Type::getInt64Ty(ctx));
  auto *str_ptr = llvm::GetElementPtrInst::CreateInBounds(
      string_type, block,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1), zero}, "",
      current_block);
  llvm::CallInst::Create(
      memcpy, {str_ptr, func->arg_begin() + 2, func->arg_begin() + 1}, "",
      current_block);
  auto *cast = new llvm::BitCastInst(block, ptr_ty, "", current_block);
  llvm::BranchInst::Create(merge_block, current_block);
  phi->addIncoming(cast, current_block);
  llvm::ReturnInst::Create(ctx, phi, merge_block);
  merge_block->insertInto(func);
}

static llvm::StructType *make_packed_visitor_structure_type(
    llvm::LLVMContext &ctx, llvm::Module *module, bool is_serialize) {
  std::string const name = is_serialize ? "serialize_visitor" : "visitor";
  static auto types = std::map<llvm::LLVMContext *, llvm::StructType *>{};

  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);

  if (types.find(&ctx) == types.end()) {
    auto element_types = std::vector<llvm::Type *>(10, ptr_ty);
    if (!is_serialize) {
      element_types.push_back(ptr_ty);
    }
    auto *struct_ty = llvm::StructType::create(ctx, element_types, name);
    types[&ctx] = struct_ty;
  }

  return types.at(&ctx);
}

static void emit_traversal(
    std::string const &name, kore_definition *definition, llvm::Module *module,
    bool is_visitor, bool is_serialize,
    void getter(
        kore_definition *, llvm::Module *, kore_symbol *, llvm::BasicBlock *,
        std::vector<llvm::Value *> const &callbacks)) {
  llvm::LLVMContext &ctx = module->getContext();
  std::vector<llvm::Type *> arg_types;

  auto *block_ty = get_block_type(module);
  arg_types.push_back(block_ty->getPointerTo());
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);

  if (is_visitor) {
    // cf runtime/util/header.h visitChildren
    arg_types.push_back(ptr_ty);
    arg_types.push_back(
        make_packed_visitor_structure_type(ctx, module, is_serialize)
            ->getPointerTo());
    if (!is_serialize) {
      arg_types.push_back(ptr_ty);
    }
  } else {
    arg_types.push_back(ptr_ty);
  }

  auto *func = llvm::cast<llvm::Function>(get_or_insert_function(
      module, name,
      llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), arg_types, false)));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
  auto *entry_block = llvm::BasicBlock::Create(ctx, "entry", func);

  auto *hdr_ptr = llvm::GetElementPtrInst::CreateInBounds(
      block_ty, func->arg_begin(), {zero, zero32, zero32}, "", entry_block);
  auto *hdr = new llvm::LoadInst(
      llvm::Type::getInt64Ty(ctx), hdr_ptr, "", entry_block);
  auto *tag
      = new llvm::TruncInst(hdr, llvm::Type::getInt32Ty(ctx), "", entry_block);

  auto callbacks = std::vector<llvm::Value *>{};
  if (is_visitor) {
    auto *visitor_struct = func->getArg(2);
    auto *visitor_type
        = make_packed_visitor_structure_type(ctx, module, is_serialize);

    for (auto i = 0; i < visitor_type->getNumElements(); ++i) {
      auto *ptr = llvm::GetElementPtrInst::CreateInBounds(
          visitor_type, visitor_struct,
          {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), i)}, "",
          entry_block);

      callbacks.push_back(new llvm::LoadInst(
          visitor_type->getTypeAtIndex(i), ptr, "", entry_block));
    }
  }

  auto *stuck = llvm::BasicBlock::Create(ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *switch_inst
      = llvm::SwitchInst::Create(tag, stuck, syms.size(), entry_block);

  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto *symbol = entry.second;
    if (symbol->get_arguments().empty()) {
      continue;
    }
    auto *case_block
        = llvm::BasicBlock::Create(ctx, "tag" + std::to_string(tag), func);
    switch_inst->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), tag), case_block);
    getter(definition, module, symbol, case_block, callbacks);
    llvm::ReturnInst::Create(ctx, case_block);
  }
  add_abort(stuck, module);
  stuck->insertInto(func);
}

static void get_store(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &) {
  llvm::LLVMContext &ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  llvm::Function *func = case_block->getParent();
  llvm::Value *arguments_array = func->arg_begin() + 1;
  int idx = 0;
  auto *block_type = get_block_type(module, definition, symbol);
  auto *cast = new llvm::BitCastInst(
      func->arg_begin(), llvm::PointerType::getUnqual(ctx), "", case_block);
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat = dynamic_cast<kore_composite_sort *>(sort.get())
                         ->get_category(definition);
    llvm::Value *arg
        = get_arg_value(arguments_array, idx, case_block, cat, module);
    llvm::Type *arg_ty = get_arg_type(cat, module);
    llvm::Value *child_ptr = llvm::GetElementPtrInst::CreateInBounds(
        block_type, cast,
        {zero, llvm::ConstantInt::get(
                   llvm::Type::getInt32Ty(ctx),
                   get_block_offset(definition, symbol, idx++))},
        "", case_block);
    if (is_collection_sort(cat)) {
      arg = new llvm::LoadInst(arg_ty, arg, "", case_block);
    }
    new llvm::StoreInst(arg, child_ptr, case_block);
  }
}

static void
emit_store_symbol_children(kore_definition *definition, llvm::Module *module) {
  emit_traversal(
      "store_symbol_children", definition, module, false, false, get_store);
}

static llvm::Constant *get_symbol_name(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol) {
  return get_symbol_name_ptr(symbol, nullptr, module, true);
}

static void
emit_get_symbol_name_for_tag(kore_definition *def, llvm::Module *mod) {
  emit_data_table_for_symbol(
      "get_symbol_name_for_tag",
      llvm::PointerType::getUnqual(mod->getContext()),
      get_char_ptr_debug_type(), def, mod, get_symbol_name);
}

static void visit_collection(
    kore_definition *definition, llvm::Module *module,
    kore_composite_sort *composite_sort, llvm::Function *func,
    llvm::Value *child_ptr, llvm::BasicBlock *case_block, llvm::Value *callback,
    llvm::Value *state_ptr, bool use_sort_name) {
  llvm::LLVMContext &ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *sort_decl
      = definition->get_sort_declarations().at(composite_sort->get_name());
  llvm::Constant *concat_ptr = nullptr;
  if (sort_decl->attributes().contains(attribute_set::key::Concat)) {
    auto *concat = (kore_composite_pattern *)sort_decl->attributes()
                       .get(attribute_set::key::Concat)
                       ->get_arguments()[0]
                       .get();
    // the attributes don't have tags associated with them, so we get the
    // canonical version of the symbol
    auto *concat_symbol = definition->get_all_symbols().at(
        concat->get_constructor()->get_name() + "{}");
    concat_ptr
        = get_symbol_name_ptr(concat_symbol, nullptr, module, use_sort_name);
  } else {
    if (use_sort_name) {
      concat_ptr = llvm::ConstantPointerNull::get(ptr_ty);
    } else {
      concat_ptr = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
    }
  }
  auto *unit = (kore_composite_pattern *)sort_decl->attributes()
                   .get(attribute_set::key::Unit)
                   ->get_arguments()[0]
                   .get();
  // the attributes don't have tags associated with them, so we get the
  // canonical version of the symbol
  auto *unit_symbol = definition->get_all_symbols().at(
      unit->get_constructor()->get_name() + "{}");
  auto *unit_ptr
      = get_symbol_name_ptr(unit_symbol, nullptr, module, use_sort_name);
  auto *element = (kore_composite_pattern *)sort_decl->attributes()
                      .get(attribute_set::key::Element)
                      ->get_arguments()[0]
                      .get();
  // the attributes don't have tags associated with them, so we get the
  // canonical version of the symbol
  auto *element_symbol = definition->get_all_symbols().at(
      element->get_constructor()->get_name() + "{}");
  auto *element_ptr
      = get_symbol_name_ptr(element_symbol, nullptr, module, use_sort_name);
  llvm::Type *sort_type = nullptr;
  if (use_sort_name) {
    sort_type = ptr_ty;
  } else {
    sort_type = llvm::Type::getInt32Ty(ctx);
  }
  auto *fn_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(ctx),
      {ptr_ty, child_ptr->getType(), sort_type, sort_type, sort_type, ptr_ty},
      false);
  llvm::CallInst::Create(
      fn_type, callback,
      {func->arg_begin() + 1, child_ptr, unit_ptr, element_ptr, concat_ptr,
       state_ptr},
      "", case_block);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
static void get_visitor(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &callbacks,
    bool use_sort_name) {
  llvm::LLVMContext &ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  llvm::Function *func = case_block->getParent();
  int idx = 0;
  auto *block_type = get_block_type(module, definition, symbol);
  auto *cast = new llvm::BitCastInst(func->arg_begin(), ptr_ty, "", case_block);
  unsigned i = 0;

  auto *state_ptr = func->arg_end() - 1;

  for (auto const &sort : symbol->get_arguments()) {
    auto *composite_sort = dynamic_cast<kore_composite_sort *>(sort.get());
    value_type cat = composite_sort->get_category(definition);
    llvm::Value *child_ptr = llvm::GetElementPtrInst::CreateInBounds(
        block_type, cast,
        {zero, llvm::ConstantInt::get(
                   llvm::Type::getInt32Ty(ctx),
                   get_block_offset(definition, symbol, idx++))},
        "", case_block);
    llvm::Value *child = new llvm::LoadInst(
        getvalue_type(cat, module), child_ptr, "", case_block);
    llvm::Constant *sort_val = nullptr;
    llvm::Type *sort_type = nullptr;
    if (use_sort_name) {
      auto sort_name = ast_to_string(*sort);
      auto *str = llvm::ConstantDataArray::getString(ctx, sort_name, true);
      auto *global = module->getOrInsertGlobal(
          fmt::format("sort_name_{}", sort_name), str->getType());
      auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
      if (!global_var->hasInitializer()) {
        global_var->setInitializer(str);
      }
      sort_val = llvm::ConstantExpr::getInBoundsGetElementPtr(
          str->getType(), global, indices);
      sort_type = ptr_ty;
    } else {
      uint32_t const num_tags = definition->get_symbols().size();
      uint32_t ordinal
          = dynamic_cast<kore_composite_sort *>(sort.get())->get_ordinal();
      sort_type = llvm::Type::getInt32Ty(ctx);
      sort_val = llvm::ConstantInt::get(sort_type, ordinal + num_tags);
    }
    switch (cat.cat) {
    case sort_category::Variable:
    case sort_category::Symbol:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx),
              {ptr_ty, child->getType(), sort_type, llvm::Type::getInt1Ty(ctx),
               ptr_ty},
              false),
          callbacks.at(0),
          {func->arg_begin() + 1, child, sort_val,
           llvm::ConstantInt::get(
               llvm::Type::getInt1Ty(ctx), cat.cat == sort_category::Variable),
           state_ptr},
          "", case_block);
      break;
    case sort_category::Int:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx),
              {ptr_ty, child->getType(), sort_type, ptr_ty}, false),
          callbacks.at(4), {func->arg_begin() + 1, child, sort_val, state_ptr},
          "", case_block);
      break;
    case sort_category::Float:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx),
              {ptr_ty, child->getType(), sort_type, ptr_ty}, false),
          callbacks.at(5), {func->arg_begin() + 1, child, sort_val, state_ptr},
          "", case_block);
      break;
    case sort_category::Bool:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx),
              {ptr_ty, child->getType(), sort_type, ptr_ty}, false),
          callbacks.at(6), {func->arg_begin() + 1, child, sort_val, state_ptr},
          "", case_block);
      break;
    case sort_category::StringBuffer:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx),
              {ptr_ty, child->getType(), sort_type, ptr_ty}, false),
          callbacks.at(7), {func->arg_begin() + 1, child, sort_val, state_ptr},
          "", case_block);
      break;
    case sort_category::MInt: {
      llvm::Value *mint = new llvm::LoadInst(
          get_arg_type(cat, module), child_ptr, "mint", case_block);
      size_t nwords = (cat.bits + 63) / 64;
      auto *nbits
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), cat.bits);
      auto *fn_type = llvm::FunctionType::get(
          llvm::Type::getVoidTy(ctx),
          {ptr_ty, ptr_ty, llvm::Type::getInt64Ty(ctx), sort_type, ptr_ty},
          false);
      if (nwords == 0) {
        llvm::CallInst::Create(
            fn_type, func->arg_begin() + 10,
            {func->arg_begin() + 1, llvm::ConstantPointerNull::get(ptr_ty),
             nbits, sort_val, state_ptr},
            "", case_block);
      } else {
        auto *ptr = allocate_term(
            llvm::Type::getInt64Ty(ctx),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), nwords * 8),
            case_block, "kore_alloc_always_gc");
        if (nwords == 1) {
          llvm::Value *word = nullptr;
          if (cat.bits == 64) {
            word = mint;
          } else {
            word = new llvm::ZExtInst(
                mint, llvm::Type::getInt64Ty(ctx), "word", case_block);
          }
          new llvm::StoreInst(word, ptr, case_block);
        } else { // nwords >= 2
          llvm::Value *ptr2 = ptr;
          llvm::Value *accum = mint;
          for (size_t i = 0; i < nwords; i++) {
            auto *word = new llvm::TruncInst(
                accum, llvm::Type::getInt64Ty(ctx), "word", case_block);
            new llvm::StoreInst(word, ptr2, case_block);
            ptr2 = llvm::GetElementPtrInst::Create(
                llvm::Type::getInt64Ty(ctx), ptr2,
                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 1)}, "ptr",
                case_block);
            accum = llvm::BinaryOperator::Create(
                llvm::Instruction::LShr, accum,
                llvm::ConstantInt::get(mint->getType(), 64), "shift",
                case_block);
          }
        }
        llvm::CallInst::Create(
            fn_type, callbacks.at(8),
            {func->arg_begin() + 1, ptr, nbits, sort_val, state_ptr}, "",
            case_block);
      }
      break;
    }
    case sort_category::Map:
      visit_collection(
          definition, module, composite_sort, func, child_ptr, case_block,
          callbacks.at(1), state_ptr, use_sort_name);
      break;
    case sort_category::RangeMap:
      visit_collection(
          definition, module, composite_sort, func, child_ptr, case_block,
          callbacks.at(10), state_ptr, use_sort_name);
      break;
    case sort_category::Set:
      visit_collection(
          definition, module, composite_sort, func, child_ptr, case_block,
          callbacks.at(3), state_ptr, use_sort_name);
      break;
    case sort_category::List: {
      visit_collection(
          definition, module, composite_sort, func, child_ptr, case_block,
          callbacks.at(2), state_ptr, use_sort_name);
      break;
    }
    case sort_category::MapIter:
    case sort_category::SetIter:
    case sort_category::Uncomputed: abort();
    }
    if (i != symbol->get_arguments().size() - 1 && use_sort_name) {
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(ctx), {ptr_ty, ptr_ty}, false),
          callbacks.at(9), {func->arg_begin() + 1, state_ptr}, "", case_block);
    }
    i++;
  }
}

// NOLINTNEXTLINE(*-cognitive-complexity)
static void get_visitor(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &callbacks) {
  get_visitor(definition, module, symbol, case_block, callbacks, true);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
static void get_serialize_visitor(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &callbacks) {
  get_visitor(definition, module, symbol, case_block, callbacks, false);
}

static llvm::Constant *get_layout_data(
    uint16_t layout, kore_symbol *symbol, llvm::Module *module,
    kore_definition *def) {
  uint8_t len = symbol->get_arguments().size();
  std::vector<llvm::Constant *> elements;
  llvm::LLVMContext &ctx = module->getContext();
  auto *block_type = get_block_type(module, def, symbol);
  int i = 0;
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat
        = dynamic_cast<kore_composite_sort *>(sort.get())->get_category(def);
    auto *offset = get_offset_of_member(
        module, block_type, get_block_offset(def, symbol, i++));
    elements.push_back(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(
            module->getContext(), layoutitem_struct),
        offset,
        llvm::ConstantInt::get(
            llvm::Type::getInt16Ty(ctx), (int)cat.cat + cat.bits)));
  }
  auto *arr = llvm::ConstantArray::get(
      llvm::ArrayType::get(
          llvm::StructType::getTypeByName(
              module->getContext(), layoutitem_struct),
          len),
      elements);
  auto *global = module->getOrInsertGlobal(
      "layout_item_" + std::to_string(layout), arr->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(arr);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      arr->getType(), global_var, indices);
  std::string name = "layout_" + std::to_string(layout);
  auto *global2 = module->getOrInsertGlobal(
      name,
      llvm::StructType::getTypeByName(module->getContext(), layout_struct));
  auto *global_var2 = llvm::cast<llvm::GlobalVariable>(global2);
  init_debug_global(name, get_forward_decl(layout_struct), global_var2);
  if (!global_var2->hasInitializer()) {
    global_var2->setInitializer(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(module->getContext(), layout_struct),
        llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx), len), ptr));
  }
  return global_var2;
}

static void emit_layouts(kore_definition *definition, llvm::Module *module) {
  std::map<uint16_t, kore_symbol *> layouts;
  for (auto entry : definition->get_symbols()) {
    layouts[entry.second->get_layout()] = entry.second;
  }
  llvm::LLVMContext &ctx = module->getContext();
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt16Ty(ctx));
  auto *func = llvm::cast<llvm::Function>(get_or_insert_function(
      module, "get_layout_data",
      llvm::FunctionType::get(ptr_ty, arg_types, false)));
  init_debug_function(
      "get_layout_data", "get_layout_data",
      get_debug_function_type(
          get_pointer_debug_type(get_forward_decl(layout_struct), "layout *"),
          {get_short_debug_type()}),
      definition, func);
  auto *entry_block = llvm::BasicBlock::Create(ctx, "entry", func);
  auto *merge_block = llvm::BasicBlock::Create(ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(ctx, "stuck");
  auto *switch_inst = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, layouts.size(), entry_block);
  auto *phi = llvm::PHINode::Create(ptr_ty, layouts.size(), "phi", merge_block);
  for (auto entry : layouts) {
    uint16_t layout = entry.first;
    auto *symbol = entry.second;
    auto *case_block = llvm::BasicBlock::Create(
        ctx, "layout" + std::to_string(layout), func);
    llvm::BranchInst::Create(merge_block, case_block);
    auto *data = get_layout_data(layout, symbol, module, definition);
    phi->addIncoming(data, case_block);
    switch_inst->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt16Ty(ctx), layout),
        case_block);
  }
  llvm::ReturnInst::Create(ctx, phi, merge_block);
  merge_block->insertInto(func);
  add_abort(stuck, module);
  stuck->insertInto(func);
}

static void emit_visit_children(kore_definition *def, llvm::Module *mod) {
  emit_traversal("visit_children", def, mod, true, false, get_visitor);
}

static void
emit_visit_children_for_serialize(kore_definition *def, llvm::Module *mod) {
  emit_traversal(
      "visit_children_for_serialize", def, mod, true, true,
      get_serialize_visitor);
}

static void emit_inj_tags(kore_definition *def, llvm::Module *mod) {
  llvm::LLVMContext &ctx = mod->getContext();
  auto *global
      = mod->getOrInsertGlobal("FIRST_INJ_TAG", llvm::Type::getInt32Ty(ctx));
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
  global_var->setConstant(true);
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(ctx), def->get_inj_symbol()->get_first_tag()));
  }
  global = mod->getOrInsertGlobal("LAST_INJ_TAG", llvm::Type::getInt32Ty(ctx));
  global_var = llvm::cast<llvm::GlobalVariable>(global);
  global_var->setConstant(true);
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(ctx), def->get_inj_symbol()->get_last_tag()));
  }
}

static void emit_sort_table_v2(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto *subtable_type = llvm::ArrayType::get(
        llvm::Type::getInt32Ty(ctx), symbol->get_arguments().size());
    auto *subtable = module->getOrInsertGlobal(
        fmt::format("sort_tags_{}", ast_to_string(*symbol)), subtable_type);
    auto *subtable_var = llvm::dyn_cast<llvm::GlobalVariable>(subtable);
    init_debug_global(
        "sort_tags_" + symbol->get_name(),
        get_array_debug_type(
            get_int_debug_type(), symbol->get_arguments().size(),
            llvm::DataLayout(module).getABITypeAlign(
                llvm::Type::getInt32Ty(ctx))),
        subtable_var);
    llvm::Constant *zero
        = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    const uint32_t num_tags = definition->get_symbols().size();

    std::vector<llvm::Constant *> subvalues;
    for (const auto &i : symbol->get_arguments()) {
      subvalues.push_back(llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(ctx),
          dynamic_cast<kore_composite_sort *>(i.get())->get_ordinal()
              + num_tags));
    }
    subtable_var->setInitializer(
        llvm::ConstantArray::get(subtable_type, subvalues));

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        subtable_type, subtable_var, indices);
  };

  auto *entry_ty = llvm::PointerType::getUnqual(mod->getContext());
  auto *debug_ty = get_pointer_debug_type(get_int_debug_type(), "int *");

  emit_data_table_for_symbol(
      "get_argument_sorts_for_tag_v2", entry_ty, debug_ty, def, mod, getter);
}

static void emit_sort_table(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
    auto *subtable_type
        = llvm::ArrayType::get(ptr_ty, symbol->get_arguments().size());
    auto *subtable = module->getOrInsertGlobal(
        fmt::format("sorts_{}", ast_to_string(*symbol)), subtable_type);
    auto *subtable_var = llvm::dyn_cast<llvm::GlobalVariable>(subtable);
    init_debug_global(
        "sorts_" + symbol->get_name(),
        get_array_debug_type(
            get_char_ptr_debug_type(), symbol->get_arguments().size(),
            llvm::DataLayout(module).getABITypeAlign(ptr_ty)),
        subtable_var);
    llvm::Constant *zero
        = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
    auto indices = std::vector<llvm::Constant *>{zero, zero};

    std::vector<llvm::Constant *> subvalues;
    for (const auto &i : symbol->get_arguments()) {
      auto arg_str = ast_to_string(*i);
      auto *str_type = llvm::ArrayType::get(
          llvm::Type::getInt8Ty(ctx), arg_str.size() + 1);
      auto *sort_name = module->getOrInsertGlobal(
          fmt::format("sort_name_{}", arg_str), str_type);
      subvalues.push_back(llvm::ConstantExpr::getInBoundsGetElementPtr(
          str_type, sort_name, indices));
    }
    subtable_var->setInitializer(
        llvm::ConstantArray::get(subtable_type, subvalues));

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        subtable_type, subtable_var, indices);
  };

  auto *ptr_ty = llvm::PointerType::getUnqual(mod->getContext());
  auto *debug_ty = get_pointer_debug_type(get_char_ptr_debug_type(), "char **");

  emit_data_table_for_symbol(
      "get_argument_sorts_for_tag", ptr_ty, debug_ty, def, mod, getter);
}

/*
 * Emit a table mapping symbol tags to the declared return sort for that symbol.
 * For example:
 *
 *   tag_of(initGeneratedTopCell) |-> sort_name_SortGeneratedTopCell{}
 *
 * Each value in the table is a pointer to a global variable containing the
 * relevant sort name as a null-terminated string.
 */
static void emit_return_sort_table(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto sort = symbol->get_sort();
    auto sort_str = ast_to_string(*sort);

    auto *char_type = llvm::Type::getInt8Ty(ctx);
    auto *str_type = llvm::ArrayType::get(char_type, sort_str.size() + 1);

    auto *str = llvm::ConstantDataArray::getString(ctx, sort_str, true);
    auto *sort_name
        = module->getOrInsertGlobal("sort_name_" + sort_str, str_type);
    auto *global_var = llvm::cast<llvm::GlobalVariable>(sort_name);
    if (!global_var->hasInitializer()) {
      global_var->setInitializer(str);
    }

    auto *i64_type = llvm::Type::getInt64Ty(ctx);
    auto *zero = llvm::ConstantInt::get(i64_type, 0);

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        str_type, sort_name, std::vector<llvm::Constant *>{zero});
  };

  emit_data_table_for_symbol(
      "get_return_sort_for_tag",
      llvm::PointerType::getUnqual(mod->getContext()),
      get_char_ptr_debug_type(), def, mod, getter);
}

/*
 * Emit a table that records whether the symbol corresponding to a particular
 * tag has instantiated sort parameters. For example:
 *
 *   tag_of(inj{SortA{}, SortKItem{}}) |-> true
 *   tag_of(someSymbol{})              |-> false
 */
static void
emit_symbol_is_instantiation(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto *bool_ty = llvm::Type::getInt1Ty(ctx);

    return llvm::ConstantInt::get(
        bool_ty, !symbol->get_formal_arguments().empty());
  };

  emit_data_table_for_symbol(
      "symbol_is_instantiation", llvm::Type::getInt1Ty(mod->getContext()),
      get_bool_debug_type(), def, mod, getter);
}

void emit_config_parser_functions(
    kore_definition *definition, llvm::Module *module) {
  emit_get_tag_for_symbol_name(definition, module);
  emit_get_block_header_for_symbol(definition, module);
  emit_is_symbol_a_function(definition, module);
  emit_is_symbol_a_binder(definition, module);
  emit_store_symbol_children(definition, module);
  emit_evaluate_function_symbol(definition, module);
  emit_get_token(definition, module);
  emit_get_tag_for_fresh_sort(definition, module);
  emit_get_injection_for_sort_of_tag(definition, module);
  emit_get_symbol_arity(definition, module);

  emit_get_symbol_name_for_tag(definition, module);
  emit_visit_children(definition, module);
  emit_visit_children_for_serialize(definition, module);

  emit_layouts(definition, module);

  emit_inj_tags(definition, module);

  emit_sort_table(definition, module);
  emit_sort_table_v2(definition, module);
  emit_return_sort_table(definition, module);
  emit_symbol_is_instantiation(definition, module);
}

} // namespace kllvm
