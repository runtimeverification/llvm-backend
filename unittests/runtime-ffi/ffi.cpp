#include<boost/test/unit_test.hpp>
#include<gmp.h>
#include<mpfr.h>
#include<cstdint>
#include<cstdlib>
#include<cstring>
#include<dlfcn.h>

#include "runtime/header.h"
#include "runtime/alloc.h"

#define KCHAR char
extern "C" {

  uint32_t getTagForSymbolName(const char *s) {
    return 0;
  }

  struct blockheader getBlockHeaderForSymbol(uint32_t tag) {
    return blockheader {tag};
  }

  void add_hash64(void*, uint64_t) {}
  mpz_ptr hook_FFI_address(string * fn);
  string * hook_FFI_call(mpz_t addr, struct list * args, struct list * types, block * ret);

  string * makeString(const KCHAR *, int64_t len = -1);

  struct list hook_LIST_element(block * value);

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
}

BOOST_AUTO_TEST_SUITE(FfiTest)

BOOST_AUTO_TEST_CASE(address) {
}

BOOST_AUTO_TEST_CASE(call) {
  void * handle = dlopen("lib.so", RTLD_NOW);

  BOOST_CHECK(handle != NULL);

  string * argstr = makeString("*"); 

  block * arg = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(string *)));
  arg->h = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortBytes{}}"));
  memcpy(arg->children, &argstr, sizeof(string *));

  struct list args = hook_LIST_element(arg);
  block * type_sint = (block *)((((uint64_t)getTagForSymbolName("Lbl'hash'sint{}")) << 32) | 1);

  block * argtype = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(block *)));
  argtype->h = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortFFIType{}}"));
  memcpy(argtype->children, &type_sint, sizeof(block *));

  struct list types = hook_LIST_element(argtype);

  string * fn = makeString("timesTwo");
  mpz_ptr addr = hook_FFI_address(fn);

  string * bytes = hook_FFI_call(addr, &args, &types, type_sint);

  BOOST_CHECK(bytes != NULL);
}

BOOST_AUTO_TEST_SUITE_END()
