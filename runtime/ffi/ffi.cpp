#include <cstring>
#include <dlfcn.h>
#include <ffi.h>
#include <gmp.h>
#include <stdexcept>

#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
#define TYPETAG(type) "Lbl'hash'" #type "{}"

#define TAG_TYPE(NAME) static uint64_t tag_type_##NAME() {\
  static uint64_t tag = -1; \
  if (tag == -1) { \
    tag = ((uint64_t)getTagForSymbolName(TYPETAG(NAME)) << 32) | 1; \
  } \
  return tag; \
}

  TAG_TYPE(void)
  TAG_TYPE(uint8)
  TAG_TYPE(sint8)
  TAG_TYPE(uint16)
  TAG_TYPE(sint16)
  TAG_TYPE(uint32)
  TAG_TYPE(sint32)
  TAG_TYPE(uint64)
  TAG_TYPE(sint64)
  TAG_TYPE(float)
  TAG_TYPE(double)
  TAG_TYPE(uchar)
  TAG_TYPE(schar)
  TAG_TYPE(ushort)
  TAG_TYPE(sshort)
  TAG_TYPE(uint)
  TAG_TYPE(sint)
  TAG_TYPE(ulong)
  TAG_TYPE(slong)
  TAG_TYPE(longdouble)

  mpz_ptr move_int(mpz_t);
  char * getTerminatedString(string * str);

  mpz_ptr hook_LIST_size(struct list * l);
  block * hook_LIST_get(struct list * l, int idx);

  static void * so_lib_handle() {
    static void * handle = NULL;

    if (handle == NULL) {
      handle = dlopen(NULL, RTLD_LAZY);
    }

    return handle;
  }

  static ffi_type getTypeFromSymbol(uint64_t symbol) {
    if (symbol == tag_type_void()) {
      return ffi_type_void;
    } else if (symbol == tag_type_uint8()) {
      return ffi_type_uint8;
    } else if (symbol == tag_type_sint8()) {
      return ffi_type_sint8;
    } else if (symbol == tag_type_uint16()) {
      return ffi_type_uint16;
    } else if (symbol == tag_type_sint16()) {
      return ffi_type_sint16;
    } else if (symbol == tag_type_uint32()) {
      return ffi_type_uint32;
    } else if (symbol == tag_type_sint32()) {
      return ffi_type_sint32;
    } else if (symbol == tag_type_uint64()) {
      return ffi_type_uint64;
    } else if (symbol == tag_type_sint64()) {
      return ffi_type_sint64;
    } else if (symbol == tag_type_float()) {
      return ffi_type_float;
    } else if (symbol == tag_type_double()) {
      return ffi_type_double;
    } else if (symbol == tag_type_uchar()) {
      return ffi_type_uchar;
    } else if (symbol == tag_type_schar()) {
      return ffi_type_schar;
    } else if (symbol == tag_type_ushort()) {
      return ffi_type_ushort;
    } else if (symbol == tag_type_sshort()) {
      return ffi_type_sshort;
    } else if (symbol == tag_type_uint()) {
      return ffi_type_uint;
    } else if (symbol == tag_type_sint()) {
      return ffi_type_sint;
    } else if (symbol == tag_type_ulong()) {
      return ffi_type_ulong;
    } else if (symbol == tag_type_slong()) {
      return ffi_type_slong;
    } else if (symbol == tag_type_longdouble()) {
      return ffi_type_longdouble;
    } else {
      throw std::invalid_argument("Arg is not a supported type");
    }
  }

  block * hook_FFI_call(mpz_t addr, struct list * args, struct list * types, block * ret) {
    ffi_cif cif;
    ffi_type *argtypes, rtype;
    void (* address)();

    if (!mpz_fits_ulong_p(addr)) {
      throw std::invalid_argument("Addr is too large");
    }
    *(void **)(&address) = (void *) mpz_get_ui(addr);

    mpz_ptr nargs_ptr = hook_LIST_size(args);
    mpz_ptr ntypes_ptr = hook_LIST_size(types);
    if (!mpz_fits_ulong_p(nargs_ptr)) {
      throw std::invalid_argument("Args list is too large");
    }
    if (!mpz_fits_ulong_p(ntypes_ptr)) {
      throw std::invalid_argument("Types list is too large");
    }

    size_t nargs = mpz_get_si(nargs_ptr);
    size_t ntypes = mpz_get_si(ntypes_ptr);

    if (nargs != ntypes) {
      throw std::invalid_argument("Args size does not match types size");
    }

    argtypes = (ffi_type *) malloc(sizeof(ffi_type) * nargs);

    block * type, * elem;
    ffi_type arg_t;
    size_t avalues_size = 0;
    for (int i = 0; i < nargs; i++) {
        elem = hook_LIST_get(types, i);
        if (elem->h.hdr != (uint64_t)getTagForSymbolName("inj{SortFFIType{}}")) {
          throw std::invalid_argument("Types list contains invalid FFI type");
        }
        type = (block *) *elem->children;
        arg_t = getTypeFromSymbol((uint64_t)type);
        argtypes[i] = arg_t;
        avalues_size += arg_t.size;
    }

    string * bytes;
    void * avalues = malloc(avalues_size);
    size_t len, offset = 0;
    for (int i = 0; i < nargs; i++) {
        elem = hook_LIST_get(args, i);
        if (elem->h.hdr != (uint64_t)getTagForSymbolName("inj{SortBytes{}}")) {
          throw std::invalid_argument("Args list contains non-bytes type");
        }
        bytes = (string *) *elem->children;
        len = len(bytes);
        memcpy((char *)avalues + offset, bytes, sizeof(char) * len);
        offset += len;
    }

    rtype = getTypeFromSymbol((uint64_t)ret);

    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, &rtype, &argtypes);
    free(argtypes);

    switch (status) {
      case FFI_OK:
        break;
      case FFI_BAD_TYPEDEF:
          throw std::invalid_argument("Types list contains invalid FFI type");
        break;
      case FFI_BAD_ABI:
          throw std::invalid_argument("Invalid ABI mode");
        break;
    }

    void * rvalue = malloc(rtype.size);
    ffi_call(&cif, address, rvalue, &avalues);

    free(avalues);

    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + rtype.size));
    retBlock->h = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortBytes{}}"));
    memcpy(retBlock->children, &rvalue, rtype.size);
    free(rvalue);

    return retBlock;
  }

  mpz_ptr hook_FFI_address(string * fn) {
    char * func = getTerminatedString(fn);
    void * handle = so_lib_handle();
    void * address = dlsym(handle, func);

    mpz_t result;
    mpz_init_set_ui(result, (uint64_t)address);
    return move_int(result);
  }
}
