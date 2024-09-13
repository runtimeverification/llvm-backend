#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gmp.h>
#include <mpfr.h>
#include <vector>

#include "runtime/alloc.h"
#include "runtime/header.h"

#define KCHAR char
#define TYPETAG(type) "Lbl'Hash'ffi'Unds'" #type "{}"

void *
construct_composite_pattern(uint32_t tag, std::vector<void *> &arguments) {
  return nullptr;
}

extern "C" {

bool enable_mutable_bytes = false;

struct point {
  int x;
  int y;
};

struct point2 {
  struct point p;
};

#define NUM_SYMBOLS 6
char const *symbols[NUM_SYMBOLS]
    = {TYPETAG(struct),
       TYPETAG(uint),
       TYPETAG(sint),
       TYPETAG(pointer),
       "inj{SortBytes{}, SortKItem{}}",
       "inj{SortFFIType{}, SortKItem{}}"};

char const **get_argument_sorts_for_tag(uint32_t tag) {
  return nullptr;
}

uint32_t const FIRST_INJ_TAG = 4;
uint32_t const LAST_INJ_TAG = 5;

char *get_terminated_string(string *str);

uint32_t get_tag_for_symbol_name(char const *s) {
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (0 == strcmp(symbols[i], s)) {
      return i + 1;
    }
  }

  return 0;
}

struct blockheader get_block_header_for_symbol(uint32_t tag) {
  return blockheader{tag};
}

void add_hash64(void *, uint64_t) { }

size_t hash_k(block *kitem) {
  return (size_t)kitem;
}

bool hash_enter(void) {
  return true;
}

void hash_exit(void) { }

void k_hash(block *, void *) { }

bool during_gc() {
  return false;
}

size_t get_gc_threshold() {
  return SIZE_MAX;
}

void print_configuration_internal(
    writer *file, block *subject, char const *sort, bool, void *) { }
void sfprintf(writer *, char const *, ...) { }

bool hook_KEQUAL_eq(block *lhs, block *rhs) {
  return lhs->h.hdr == rhs->h.hdr;
}

mpz_ptr hook_FFI_address(string *fn);
string *hook_FFI_call(mpz_t addr, list *args, list *types, block *ret);
string *hook_FFI_call_variadic(
    mpz_t addr, list *args, list *fixtypes, list *vartypes, block *ret);

mpz_ptr hook_FFI_bytes_address(string *bytes);
block *hook_FFI_free(block *kitem);
block *hook_FFI_freeAll(void);
block *hook_FFI_bytes_ref(string *bytes);
string *hook_FFI_alloc(block *kitem, mpz_t size, mpz_t align);
bool hook_FFI_allocated(block *kitem);

string *make_string(const KCHAR *, int64_t len = -1);

list hook_LIST_element(block *value);
list hook_LIST_concat(list *l1, list *l2);
list hook_LIST_unit();
size_t hook_LIST_size_long(list *l);
block *hook_LIST_get_long(list *l, ssize_t idx);

mpz_ptr move_int(mpz_t i) {
  mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
  *result = *i;
  return result;
}

floating *move_float(floating *i) {
  floating *result = (floating *)malloc(sizeof(floating));
  *result = *i;
  return result;
}

block D1 = {{1}};
block *DUMMY1 = &D1;
}

struct ffi_test_fixture {

  ~ffi_test_fixture() { hook_FFI_freeAll(); }
};

BOOST_FIXTURE_TEST_SUITE(FfiTest, ffi_test_fixture)

BOOST_AUTO_TEST_CASE(address) {
  string *fn = make_string("times_two");
  mpz_ptr addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("utimes_two");
  addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("times");
  addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("get_x");
  addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("increase_x");
  addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("times_point");
  addr = hook_FFI_address(fn);
  BOOST_CHECK(0 < mpz_cmp_ui(addr, 0));

  fn = make_string("fake_function");
  addr = hook_FFI_address(fn);
  BOOST_CHECK_EQUAL(0, mpz_cmp_ui(addr, 0));
}

