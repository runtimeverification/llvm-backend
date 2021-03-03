#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"
#include "runtime/alloc.h"

#include <gmp.h>
#include <variant>
#include <cstdlib>
#include <map>

#include "runtime/header.h"

using namespace kllvm;
using namespace kllvm::parser;

extern "C" {
  void init_float(floating *result, const char *c_str) {
    std::string contents = std::string(c_str);
    init_float2(result, contents);
  }
}

struct construction {
  uint32_t tag;
  size_t nchildren;
};

class CachedGTFSN{
public:
    explicit CachedGTFSN()
    : use_cache{std::getenv("USE_SYMBOL_CACHE") != nullptr}
    {}

    // TODO test with string_view - it might be faster
    uint32_t getTagForSymbolName(std::string const &s) {
        if (!use_cache)
            return ::getTagForSymbolName(s.c_str());

        // https://stackoverflow.com/a/101980/6209703
        Cache::iterator lb = cache.lower_bound(s);
        // key exists
        if (lb != cache.end() && !(cache.key_comp()(s, lb->first))) {
            return lb->second;
        }
        uint32_t const tag = ::getTagForSymbolName(s.c_str());
        cache.insert(lb, Cache::value_type{s, tag});
        return tag;
    }
private:
    bool const use_cache;
    using Cache = std::map<std::string, uint32_t>;
    Cache cache;
};

static void *constructInitialConfiguration(const KOREPattern *initial) {
    CachedGTFSN cachedGtfsn;
  std::vector<std::variant<const KOREPattern *, construction>> workList{initial};
  std::vector<void *> output;
  while (!workList.empty()) {
    std::variant<const KOREPattern *, construction> current = workList.back();
    workList.pop_back();
    if (current.index() == 0) {
      const auto constructor = dynamic_cast<const KORECompositePattern *>(*std::get_if<const KOREPattern *>(&current));
      assert(constructor);

      const KORESymbol *symbol = constructor->getConstructor();
      assert(symbol->isConcrete() && "found sort variable in initial configuration");
      if (symbol->getName() == "\\dv") {
        const auto sort = dynamic_cast<KORECompositeSort *>(symbol->getFormalArguments()[0].get());
        const auto strPattern =
          dynamic_cast<KOREStringPattern *>(constructor->getArguments()[0].get());
        std::string contents = strPattern->getContents();
        output.push_back(getToken(sort->getName().c_str(), contents.size(), contents.c_str()));
        continue;
      }

      std::ostringstream Out;
      symbol->print(Out);
      // TODO: test whether with string view would it be faster
      uint32_t tag = cachedGtfsn.getTagForSymbolName(Out.str());
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
      if (isSymbolAFunction(tag)) {
        output.push_back(evaluateFunctionSymbol(tag, &arguments[0]));
        continue;
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
              output.push_back(child);
              continue;
            }
          }
        }
      }

      block *Block = (block *) koreAlloc(size);
      Block->h = headerVal;

      storeSymbolChildren(Block, &arguments[0]);
      if (isSymbolABinder(tag)) {
        Block = debruijnize(Block);
      }
      output.push_back(Block);
    }
  }
  return output[0];
}

block *parseConfiguration(const char *filename) {
  // Parse initial configuration as a KOREPattern
  KOREParser parser(filename);
  ptr<KOREPattern> InitialConfiguration = parser.pattern();

  //InitialConfiguration->print(std::cout);

  // Allocate the llvm KORE datastructures for the configuration
  auto b = (block *) constructInitialConfiguration(InitialConfiguration.get());
  deallocateSPtrKorePattern(std::move(InitialConfiguration));
  return b;
}
