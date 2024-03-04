#include <kllvm/ast/AST.h>

#include <iostream>
#include <string>
#include <unordered_set>

namespace kllvm {

namespace {

template <typename Elem, typename Hash, typename Equal>
std::unordered_map<Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
transitiveClosure(std::unordered_map<
                  Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
                      relations) {
  bool dirty = false;
  do {
    dirty = false;
    for (auto &entry : relations) {
      SortSet newSucc;
      for (auto &elem : entry.second) {
        auto &relation = relations[elem];
        for (auto *elem2 : relation) {
          dirty |= relations[entry.first].insert(elem2).second;
        }
      }
    }
  } while (dirty);
  return relations;
}

} // namespace

std::unordered_set<std::string>
kore_definition::get_sorts_hooked_to(std::string const &hook_name) const {
  auto ret = std::unordered_set<std::string>{};

  for (auto const &[name, decl] : get_sort_declarations()) {
    if (decl->is_hooked()) {
      if (auto hook = decl->attributes().get_string(attribute_set::key::Hook);
          hook == hook_name) {
        ret.insert(name);
      }
    }
  }

  return ret;
}

void kore_definition::add_module(sptr<kore_module> module) {
  for (auto const &decl : module->get_declarations()) {
    if (auto *sortDecl
        = dynamic_cast<kore_composite_sort_declaration *>(decl.get())) {
      sort_declarations_.insert({sortDecl->get_name(), sortDecl});
      auto sort = kore_composite_sort::create(sortDecl->get_name());
    } else if (
        auto *symbolDecl = dynamic_cast<kore_symbol_declaration *>(decl.get())) {
      symbol_declarations_.insert(
          {symbolDecl->get_symbol()->get_name(), symbolDecl});
    } else if (
        auto *aliasDecl = dynamic_cast<kore_alias_declaration *>(decl.get())) {
      alias_declarations_.insert({aliasDecl->get_symbol()->get_name(), aliasDecl});
    } else if (auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get())) {
      axioms_.push_back(axiom);
    }
  }
  modules_.push_back(std::move(module));
}

void kore_definition::insert_reserved_symbols() {
  auto mod = kore_module::create("K-RAW-TERM");
  auto decl = kore_symbol_declaration::create("rawTerm", true);
  auto sort = kore_composite_sort::create("SortKItem");

  decl->get_symbol()->add_sort(sort);
  decl->get_symbol()->add_argument(sort);
  mod->add_declaration(std::move(decl));

  add_module(std::move(mod));
}

SubsortMap kore_definition::get_subsorts() const {
  auto subsorts = SubsortMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::Subsort)) {
      auto const &att = axiom->attributes().get(attribute_set::key::Subsort);
      auto const &innerSort = att->get_constructor()->get_formal_arguments()[0];
      auto const &outerSort = att->get_constructor()->get_formal_arguments()[1];
      subsorts[innerSort.get()].insert(outerSort.get());
    }
  }

  return transitiveClosure(subsorts);
}

SymbolMap kore_definition::get_overloads() const {
  auto overloads = SymbolMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::SymbolOverload)) {
      auto const &att
          = axiom->attributes().get(attribute_set::key::SymbolOverload);
      auto *innerSymbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                              att->get_arguments()[1])
                              ->get_constructor();
      auto *outerSymbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                              att->get_arguments()[0])
                              ->get_constructor();
      overloads[innerSymbol].insert(outerSymbol);
    }
  }

  return transitiveClosure(overloads);
}

