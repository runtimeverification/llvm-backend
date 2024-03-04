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
kore_definition::getSortsHookedTo(std::string const &hook_name) const {
  auto ret = std::unordered_set<std::string>{};

  for (auto const &[name, decl] : getSortDeclarations()) {
    if (decl->isHooked()) {
      if (auto hook = decl->attributes().get_string(attribute_set::key::hook);
          hook == hook_name) {
        ret.insert(name);
      }
    }
  }

  return ret;
}

void kore_definition::addModule(sptr<kore_module> module) {
  for (auto const &decl : module->getDeclarations()) {
    if (auto *sortDecl
        = dynamic_cast<kore_composite_sort_declaration *>(decl.get())) {
      sort_declarations_.insert({sortDecl->getName(), sortDecl});
      auto sort = kore_composite_sort::Create(sortDecl->getName());
    } else if (
        auto *symbolDecl = dynamic_cast<kore_symbol_declaration *>(decl.get())) {
      symbol_declarations_.insert(
          {symbolDecl->getSymbol()->getName(), symbolDecl});
    } else if (
        auto *aliasDecl = dynamic_cast<kore_alias_declaration *>(decl.get())) {
      alias_declarations_.insert({aliasDecl->getSymbol()->getName(), aliasDecl});
    } else if (auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get())) {
      axioms_.push_back(axiom);
    }
  }
  modules_.push_back(std::move(module));
}

void kore_definition::insertReservedSymbols() {
  auto mod = kore_module::Create("K-RAW-TERM");
  auto decl = kore_symbol_declaration::Create("rawTerm", true);
  auto sort = kore_composite_sort::Create("SortKItem");

  decl->getSymbol()->addSort(sort);
  decl->getSymbol()->addArgument(sort);
  mod->addDeclaration(std::move(decl));

  addModule(std::move(mod));
}

SubsortMap kore_definition::getSubsorts() const {
  auto subsorts = SubsortMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::subsort)) {
      auto const &att = axiom->attributes().get(attribute_set::key::subsort);
      auto const &innerSort = att->getConstructor()->getFormalArguments()[0];
      auto const &outerSort = att->getConstructor()->getFormalArguments()[1];
      subsorts[innerSort.get()].insert(outerSort.get());
    }
  }

  return transitiveClosure(subsorts);
}

SymbolMap kore_definition::getOverloads() const {
  auto overloads = SymbolMap{};

  for (auto *axiom : axioms_) {
    if (axiom->attributes().contains(attribute_set::key::symbol_overload)) {
      auto const &att
          = axiom->attributes().get(attribute_set::key::symbol_overload);
      auto *innerSymbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                              att->getArguments()[1])
                              ->getConstructor();
      auto *outerSymbol = std::dynamic_pointer_cast<kore_composite_pattern>(
                              att->getArguments()[0])
                              ->getConstructor();
      overloads[innerSymbol].insert(outerSymbol);
    }
  }

  return transitiveClosure(overloads);
}

// NOLINTNEXTLINE(*-function-cognitive-complexity)
void kore_definition::preprocess() {
  insertReservedSymbols();

  for (auto *axiom : axioms_) {
    axiom->pattern_ = axiom->pattern_->expandAliases(this);
  }
  auto symbols = std::map<std::string, std::vector<kore_symbol *>>{};
  unsigned nextOrdinal = 0;
  for (auto const &decl : symbol_declarations_) {
    if (decl.second->attributes().contains(
            attribute_set::key::fresh_generator)) {
      auto sort = decl.second->getSymbol()->getSort();
      if (sort->isConcrete()) {
        fresh_functions_[dynamic_cast<kore_composite_sort *>(sort.get())->getName()]
            = decl.second->getSymbol();
      }
    }
  }
  for (auto iter = axioms_.begin(); iter != axioms_.end();) {
    auto *axiom = *iter;
    axiom->ordinal_ = nextOrdinal;
    ordinals_[nextOrdinal++] = axiom;
    axiom->pattern_->markSymbols(symbols);
    if (!axiom->isRequired()) {
      iter = axioms_.erase(iter);
    } else {
      ++iter;
    }
  }
  for (auto &module : modules_) {
    auto const &declarations = module->getDeclarations();
    for (auto const &declaration : declarations) {
      auto *decl = dynamic_cast<kore_symbol_declaration *>(declaration.get());
      if (decl == nullptr) {
        continue;
      }
      if (decl->isHooked() && decl->getObjectSortVariables().empty()) {
        kore_symbol *symbol = decl->getSymbol();
        symbols.emplace(symbol->getName(), std::vector<kore_symbol *>{symbol});
      }
    }
  }
  for (auto const &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = symbol_declarations_.at(symbol->getName());
      symbol->instantiateSymbol(decl);
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
      if (symbol->isConcrete()) {
        if (!instantiations.contains(*symbol)) {
          instantiations.emplace(*symbol, nextSymbol++);
        }
        std::string layoutStr = symbol->layoutString(this);
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
      for (auto const &sort : symbol->getArguments()) {
        if (sort->isConcrete()) {
          hooked_sorts_[dynamic_cast<kore_composite_sort *>(sort.get())
                          ->getCategory(this)]
              = std::dynamic_pointer_cast<kore_composite_sort>(sort);
        }
      }
      if (symbol->getSort()->isConcrete()) {
        hooked_sorts_[dynamic_cast<kore_composite_sort *>(symbol->getSort().get())
                        ->getCategory(this)]
            = std::dynamic_pointer_cast<kore_composite_sort>(symbol->getSort());
      }
      if (!symbol->isConcrete()) {
        if (symbol->isPolymorphic()) {
          symbol->first_tag_ = range.first;
          symbol->last_tag_ = range.second;
          auto *decl = symbol_declarations_.at(symbol->getName());
          if (decl->attributes().contains(attribute_set::key::sort_injection)) {
            inj_symbol_ = symbol;
          }
        }
      }
    }
  }
}

} // namespace kllvm
