#include "kllvm/binary/deserializer.h"
#include "kllvm/binary/version.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/KOREScanner.h"
#include "runtime/alloc.h"

#include <fmt/format.h>

#include <gmp.h>
#include <map>
#include <variant>

#include "runtime/header.h"

using namespace kllvm;
using namespace kllvm::parser;

using Cache = std::map<std::string, uint32_t>;
static thread_local Cache cache;

extern "C" {

uint32_t get_tag_for_symbol_name_internal(char const *);

void init_float(floating *result, char const *c_str) {
  std::string contents = std::string(c_str);
  init_float2(result, contents);
}

uint32_t get_tag_for_symbol_name(char const *name) {
  std::string s = name;
  // https://stackoverflow.com/a/101980/6209703
  auto lb = cache.lower_bound(s);
  // key exists
  if (lb != cache.end() && !(cache.key_comp()(s, lb->first))) {
    return lb->second;
  }

  uint32_t const tag = get_tag_for_symbol_name_internal(s.c_str());

  if (tag == ERROR_TAG) {
    auto error_message = fmt::format(
        "No tag found for symbol {}. Maybe attempted to evaluate a symbol with "
        "no rules?\n",
        name);

    throw std::runtime_error(error_message);
  }

  cache.insert(lb, Cache::value_type{s, tag});
  return tag;
}
}

static uint32_t get_tag_for_symbol(kore_symbol const &symbol) {
  auto name = ast_to_string(symbol);
  return get_tag_for_symbol_name(name.c_str());
}

void *
construct_composite_pattern(uint32_t tag, std::vector<void *> &arguments) {
  if (is_symbol_a_function(tag)) {
    return evaluate_function_symbol(tag, arguments.data());
  }

  struct blockheader header_val = get_block_header_for_symbol(tag);
  size_t size = size_hdr(header_val.hdr);

  if (tag >= FIRST_INJ_TAG && tag <= LAST_INJ_TAG) {
    uint16_t layout_code = layout_hdr(header_val.hdr);
    layout *data = get_layout_data(layout_code);
    if (data->args[0].cat == SYMBOL_LAYOUT) {
      auto *child = (block *)arguments[0];
      if (!is_leaf_block(child) && get_layout(child) != 0) {
        uint32_t tag = tag_hdr(child->h.hdr);
        if (tag >= FIRST_INJ_TAG && tag <= LAST_INJ_TAG) {
          return child;
        }
      }
    }
  }

  auto *new_block = (block *)kore_alloc(size);
  new_block->h = header_val;

  store_symbol_children(new_block, arguments.data());
  if (is_symbol_a_binder(tag)) {
    new_block = debruijnize(new_block);
  }

  return new_block;
}

struct construction {
  uint32_t tag;
  size_t nchildren;
};

// NOLINTNEXTLINE(*-cognitive-complexity)
extern "C" void *construct_initial_configuration(kore_pattern const *initial) {
  bool enabled = gc_enabled;
  gc_enabled = false;
  std::vector<std::variant<kore_pattern const *, construction>> work_list{
      initial};
  std::vector<void *> output;

  while (!work_list.empty()) {
    std::variant<kore_pattern const *, construction> current = work_list.back();
    work_list.pop_back();

    if (std::holds_alternative<kore_pattern const *>(current)) {
      auto const *constructor = dynamic_cast<kore_composite_pattern const *>(
          *std::get_if<kore_pattern const *>(&current));
      assert(constructor && "Pattern in worklist is not composite");

      kore_symbol const *symbol = constructor->get_constructor();
      assert(
          symbol->is_concrete()
          && "found sort variable in initial configuration");
      if (symbol->get_name() == "\\dv") {
        auto *const sort = dynamic_cast<kore_composite_sort *>(
            symbol->get_formal_arguments()[0].get());
        auto *const str_pattern = dynamic_cast<kore_string_pattern *>(
            constructor->get_arguments()[0].get());
        std::string contents = str_pattern->get_contents();
        output.push_back(get_token(
            sort->get_name().c_str(), contents.size(), contents.c_str()));
        continue;
      }

      uint32_t tag = get_tag_for_symbol(*symbol);

      if (is_symbol_a_function(tag) && constructor->get_arguments().empty()) {
        output.push_back(evaluate_function_symbol(tag, nullptr));
        continue;
      }
      if (constructor->get_arguments().empty()) {
        output.push_back(leaf_block(tag));
        continue;
      }

      construction term{tag, constructor->get_arguments().size()};
      work_list.emplace_back(term);
      for (auto const &child : constructor->get_arguments()) {
        work_list.emplace_back(child.get());
      }
    } else {
      uint32_t tag = std::get_if<construction>(&current)->tag;
      size_t nchildren = std::get_if<construction>(&current)->nchildren;

      std::vector<void *> arguments;
      for (size_t i = 0; i < nchildren; i++) {
        arguments.push_back(output.back());
        output.pop_back();
      }

      output.push_back(construct_composite_pattern(tag, arguments));
    }
  }

  gc_enabled = enabled;
  return output[0];
}

