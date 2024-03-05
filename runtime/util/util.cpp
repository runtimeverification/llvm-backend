#include "runtime/header.h"

#include <fmt/format.h>

extern "C" {

bool is_injection(block *term) {
  auto tag = tag_hdr(term->h.hdr);
  return tag >= FIRST_INJ_TAG && tag <= LAST_INJ_TAG;
}

block *strip_injection(block *term) {
  if (is_injection(term)) {
    return (block *)(term->children[0]);
  }

  return term;
}

block *construct_k_item_inj(void *subject, char const *sort, bool raw_value) {
  auto inj_sym = "inj{" + std::string(sort) + ", SortKItem{}}";
  auto tag = get_tag_for_symbol_name(inj_sym.c_str());
  auto sort_prefix = std::string(sort).substr(0, 8);
  auto integral_sort = sort_prefix == "SortBool" || sort_prefix == "SortMInt";
  auto add_indirection = raw_value && integral_sort;
  auto args = std::vector<void *>{add_indirection ? (void *)&subject : subject};
  return static_cast<block *>(construct_composite_pattern(tag, args));
}

block *construct_raw_term(void *subject, char const *sort, bool raw_value) {
  auto tag = get_tag_for_symbol_name("rawTerm{}");
  auto args = std::vector{
      static_cast<void *>(construct_k_item_inj(subject, sort, raw_value))};
  return static_cast<block *>(construct_composite_pattern(tag, args));
}

void print_proof_hint_header(FILE *file) {
  uint32_t version = 5;
  fmt::print(file, "HINT");
  fwrite(&version, sizeof(version), 1, file);
}
}
