#include "kllvm/binary/deserializer.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/KOREScanner.h"
#include "runtime/alloc.h"

#include <gmp.h>
#include <map>
#include <variant>

#include "runtime/header.h"

using namespace kllvm;
using namespace kllvm::parser;

using Cache = std::map<std::string, uint32_t>;
static thread_local Cache cache;

extern "C" {
uint32_t getTagForSymbolNameInternal(const char *);

void init_float(floating *result, const char *c_str) {
  std::string contents = std::string(c_str);
  init_float2(result, contents);
}

uint32_t getTagForSymbolName(const char *name) {
  std::string s = name;
  // https://stackoverflow.com/a/101980/6209703
  Cache::iterator lb = cache.lower_bound(s);
  // key exists
  if (lb != cache.end() && !(cache.key_comp()(s, lb->first))) {
    return lb->second;
  }
  uint32_t const tag = getTagForSymbolNameInternal(s.c_str());
  cache.insert(lb, Cache::value_type{s, tag});
  return tag;
}
}

static uint32_t getTagForSymbol(KORESymbol const &symbol) {
  std::ostringstream out;
  symbol.print(out);
  return getTagForSymbolName(out.str().c_str());
}

static void *
constructCompositePattern(uint32_t tag, std::vector<void *> &arguments) {
  if (isSymbolAFunction(tag)) {
    return evaluateFunctionSymbol(tag, &arguments[0]);
  }

  struct blockheader headerVal = getBlockHeaderForSymbol(tag);
  size_t size = size_hdr(headerVal.hdr);

  if (tag >= first_inj_tag && tag <= last_inj_tag) {
    uint16_t layout_code = layout_hdr(headerVal.hdr);
    layout *data = getLayoutData(layout_code);
    if (data->args[0].cat == SYMBOL_LAYOUT) {
      block *child = (block *)arguments[0];
      if (!is_leaf_block(child) && layout(child) != 0) {
        uint32_t tag = tag_hdr(child->h.hdr);
        if (tag >= first_inj_tag && tag <= last_inj_tag) {
          return child;
        }
      }
    }
  }

  block *Block = (block *)koreAlloc(size);
  Block->h = headerVal;

  storeSymbolChildren(Block, &arguments[0]);
  if (isSymbolABinder(tag)) {
    Block = debruijnize(Block);
  }

  return Block;
}

struct construction {
  uint32_t tag;
  size_t nchildren;
};

static void *constructInitialConfiguration(const KOREPattern *initial) {
  std::vector<std::variant<const KOREPattern *, construction>> workList{
      initial};
  std::vector<void *> output;

  while (!workList.empty()) {
    std::variant<const KOREPattern *, construction> current = workList.back();
    workList.pop_back();

    if (std::holds_alternative<const KOREPattern *>(current)) {
      auto constructor = dynamic_cast<const KORECompositePattern *>(
          *std::get_if<const KOREPattern *>(&current));
      assert(constructor && "Pattern in worklist is not composite");

      const KORESymbol *symbol = constructor->getConstructor();
      assert(
          symbol->isConcrete()
          && "found sort variable in initial configuration");
      if (symbol->getName() == "\\dv") {
        const auto sort = dynamic_cast<KORECompositeSort *>(
            symbol->getFormalArguments()[0].get());
        const auto strPattern = dynamic_cast<KOREStringPattern *>(
            constructor->getArguments()[0].get());
        std::string contents = strPattern->getContents();
        output.push_back(getToken(
            sort->getName().c_str(), contents.size(), contents.c_str()));
        continue;
      }

      uint32_t tag = getTagForSymbol(*symbol);

      if (isSymbolAFunction(tag) && constructor->getArguments().empty()) {
        output.push_back(evaluateFunctionSymbol(tag, nullptr));
        continue;
      } else if (constructor->getArguments().empty()) {
        output.push_back(leaf_block(tag));
        continue;
      }

      construction term{tag, constructor->getArguments().size()};
      workList.push_back(term);
      for (const auto &child : constructor->getArguments()) {
        workList.push_back(child.get());
      }
    } else {
      uint32_t tag = std::get_if<construction>(&current)->tag;
      size_t nchildren = std::get_if<construction>(&current)->nchildren;

      std::vector<void *> arguments;
      for (size_t i = 0; i < nchildren; i++) {
        arguments.push_back(output.back());
        output.pop_back();
      }

      output.push_back(constructCompositePattern(tag, arguments));
    }
  }

  return output[0];
}