// NOLINTBEGIN(*-cognitive-complexity)
template <typename It>
static void *
deserialize_initial_configuration(It ptr, It end, binary_version version) {
  bool enabled = gc_enabled;
  gc_enabled = false;
  using namespace kllvm::detail;
  auto begin = ptr;

  auto output = std::vector<void *>{};

  auto token_stack = std::vector<std::string>{};
  auto sort_stack = std::vector<sptr<kore_sort>>{};
  auto symbol = kllvm::ptr<kore_symbol>{};

  while (ptr < end) {
    switch (peek(ptr)) {

    case header_byte<kore_composite_pattern>: {
      ++ptr;
      auto arity = read_length(ptr, end, version, 2);

      assert(symbol && "No symbol set when reaching composite pattern");
      assert(
          symbol->is_concrete()
          && "found sort variable in initial configuration");

      if (symbol->get_name() == "\\dv") {
        auto *sort = dynamic_cast<kore_composite_sort *>(
            symbol->get_formal_arguments()[0].get());
        assert(sort && "Not a composite sort");
        auto const &token = token_stack.back();

        output.push_back(
            get_token(sort->get_name().c_str(), token.size(), token.c_str()));

        token_stack.pop_back();
        break;
      }

      uint32_t tag = get_tag_for_symbol(*symbol);
      symbol = nullptr;

      if (is_symbol_a_function(tag) && arity == 0) {
        output.push_back(evaluate_function_symbol(tag, nullptr));
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

      output.push_back(construct_composite_pattern(tag, arguments));
      break;
    }

    case header_byte<kore_string_pattern>:
      ++ptr;
      token_stack.push_back(read_string(ptr, end, version));
      break;

    case header_byte<kore_symbol>: {
      ++ptr;
      symbol = read_symbol(ptr, end, sort_stack, version);
      break;
    }

    case header_byte<kore_sort_variable>: {
      ++ptr;
      sort_stack.push_back(
          kore_sort_variable::create(read_string(ptr, end, version)));
      break;
    }

    case header_byte<kore_composite_sort>: {
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

  gc_enabled = enabled;
  assert(output.size() == 1 && "Output stack left in invalid state");
  return output.front();
}
// NOLINTEND(*-cognitive-complexity)

block *parse_configuration(char const *filename) {
  if (has_binary_kore_header(filename)) {
    auto data = file_contents(filename);
    return deserialize_configuration(data.data(), data.size());
  }
  auto initial_configuration = parser::kore_parser(filename).pattern();
  // InitialConfiguration->print(std::cout);

  // Allocate the llvm KORE datastructures for the configuration
  auto *b
      = (block *)construct_initial_configuration(initial_configuration.get());
  deallocate_s_ptr_kore_pattern(std::move(initial_configuration));
  return b;
}

block *deserialize_configuration(char *data, size_t size) {
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
      deserialize_initial_configuration(ptr, end, version));
}
