#include <cstring>
#include <dlfcn.h>
#include <ffi.h>
#include <gmp.h>
#include <stdexcept>
#include <vector>
#include <map>
#include <unordered_map>

#include "runtime/alloc.h"
#include "runtime/collect.h"
#include "runtime/header.h"

extern "C" {

  bool hook_KEQUAL_eq(block *, block *);

#define KCHAR char
#define TYPETAG(type) "Lbl'Hash'" #type "{}"

#define TAG_TYPE(NAME) static uint64_t tag_type_##NAME() {\
  static uint64_t tag = -1; \
  if (tag == -1) { \
    tag = ((uint64_t)getTagForSymbolName(TYPETAG(NAME)) << 32) | 1; \
  } \
  return tag; \
}

  struct KEq {
    bool operator() (block * const& lhs, block * const& rhs) const {
      return hook_KEQUAL_eq(lhs, rhs);
    }
  };

  static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);

  thread_local static std::vector<ffi_type *> structTypes;

  static std::unordered_map<block *, string *, HashBlock, KEq> allocatedKItemPtrs;
  static std::map<string *, block *> allocatedBytesRefs;

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
  TAG_TYPE(pointer)

  mpz_ptr move_int(mpz_t);
  char * getTerminatedString(string * str);

  size_t hook_LIST_size_long(list * l);
  block * hook_LIST_get_long(list * l, ssize_t idx);

  static void * so_lib_handle() {
    static void * handle = NULL;

    if (handle == NULL) {
      handle = dlopen(NULL, RTLD_LAZY);

      if (handle == NULL) {
        throw std::invalid_argument("dlopen returned NULL");
      }
    }

    return handle;
  }

  static ffi_type * getTypeFromBlock(block * elem) {
    if ((uint64_t) elem & 1) {
      uint64_t symbol = (uint64_t) elem;

      if (symbol == tag_type_void()) {
        return &ffi_type_void;
      } else if (symbol == tag_type_uint8()) {
        return &ffi_type_uint8;
      } else if (symbol == tag_type_sint8()) {
        return &ffi_type_sint8;
      } else if (symbol == tag_type_uint16()) {
        return &ffi_type_uint16;
      } else if (symbol == tag_type_sint16()) {
        return &ffi_type_sint16;
      } else if (symbol == tag_type_uint32()) {
        return &ffi_type_uint32;
      } else if (symbol == tag_type_sint32()) {
        return &ffi_type_sint32;
      } else if (symbol == tag_type_uint64()) {
        return &ffi_type_uint64;
      } else if (symbol == tag_type_sint64()) {
        return &ffi_type_sint64;
      } else if (symbol == tag_type_float()) {
        return &ffi_type_float;
      } else if (symbol == tag_type_double()) {
        return &ffi_type_double;
      } else if (symbol == tag_type_uchar()) {
        return &ffi_type_uchar;
      } else if (symbol == tag_type_schar()) {
        return &ffi_type_schar;
      } else if (symbol == tag_type_ushort()) {
        return &ffi_type_ushort;
      } else if (symbol == tag_type_sshort()) {
        return &ffi_type_sshort;
      } else if (symbol == tag_type_uint()) {
        return &ffi_type_uint;
      } else if (symbol == tag_type_sint()) {
        return &ffi_type_sint;
      } else if (symbol == tag_type_ulong()) {
        return &ffi_type_ulong;
      } else if (symbol == tag_type_slong()) {
        return &ffi_type_slong;
      } else if (symbol == tag_type_longdouble()) {
        return &ffi_type_longdouble;
      } else if (symbol == tag_type_pointer()) {
        return &ffi_type_pointer;
      }
    } else if (elem->h.hdr == (uint64_t)getTagForSymbolName(TYPETAG(struct))){
      list * elements = (list *) *elem->children;
      size_t numFields = hook_LIST_size_long(elements);
      block * structField;

      ffi_type * structType = (ffi_type *) malloc(sizeof(ffi_type));
      structType->size = 0;
      structType->alignment = 0;
      structType->type = FFI_TYPE_STRUCT;
      structType->elements = (ffi_type **) malloc(sizeof(ffi_type *) * (numFields + 1));

      for (int j = 0; j < numFields; j++) {
        structField = hook_LIST_get_long(elements, j);

        if (structField->h.hdr != (uint64_t)getTagForSymbolName("inj{SortFFIType{}}")) {
          throw std::invalid_argument("Struct list contains invalid FFI type");
        }

        structType->elements[j]= getTypeFromBlock((block *) *(structField->children));
      }

      structType->elements[numFields] = NULL;

      structTypes.push_back(structType);

      return structType;
    }

    throw std::invalid_argument("Arg is not a supported type");
  }

  string * ffiCall(bool isVariadic, mpz_t addr, list * args, list * fixtypes, list * vartypes, block * ret) {
    ffi_cif cif;
    ffi_type ** argtypes, * rtype;
    void (* address)(void);

    if (!mpz_fits_ulong_p(addr)) {
      throw std::invalid_argument("Addr is too large");
    }

    address = (void (*) (void))  mpz_get_ui(addr);

    size_t nargs = hook_LIST_size_long(args);
    size_t nfixtypes = hook_LIST_size_long(fixtypes);
    size_t nvartypes = 0;

    if (isVariadic) {
      nvartypes = hook_LIST_size_long(vartypes);
    }

    if (nargs != (nfixtypes + nvartypes)) {
      throw std::invalid_argument("Args size does not match types size");
    }

    argtypes = (ffi_type **) malloc(sizeof(ffi_type *) * nargs);

    block * elem;
    for (int i = 0; i < nfixtypes; i++) {
        elem = hook_LIST_get_long(fixtypes, i);
        if (elem->h.hdr != (uint64_t)getTagForSymbolName("inj{SortFFIType{}}")) {
          throw std::invalid_argument("Fix types list contains invalid FFI type");
        }

        argtypes[i] = getTypeFromBlock((block *) *elem->children);
    }

    for (int i = 0; i < nvartypes; i++) {
        elem = hook_LIST_get_long(vartypes, i);
        if (elem->h.hdr != (uint64_t)getTagForSymbolName("inj{SortFFIType{}}")) {
          throw std::invalid_argument("Var types list contains invalid FFI type");
        }

        argtypes[i + nfixtypes] = getTypeFromBlock((block *) *elem->children);
    }

    void ** avalues = (void **) malloc(sizeof(void *) * nargs);
    for (int i = 0; i < nargs; i++) {
        elem = hook_LIST_get_long(args, i);
        if (elem->h.hdr != (uint64_t)getTagForSymbolName("inj{SortBytes{}}")) {
          throw std::invalid_argument("Args list contains non-bytes type");
        }
        avalues[i] = ((string *) *elem->children)->data;
    }

    rtype = getTypeFromBlock(ret);

    ffi_status status;
    if (isVariadic) {
      status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, nfixtypes, nargs, rtype, argtypes);
    } else {
      status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, rtype, argtypes);
    }

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

    string * rvalue = static_cast<string *>(koreAlloc(rtype->size));
    ffi_call(&cif, address, (void *)(rvalue->data), avalues);

    set_len(rvalue, rtype->size);
    free(avalues);

    for (auto &s : structTypes) {
      free(s->elements);
      free(s);
    }

    structTypes.clear();

    return rvalue;
  }

  string * hook_FFI_call(mpz_t addr, list * args, list * types, block * ret) {
    return ffiCall(false, addr, args, types, NULL, ret);
  }

  string * hook_FFI_call_variadic(mpz_t addr, list * args, list * fixtypes, list * vartypes, block * ret) {
    return ffiCall(true, addr, args, fixtypes, vartypes, ret);
  }

  mpz_ptr hook_FFI_address(string * fn) {
    char * func = getTerminatedString(fn);
    void * handle = so_lib_handle();
    void * address = dlsym(handle, func);

    mpz_t result;
    mpz_init_set_ui(result, (uintptr_t)address);
    return move_int(result);
  }

  static std::pair<std::vector<block **>::iterator, std::vector<block **>::iterator> firstBlockEnumerator() {
    static std::vector<block **> blocks;

    for (auto &keyVal : allocatedKItemPtrs) {
      blocks.push_back(const_cast<block**>(&(keyVal.first)));
    }

    return std::make_pair(blocks.begin(), blocks.end());
  }

  static std::pair<std::vector<block **>::iterator, std::vector<block **>::iterator> secondBlockEnumerator() {
    static std::vector<block **> blocks;

    for (auto &keyVal : allocatedBytesRefs) {
      blocks.push_back(const_cast<block**>(&(keyVal.second)));
    }

    return std::make_pair(blocks.begin(), blocks.end());
  }

  string * hook_FFI_alloc(block * kitem, mpz_t size) {
    static int registered = -1;

    if (registered == -1) {
      registerGCRootsEnumerator(firstBlockEnumerator);
      registerGCRootsEnumerator(secondBlockEnumerator);
      registered = 0;
    }

    if (!mpz_fits_ulong_p(size)) {
      throw std::invalid_argument("Size is too large");
    }

    if (allocatedKItemPtrs.find(kitem) != allocatedKItemPtrs.end()) {
      return allocatedKItemPtrs[kitem];
    }

    size_t s = mpz_get_ui(size);

    string * ret = (string *) calloc(sizeof(string *) + s, 1);
    set_len(ret, s);

    allocatedKItemPtrs[kitem] = ret;
    allocatedBytesRefs[ret] = kitem;

    return ret;
  }

  block * hook_FFI_free(block * kitem) {
    auto ptrIter = allocatedKItemPtrs.find(kitem);
    auto refIter = allocatedBytesRefs.find(ptrIter->second);

    if (ptrIter != allocatedKItemPtrs.end()) {
      free(allocatedKItemPtrs[kitem]);
      allocatedKItemPtrs.erase(ptrIter);

      if (refIter != allocatedBytesRefs.end()) {
        allocatedBytesRefs.erase(refIter);
      } else {
        throw std::runtime_error("Internal memory map is out of sync");
      }
    }

    return dotK;
  }

  block * hook_FFI_bytes_ref(string * bytes) {
    auto refIter = allocatedBytesRefs.find(bytes);

    if (refIter == allocatedBytesRefs.end()) {
      throw std::invalid_argument("Bytes have no reference");
    }

    return allocatedBytesRefs[bytes];
  }

  mpz_ptr hook_FFI_bytes_address(string * bytes) {
    mpz_t addr;
    mpz_init_set_ui(addr, (uintptr_t)bytes->data);
    return move_int(addr);
  }

  bool hook_FFI_allocated(block * kitem) {
    return allocatedKItemPtrs.find(kitem) != allocatedKItemPtrs.end();
  }
}