BOOST_AUTO_TEST_CASE(call) {
  /* int times_two(int x) */
  int x = -3;
  string *xargstr = make_string((char *)&x, sizeof(int));

  block *xarg
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  xarg->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(xarg->children, &xargstr, sizeof(string *));

  list args = hook_LIST_element(xarg);
  block *type_sint = leaf_block(get_tag_for_symbol_name(TYPETAG(sint)));

  block *argtype
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  argtype->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortFFIType{}, SortKItem{}}"));
  memcpy(argtype->children, &type_sint, sizeof(block *));

  list types = hook_LIST_element(argtype);

  string *fn = make_string("times_two");
  mpz_ptr addr = hook_FFI_address(fn);

  string *bytes = hook_FFI_call(addr, &args, &types, type_sint);

  BOOST_CHECK(bytes != NULL);

  int ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, x * 2);

  /* unsigned int utimes_two(unsigned int x) */
  x = 4;
  xargstr = make_string((char *)&x, sizeof(int));

  memcpy(xarg->children, &xargstr, sizeof(string *));

  args = hook_LIST_element(xarg);
  block *type_uint = leaf_block(get_tag_for_symbol_name(TYPETAG(uint)));
  memcpy(argtype->children, &type_uint, sizeof(block *));

  types = hook_LIST_element(argtype);

  fn = make_string("utimes_two");
  addr = hook_FFI_address(fn);

  bytes = hook_FFI_call(addr, &args, &types, type_uint);

  BOOST_CHECK(bytes != NULL);

  ret = *(unsigned int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, x * 2);

  /* int times(int x, int y) */
  int y = 4;
  string *yargstr = make_string((char *)&y, sizeof(int));

  memcpy(argtype->children, &type_sint, sizeof(block *));

  block *yarg
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  yarg->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(yarg->children, &yargstr, sizeof(string *));

  list yargs = hook_LIST_element(yarg);

  args = hook_LIST_concat(&args, &yargs);
  types = hook_LIST_concat(&types, &types);

  fn = make_string("times");
  addr = hook_FFI_address(fn);
  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, x * y);

  /* struct point construct_point(int x, int y) */
  block *structType
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  structType->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name(TYPETAG(struct)));

  list *structFields = static_cast<list *>(kore_alloc(sizeof(list)));
  list tmp = hook_LIST_element(argtype);
  tmp = hook_LIST_concat(&tmp, &tmp);
  memcpy(structFields, &tmp, sizeof(list));

  memcpy(structType->children, &structFields, sizeof(list *));

  fn = make_string("construct_point");
  addr = hook_FFI_address(fn);
  bytes = hook_FFI_call(addr, &args, &types, structType);

  struct point p = *(struct point *)bytes->data;
  BOOST_CHECK_EQUAL(p.x, x);
  BOOST_CHECK_EQUAL(p.y, y);

  /* int get_x(void) */
  fn = make_string("get_x");
  addr = hook_FFI_address(fn);
  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, 1);

  /* void increase_x(void) */
  fn = make_string("increase_x");
  addr = hook_FFI_address(fn);
  bytes = hook_FFI_call(addr, &args, &types, type_sint);

  /* int get_x(void) */
  fn = make_string("get_x");
  addr = hook_FFI_address(fn);
  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, 2);

  /* struct point {
   *  int x;
   *  int y;
   * }
   *
   * int timesPoint(struct point p) */
  p = {.x = 2, .y = 5};
  string *pargstr = make_string((char *)&p, sizeof(struct point));

  block *parg
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  parg->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(parg->children, &pargstr, sizeof(string *));

  args = hook_LIST_element(parg);

  block *new_argtype
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  new_argtype->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortFFIType{}, SortKItem{}}"));
  memcpy(new_argtype->children, &structType, sizeof(block *));
  types = hook_LIST_element(new_argtype);

  fn = make_string("times_point");
  addr = hook_FFI_address(fn);

  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, p.x * p.y);

  /* struct point2 {
   *  struct point p;
   * }
   *
   * int timesPoint2(struct point2 p) */
  struct point2 p2 = {.p = p};
  string *pargstr2 = make_string((char *)&p2, sizeof(struct point2));

  memcpy(parg->children, &pargstr2, sizeof(string *));

  args = hook_LIST_element(parg);

  block *structType2
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  structType2->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name(TYPETAG(struct)));

  block *structArgType
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  structArgType->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortFFIType{}, SortKItem{}}"));
  memcpy(structArgType->children, &structType, sizeof(block *));

  list *structFields2 = static_cast<list *>(kore_alloc(sizeof(list)));
  list tmp2 = hook_LIST_element(structArgType);
  memcpy(structFields2, &tmp2, sizeof(list));

  memcpy(structType2->children, &structFields2, sizeof(list *));

  memcpy(new_argtype->children, &structType2, sizeof(block *));
  types = hook_LIST_element(new_argtype);

  fn = make_string("times_point2");
  addr = hook_FFI_address(fn);

  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, p2.p.x * p2.p.y);

  /* Make sure there is no double free */
  bytes = hook_FFI_call(addr, &args, &types, type_sint);
  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, p2.p.x * p2.p.y);

  /* int pointerTest(int * x) */
  x = 2;
  mpz_t s1, align;
  mpz_init_set_ui(s1, sizeof(int));
  mpz_init_set_ui(align, 16);
  string *b1 = hook_FFI_alloc(DUMMY1, s1, align);
  memcpy(b1->data, &x, sizeof(int));

  mpz_ptr addr1 = hook_FFI_bytes_address(b1);
  uintptr_t address1 = mpz_get_ui(addr1);

  string *ptrargstr = make_string((char *)&address1, sizeof(uintptr_t *));

  block *ptrarg
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  ptrarg->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(ptrarg->children, &ptrargstr, sizeof(string *));

  args = hook_LIST_element(ptrarg);
  block *type_pointer = leaf_block(get_tag_for_symbol_name(TYPETAG(pointer)));

  memcpy(argtype->children, &type_pointer, sizeof(block *));

  types = hook_LIST_element(argtype);

  fn = make_string("pointer_test");
  addr = hook_FFI_address(fn);

  bytes = hook_FFI_call(addr, &args, &types, type_sint);

  hook_FFI_free(DUMMY1);

  BOOST_CHECK(bytes != NULL);

  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, x);
}

