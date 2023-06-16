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
}