template <typename It>
static void *deserializeInitialConfiguration(It ptr, It end) {
  using namespace kllvm::detail;
  auto begin = ptr;

  auto output = std::vector<void *>{};

  auto token_stack = std::vector<std::string>{};
  auto sort_stack = std::vector<sptr<KORESort>>{};
  auto symbol = kllvm::ptr<KORESymbol>{};

  while (ptr < end) {
    switch (peek(ptr)) {

    case header_byte<KORECompositePattern>: {
      ++ptr;
      auto arity = read<int16_t>(ptr);

      assert(symbol && "No symbol set when reaching composite pattern");
      assert(
          symbol->isConcrete()
          && "found sort variable in initial configuration");

      if (symbol->getName() == "\\dv") {
        auto sort = dynamic_cast<KORECompositeSort *>(
            symbol->getFormalArguments()[0].get());
        assert(sort && "Not a composite sort");
        auto const &token = token_stack.back();

        output.push_back(
            getToken(sort->getName().c_str(), token.size(), token.c_str()));

        token_stack.pop_back();
        break;
      }

      uint32_t tag = getTagForSymbol(*symbol);
      symbol = nullptr;

      if (isSymbolAFunction(tag) && arity == 0) {
        output.push_back(evaluateFunctionSymbol(tag, nullptr));
        break;
      } else if (arity == 0) {
        output.push_back(leaf_block(tag));
        break;
      }

      assert(arity > 0 && "Invalid arity when constructing composite block");

      auto arguments = std::vector<void *>{};
      for (size_t i = 0; i < arity; i++) {
        arguments.push_back(output.back());
        output.pop_back();
      }
      std::reverse(arguments.begin(), arguments.end());

      output.push_back(constructCompositePattern(tag, arguments));
      break;
    }

    case header_byte<KOREStringPattern>:
      ++ptr;
      token_stack.push_back(read_string(ptr));
      break;

    case header_byte<KORESymbol>: {
      ++ptr;
      symbol = read_symbol(ptr, sort_stack);
      break;
    }

    case header_byte<KORESortVariable>: {
      ++ptr;
      sort_stack.push_back(KORESortVariable::Create(read_string(ptr)));
      break;
    }

    case header_byte<KORECompositeSort>: {
      ++ptr;
      sort_stack.push_back(read_composite_sort(ptr, sort_stack));
      break;
    }

    default:
      std::cerr << "Bad term " << int(*ptr) << ' '
                << (std::distance(begin, ptr) + 11) << '\n';
      return nullptr;
    }
  }

  assert(output.size() == 1 && "Output stack left in invalid state");
  return output.front();
}

block *parseConfiguration(const char *filename) {
  if (has_binary_kore_header(filename)) {
    auto data = file_contents(filename);
    auto ptr = data.begin();

    for (auto i = 0; i < serializer::magic_header.size(); ++i) {
      detail::read<char>(ptr);
    }

    detail::read<int16_t>(ptr);
    detail::read<int16_t>(ptr);
    detail::read<int16_t>(ptr);

    auto ret = (block *)deserializeInitialConfiguration(ptr, data.end());
    return ret;
  } else {
    auto InitialConfiguration = parser::KOREParser(filename).pattern();
    // InitialConfiguration->print(std::cout);

    // Allocate the llvm KORE datastructures for the configuration
    auto b = (block *)constructInitialConfiguration(InitialConfiguration.get());
    deallocateSPtrKorePattern(std::move(InitialConfiguration));
    return b;
  }
}