BOOST_AUTO_TEST_CASE(call_variadic) {
  /* int addInts(int x, ...) */
  int n = 1;
  string *nargstr = make_string((char *)&n, sizeof(int));

  block *narg
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  narg->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(narg->children, &nargstr, sizeof(string *));

  list args = hook_LIST_element(narg);

  int arg1 = 1;
  string *arg1str = make_string((char *)&arg1, sizeof(int));

  block *arg1block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  arg1block->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(arg1block->children, &arg1str, sizeof(string *));

  list arg1list = hook_LIST_element(arg1block);

  args = hook_LIST_concat(&args, &arg1list);

  block *type_sint = leaf_block(get_tag_for_symbol_name(TYPETAG(sint)));

  block *fixargtype
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  fixargtype->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortFFIType{}, SortKItem{}}"));
  memcpy(fixargtype->children, &type_sint, sizeof(block *));

  block *varargtype
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  varargtype->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortFFIType{}, SortKItem{}}"));
  memcpy(varargtype->children, &type_sint, sizeof(block *));

  list fixtypes = hook_LIST_element(fixargtype);
  list vartypes = hook_LIST_element(varargtype);

  string *fn = make_string("add_ints");
  mpz_ptr addr = hook_FFI_address(fn);

  string *bytes
      = hook_FFI_call_variadic(addr, &args, &fixtypes, &vartypes, type_sint);

  BOOST_CHECK(bytes != NULL);

  int ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, arg1);

  /* addInts with 2 var args */
  n = 2;
  nargstr = make_string((char *)&n, sizeof(int));
  memcpy(narg->children, &nargstr, sizeof(string *));
  args = hook_LIST_element(narg);

  arg1 = 20;
  arg1str = make_string((char *)&arg1, sizeof(int));
  memcpy(arg1block->children, &arg1str, sizeof(string *));
  arg1list = hook_LIST_element(arg1block);
  args = hook_LIST_concat(&args, &arg1list);

  int arg2 = 15;
  string *arg2str = make_string((char *)&arg2, sizeof(int));

  block *arg2block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  arg2block->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name("inj{SortBytes{}, SortKItem{}}"));
  memcpy(arg2block->children, &arg2str, sizeof(string *));

  list arg2list = hook_LIST_element(arg2block);

  args = hook_LIST_concat(&args, &arg2list);

  vartypes = hook_LIST_element(varargtype);
  vartypes = hook_LIST_concat(&vartypes, &vartypes);

  bytes = hook_FFI_call_variadic(addr, &args, &fixtypes, &vartypes, type_sint);

  BOOST_CHECK(bytes != NULL);

  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, arg1 + arg2);

  /* addInts with 0 var args */
  n = 0;
  nargstr = make_string((char *)&n, sizeof(int));
  memcpy(narg->children, &nargstr, sizeof(string *));
  args = hook_LIST_element(narg);

  vartypes = hook_LIST_unit();

  bytes = hook_FFI_call_variadic(addr, &args, &fixtypes, &vartypes, type_sint);

  BOOST_CHECK(bytes != NULL);

  ret = *(int *)bytes->data;

  BOOST_CHECK_EQUAL(ret, 0);
}

