#include <kllvm/ast/AST.h>

#include <iostream>
#include <string>
#include <unordered_set>

namespace kllvm {

namespace {

template <typename Elem, typename Hash, typename Equal>
std::unordered_map<Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
transitive_closure(std::unordered_map<
                   Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
                       relations) {
  bool dirty = false;
  do {
    dirty = false;
    for (auto &entry : relations) {
      SortSet new_succ;
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
    if (auto *sort_decl
        = dynamic_cast<kore_composite_sort_declaration *>(decl.get())) {
      sort_declarations_.insert({sort_decl->get_name(), sort_decl});
      auto sort = kore_composite_sort::create(sort_decl->get_name());
    } else if (
        auto *symbol_decl
        = dynamic_cast<kore_symbol_declaration *>(decl.get())) {
      symbol_declarations_.insert(
          {symbol_decl->get_symbol()->get_name(), symbol_decl});
    } else if (
        auto *alias_decl = dynamic_cast<kore_alias_declaration *>(decl.get())) {
      alias_declarations_.insert(
          {alias_decl->get_symbol()->get_name(), alias_decl});
    } else if (
        auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get())) {
      axioms_.push_back(axiom);
    }
  }
  modules_.push_back(std::move(module));
}

std::string get_raw_symbol_name(sort_category cat) {
  return "rawCollection_" + std::to_string((int)cat);
}

void kore_definition::insert_reserved_symbols() {
  auto mod = kore_module::create("K-RAW-TERM");
  auto decl = kore_symbol_declaration::create("rawTerm", true);
  auto kitem = kore_composite_sort::create("SortKItem");

  decl->get_symbol()->add_sort(kitem);
  decl->get_symbol()->add_argument(kitem);
  mod->add_declaration(std::move(decl));

  for (auto const &cat : hooked_sorts_) {
    switch (cat.first.cat) {
    case sort_category::Map:
    case sort_category::List:
    case sort_category::Set:
    case sort_category::RangeMap: {
      auto decl = kore_symbol_declaration::create(
          "rawCollection_" + std::to_string((int)cat.first.cat), true);
      auto sort = cat.second;
      decl->get_symbol()->add_sort(kitem);
      decl->get_symbol()->add_argument(sort);
      mod->add_declaration(std::move(decl));
      break;
    }
    default: break;
    }
  }

  add_module(std::move(mod));
}

SubsortMap kore_definition::get_subsorts() const {
  auto subsorts = SubsortMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::Subsort)) {
      auto const &att = axiom->attributes().get(attribute_set::key::Subsort);
      auto const &inner_sort
          = att->get_constructor()->get_formal_arguments()[0];
      auto const &outer_sort
          = att->get_constructor()->get_formal_arguments()[1];
      subsorts[inner_sort.get()].insert(outer_sort.get());
    }
  }

  return transitive_closure(subsorts);
}

SymbolMap kore_definition::get_overloads() const {
  auto overloads = SymbolMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::SymbolOverload)) {
      auto const &att
          = axiom->attributes().get(attribute_set::key::SymbolOverload);
      auto *inner_symbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                               att->get_arguments()[1])
                               ->get_constructor();
      auto *outer_symbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                               att->get_arguments()[0])
                               ->get_constructor();
      overloads[inner_symbol].insert(outer_symbol);
    }
  }

  return transitive_closure(overloads);
}

// NOLINTNEXTLINE(*-function-cognitive-complexity)
void kore_definition::preprocess() {
  for (auto *axiom : axioms_) {
    axiom->pattern_ = axiom->pattern_->expand_aliases(this);
  }
  auto symbols = std::map<std::string, std::vector<kore_symbol *>>{};
  unsigned next_ordinal = 0;
  for (auto iter = axioms_.begin(); iter != axioms_.end();) {
    auto *axiom = *iter;
    axiom->ordinal_ = next_ordinal;
    ordinals_[next_ordinal++] = axiom;
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

  for (auto const &entry : symbols) {
    for (auto *symbol : entry.second) {
      for (auto const &sort : symbol->get_arguments()) {
        if (sort->is_concrete()) {
          hooked_sorts_[dynamic_cast<kore_composite_sort *>(sort.get())
                            ->get_category(this)]
              = std::dynamic_pointer_cast<kore_composite_sort>(sort);
        }
      }
      if (symbol->get_sort()->is_concrete()) {
        hooked_sorts_[dynamic_cast<kore_composite_sort *>(
                          symbol->get_sort().get())
                          ->get_category(this)]
            = std::dynamic_pointer_cast<kore_composite_sort>(
                symbol->get_sort());
      }
    }
  }

  insert_reserved_symbols();

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

  for (auto const &decl : symbol_declarations_) {
    if (decl.second->attributes().contains(
            attribute_set::key::FreshGenerator)) {
      auto sort = decl.second->get_symbol()->get_sort();
      if (sort->is_concrete()) {
        fresh_functions_[dynamic_cast<kore_composite_sort *>(sort.get())
                             ->get_name()]
            = decl.second->get_symbol();
      }
    }
  }

  uint32_t next_symbol = 0;
  uint16_t next_layout = 1;
  auto instantiations
      = std::unordered_map<kore_symbol, uint32_t, hash_symbol>{};
  auto layouts = std::unordered_map<std::string, uint16_t>{};
  auto variables
      = std::unordered_map<std::string, std::pair<uint32_t, uint32_t>>{};
  for (auto const &entry : symbols) {
    uint32_t first_tag = next_symbol;
    for (auto *symbol : entry.second) {
      if (symbol->is_concrete()) {
        if (!instantiations.contains(*symbol)) {
          instantiations.emplace(*symbol, next_symbol++);
        }
        std::string layout_str = symbol->layout_string(this);
        if (!layouts.contains(layout_str)) {
          layouts.emplace(layout_str, next_layout++);
        }
        symbol->first_tag_ = symbol->last_tag_ = instantiations.at(*symbol);
        symbol->layout_ = layouts.at(layout_str);
        object_symbols_[symbol->first_tag_] = symbol;
        all_object_symbols_[ast_to_string(*symbol)] = symbol;
      }
    }
    uint32_t last_tag = next_symbol - 1;
    if (!entry.second.empty()) {
      variables.emplace(
          entry.first, std::pair<uint32_t, uint32_t>{first_tag, last_tag});
    }
  }
  for (auto const &entry : symbols) {
    auto range = variables.at(entry.first);
    for (auto *symbol : entry.second) {
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
