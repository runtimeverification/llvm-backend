#include <iostream>
#include <memory>

#include <kllvm/ast/AST.h>

#include <kllvm-c/kllvm-c.h>

extern "C" {

struct kore_pattern {
  std::unique_ptr<kllvm::KOREPattern> ptr_;
};

/* KOREPattern */

char *kore_pattern_dump(kore_pattern const *pat) {
  std::ostringstream out;
  pat->ptr_->print(out);
  auto str = out.str();
  char * cstr = (char*) malloc((str.length()+1)*sizeof(char));
  std::strcpy (cstr, str.c_str());
  return cstr;
}

void kore_pattern_free(kore_pattern const *pat) {
  delete pat;
}

/* KORECompositePattern */

kore_pattern *kore_composite_pattern_new(char const *name) {
  auto pat = new kore_pattern;
  pat->ptr_ = kllvm::KORECompositePattern::Create(std::string(name));
  return pat;
}

void kore_composite_pattern_add_argument(kore_pattern *pat, kore_pattern *arg) {
  if (auto cast_ptr
      = dynamic_cast<kllvm::KORECompositePattern *>(pat->ptr_.get())) {
    cast_ptr->addArgument(
        std::shared_ptr<kllvm::KOREPattern>(arg->ptr_.release()));
  } else {
    abort();
  }
}

/* KOREStringPattern */

kore_pattern *kore_string_pattern_new(char const *contents) {
  auto pat = new kore_pattern;
  pat->ptr_ = kllvm::KOREStringPattern::Create(std::string(contents));
  return pat;
}
}
