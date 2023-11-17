#include "runtime/header.h"

extern "C" {

block *dot_k() {
  return leaf_block(getTagForSymbolName("dotk{}"));
}

bool is_injection(block *term) {
  auto tag = tag_hdr(term->h.hdr);
  return tag >= first_inj_tag && tag <= last_inj_tag;
}

block *strip_injection(block *term) {
  if (is_injection(term)) {
    return (block *)(term->children[0]);
  }

  return term;
}

block *constructKItemInj(void *subject, const char *sort, bool raw_value) {
  auto inj_sym = "inj{" + std::string(sort) + ", SortKItem{}}";
  auto tag = getTagForSymbolName(inj_sym.c_str());
  auto sort_prefix = std::string(sort).substr(0, 8);
  auto integral_sort = sort_prefix == "SortBool" || sort_prefix == "SortMInt";
  auto add_indirection = raw_value && integral_sort;
  auto args = std::vector<void *>{add_indirection ? (void *)&subject : subject};
  return static_cast<block *>(constructCompositePattern(tag, args));
}

void printProofHintHeader(char *output_file) {
  unsigned int version = 2;
  FILE *file = fopen(output_file, "a");
  fprintf(file, "HINT");
  fwrite(&version, sizeof(version), 1, file);
  fflush(file);
  fclose(file);
}
}
