#include "kllvm/binary/deserializer.h"
#include "kllvm/binary/version.h"
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
uint32_t getTagForSymbolNameInternal(char const *);

void init_float(floating *result, char const *c_str) {
  std::string contents = std::string(c_str);
  init_float2(result, contents);
}

uint32_t getTagForSymbolName(char const *name) {
  std::string s = name;
  // https://stackoverflow.com/a/101980/6209703
  auto lb = cache.lower_bound(s);
  // key exists
  if (lb != cache.end() && !(cache.key_comp()(s, lb->first))) {
    return lb->second;
  }
  uint32_t const tag = getTagForSymbolNameInternal(s.c_str());
  if (tag == ERROR_TAG) {
    std::cerr << "No tag found for symbol " << name << ". Maybe attempted to "
              << "evaluate a symbol with no rules?\n";
    abort();
  }
  cache.insert(lb, Cache::value_type{s, tag});
  return tag;
}
}

static uint32_t getTagForSymbol(KORESymbol const &symbol) {
  auto name = ast_to_string(symbol);
  return getTagForSymbolName(name.c_str());
}

void *constructCompositePattern(uint32_t tag, std::vector<void *> &arguments) {
  if (isSymbolAFunction(tag)) {
    return evaluateFunctionSymbol(tag, arguments.data());
  }

  struct blockheader headerVal = getBlockHeaderForSymbol(tag);
  size_t size = size_hdr(headerVal.hdr);

  if (tag >= first_inj_tag && tag <= last_inj_tag) {
    uint16_t layout_code = layout_hdr(headerVal.hdr);
    layout *data = getLayoutData(layout_code);
    if (data->args[0].cat == SYMBOL_LAYOUT) {
      auto *child = (block *)arguments[0];
      if (!is_leaf_block(child) && get_layout(child) != 0) {
        uint32_t tag = tag_hdr(child->h.hdr);
        if (tag >= first_inj_tag && tag <= last_inj_tag) {
          return child;
        }
      }
    }
  }

  auto *Block = (block *)koreAlloc(size);
  Block->h = headerVal;

  storeSymbolChildren(Block, arguments.data());
  if (isSymbolABinder(tag)) {
    Block = debruijnize(Block);
  }

  return Block;
}

struct construction {
  uint32_t tag;
  size_t nchildren;
};

// NOLINTNEXTLINE(*-cognitive-complexity)
extern "C" void *constructInitialConfiguration(KOREPattern const *initial) {
  std::vector<std::variant<KOREPattern const *, construction>> workList{
      initial};
  std::vector<void *> output;

  while (!workList.empty()) {
    std::variant<KOREPattern const *, construction> current = workList.back();
    workList.pop_back();

    if (std::holds_alternative<KOREPattern const *>(current)) {
      auto const *constructor = dynamic_cast<KORECompositePattern const *>(
          *std::get_if<KOREPattern const *>(&current));
      assert(constructor && "Pattern in worklist is not composite");

      KORESymbol const *symbol = constructor->getConstructor();
      assert(
          symbol->isConcrete()
          && "found sort variable in initial configuration");
      if (symbol->getName() == "\\dv") {
        auto *const sort = dynamic_cast<KORECompositeSort *>(
            symbol->getFormalArguments()[0].get());
        auto *const strPattern = dynamic_cast<KOREStringPattern *>(
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
      }
      if (constructor->getArguments().empty()) {
        output.push_back(leaf_block(tag));
        continue;
      }

      construction term{tag, constructor->getArguments().size()};
      workList.emplace_back(term);
      for (auto const &child : constructor->getArguments()) {
        workList.emplace_back(child.get());
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

// NOLINTBEGIN(*-cognitive-complexity)
template <typename It>
static void *
deserializeInitialConfiguration(It ptr, It end, binary_version version) {
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
      auto arity = read_length(ptr, end, version, 2);

      assert(symbol && "No symbol set when reaching composite pattern");
      assert(
          symbol->isConcrete()
          && "found sort variable in initial configuration");

      if (symbol->getName() == "\\dv") {
        auto *sort = dynamic_cast<KORECompositeSort *>(
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
      }
      if (arity == 0) {
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
      token_stack.push_back(read_string(ptr, end, version));
      break;

    case header_byte<KORESymbol>: {
      ++ptr;
      symbol = read_symbol(ptr, end, sort_stack, version);
      break;
    }

    case header_byte<KORESortVariable>: {
      ++ptr;
      sort_stack.push_back(
          KORESortVariable::Create(read_string(ptr, end, version)));
      break;
    }

    case header_byte<KORECompositeSort>: {
      ++ptr;
      sort_stack.push_back(read_composite_sort(ptr, end, sort_stack, version));
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
// NOLINTEND(*-cognitive-complexity)

block *parseConfiguration(char const *filename) {
  if (has_binary_kore_header(filename)) {
    auto data = file_contents(filename);
    return deserializeConfiguration(data.data(), data.size());
  }
  auto InitialConfiguration = parser::KOREParser(filename).pattern();
  // InitialConfiguration->print(std::cout);

  // Allocate the llvm KORE datastructures for the configuration
  auto *b = (block *)constructInitialConfiguration(InitialConfiguration.get());
  deallocateSPtrKorePattern(std::move(InitialConfiguration));
  return b;
}

block *deserializeConfiguration(char *data, size_t size) {
  auto *ptr = data;
  auto *end = data + size;

  for (auto i = 0; i < serializer::magic_header.size(); ++i) {
    detail::read<char>(ptr, end);
  }

  auto version = detail::read_version(ptr, end);
  auto total_size = detail::read_pattern_size(ptr, end, version);

  if (total_size > 0 && std::distance(ptr, end) > total_size) {
    end = std::next(ptr, total_size);
  }

  return static_cast<block *>(
      deserializeInitialConfiguration(ptr, end, version));
}
