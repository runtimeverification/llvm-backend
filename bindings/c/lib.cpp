#include <iostream>
#include <memory>

#include <kllvm/ast/AST.h>

#include <kllvm-c/kllvm-c.h>

namespace {
template <typename OS>
char *get_c_string(OS const &os) {
  auto str = os.str();

  // Include null terminator
  auto total_length = str.length() + 1;

  auto c_str = reinterpret_cast<char *>(malloc(total_length * sizeof(char)));
  std::strncpy(c_str, str.c_str(), total_length);

  return c_str;
}
} // namespace

extern "C" {

/* KOREPattern */

struct kore_pattern {
  std::unique_ptr<kllvm::KOREPattern> ptr_;
};

char *kore_pattern_dump(kore_pattern const *pat) {
  auto os = std::ostringstream{};
  pat->ptr_->print(os);
  return get_c_string(os);
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

/* KORESort */

struct kore_sort {
  std::shared_ptr<kllvm::KORESort> ptr_;
};

char *kore_sort_dump(kore_sort const *sort) {
  auto os = std::stringstream{};
  sort->ptr_->print(os);
  return get_c_string(os);
}

void kore_sort_free(kore_sort const *sort) {
  delete sort;
}

bool kore_sort_is_concrete(kore_sort const *sort) {
  return sort->ptr_->isConcrete();
}

/* KORECompositeSort */

kore_sort *kore_composite_sort_new(char const *name) {
  auto sort = new kore_sort;
  sort->ptr_ = kllvm::KORECompositeSort::Create(std::string(name));
  return sort;
}

void kore_composite_sort_add_argument(kore_sort *sort, kore_sort *arg) {
  if (auto cast_ptr
      = dynamic_cast<kllvm::KORECompositeSort *>(sort->ptr_.get())) {
    cast_ptr->addArgument(arg->ptr_);
  } else {
    abort();
  }
}
}
