#include <iostream>
#include <memory>

#include <kllvm/ast/AST.h>

#include <kllvm-c/kllvm-c.h>

extern "C" {

struct kore_composite_pattern {
  std::unique_ptr<kllvm::KORECompositePattern> ptr_;
};

kore_composite_pattern *kore_composite_pattern_new(char const *name) {
  auto pat = new kore_composite_pattern;
  pat->ptr_ = kllvm::KORECompositePattern::Create(std::string(name));
  return pat;
}

void kore_composite_pattern_dump(kore_composite_pattern const *pat) {
  pat->ptr_->print(std::cout);
}

void kore_composite_pattern_add_argument(
    kore_composite_pattern *pat, kore_composite_pattern *arg) {
  pat->ptr_->addArgument(
      std::shared_ptr<kllvm::KOREPattern>(arg->ptr_.release()));
}

void kore_composite_pattern_free(kore_composite_pattern const *pat) {
  delete pat;
}
}