BOOST_AUTO_TEST_CASE(alloc) {
  mpz_t s1, s2;
  mpz_init_set_ui(s1, 1);
  mpz_init_set_ui(s2, 16);

  string *b1 = hook_FFI_alloc(DUMMY1, s1, s2);
  BOOST_CHECK(0 != b1);

  string *b2 = hook_FFI_alloc(DUMMY1, s1, s2);
  BOOST_CHECK_EQUAL(b1, b2);
}

BOOST_AUTO_TEST_CASE(free) {
  mpz_t s1, s2;
  mpz_init_set_ui(s1, 1);
  mpz_init_set_ui(s2, 16);

  hook_FFI_alloc(DUMMY1, s1, s2);
  hook_FFI_free(DUMMY1);
}

BOOST_AUTO_TEST_CASE(bytes_ref) {
  mpz_t s1, s2;
  mpz_init_set_ui(s1, 1);
  mpz_init_set_ui(s2, 16);

  string *b1 = hook_FFI_alloc(DUMMY1, s1, s2);

  block *i2 = hook_FFI_bytes_ref(b1);

  BOOST_CHECK_EQUAL(DUMMY1, i2);
}

BOOST_AUTO_TEST_CASE(allocated) {
  mpz_t s1, s2;
  mpz_init_set_ui(s1, 1);
  mpz_init_set_ui(s2, 16);

  hook_FFI_alloc(DUMMY1, s1, s2);
  BOOST_CHECK_EQUAL(true, hook_FFI_allocated(DUMMY1));

  block *i2 = (block *)2;
  BOOST_CHECK_EQUAL(false, hook_FFI_allocated(i2));
}

BOOST_AUTO_TEST_CASE(alignment) {
  mpz_t s1, s2;
  mpz_init_set_ui(s1, 1);
  mpz_init_set_ui(s2, 32768);
  string *b1 = hook_FFI_alloc(DUMMY1, s1, s2);
  BOOST_CHECK_EQUAL(((uintptr_t)b1) % 32768, 0);
}

BOOST_AUTO_TEST_SUITE_END()