// NOLINTNEXTLINE(*-function-cognitive-complexity)
void kore_definition::preprocess() {
  insert_reserved_symbols();

  for (auto *axiom : axioms_) {
    axiom->pattern_ = axiom->pattern_->expand_aliases(this);
  }
  auto symbols = std::map<std::string, std::vector<kore_symbol *>>{};
  unsigned nextOrdinal = 0;
  for (auto const &decl : symbol_declarations_) {
    if (decl.second->attributes().contains(
            attribute_set::key::FreshGenerator)) {
      auto sort = decl.second->get_symbol()->get_sort();
      if (sort->is_concrete()) {
        fresh_functions_[dynamic_cast<kore_composite_sort *>(sort.get())->get_name()]
            = decl.second->get_symbol();
      }
    }
  }
  for (auto iter = axioms_.begin(); iter != axioms_.end();) {
    auto *axiom = *iter;
    axiom->ordinal_ = nextOrdinal;
    ordinals_[nextOrdinal++] = axiom;
    axiom->pattern_->mark_symbols(symbols);
    if (!axiom->is_required()) {
      iter = axioms_.erase(iter);
    } else {
      ++iter;
    }
  }
  for (auto &module : modules_) {
    auto const &declarations = module->get_declarations();
    for (auto const &declaration : declarations) {
      auto *decl = dynamic_cast<kore_symbol_declaration *>(declaration.get());
      if (decl == nullptr) {
        continue;
      }
      if (decl->is_hooked() && decl->get_object_sort_variables().empty()) {
        kore_symbol *symbol = decl->get_symbol();
        symbols.emplace(symbol->get_name(), std::vector<kore_symbol *>{symbol});
      }
    }
  }
  for (auto const &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = symbol_declarations_.at(symbol->get_name());
      symbol->instantiate_symbol(decl);
    }
  }
  uint32_t nextSymbol = 0;
  uint16_t nextLayout = 1;
  auto instantiations = std::unordered_map<kore_symbol, uint32_t, hash_symbol>{};
  auto layouts = std::unordered_map<std::string, uint16_t>{};
  auto variables
      = std::unordered_map<std::string, std::pair<uint32_t, uint32_t>>{};
  for (auto const &entry : symbols) {
    uint32_t firstTag = nextSymbol;
    for (auto *symbol : entry.second) {
      if (symbol->is_concrete()) {
        if (!instantiations.contains(*symbol)) {
          instantiations.emplace(*symbol, nextSymbol++);
        }
        std::string layoutStr = symbol->layout_string(this);
        if (!layouts.contains(layoutStr)) {
          layouts.emplace(layoutStr, nextLayout++);
        }
        symbol->first_tag_ = symbol->last_tag_ = instantiations.at(*symbol);
        symbol->layout_ = layouts.at(layoutStr);
        object_symbols_[symbol->first_tag_] = symbol;
        all_object_symbols_[ast_to_string(*symbol)] = symbol;
      }
    }
    uint32_t lastTag = nextSymbol - 1;
    if (!entry.second.empty()) {
      variables.emplace(
          entry.first, std::pair<uint32_t, uint32_t>{firstTag, lastTag});
    }
  }
  for (auto const &entry : symbols) {
    auto range = variables.at(entry.first);
    for (auto *symbol : entry.second) {
      for (auto const &sort : symbol->get_arguments()) {
        if (sort->is_concrete()) {
          hooked_sorts_[dynamic_cast<kore_composite_sort *>(sort.get())
                          ->get_category(this)]
              = std::dynamic_pointer_cast<kore_composite_sort>(sort);
        }
      }
      if (symbol->get_sort()->is_concrete()) {
        hooked_sorts_[dynamic_cast<kore_composite_sort *>(symbol->get_sort().get())
                        ->get_category(this)]
            = std::dynamic_pointer_cast<kore_composite_sort>(symbol->get_sort());
      }
      if (!symbol->is_concrete()) {
        if (symbol->is_polymorphic()) {
          symbol->first_tag_ = range.first;
          symbol->last_tag_ = range.second;
          auto *decl = symbol_declarations_.at(symbol->get_name());
          if (decl->attributes().contains(attribute_set::key::SortInjection)) {
            inj_symbol_ = symbol;
          }
        }
      }
    }
  }
}

} // namespace kllvm
