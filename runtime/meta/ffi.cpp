#include <cstring>
#include <dlfcn.h>
#include <ffi.h>
#include <gmp.h>
#include <map>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "runtime/alloc.h"
#include "runtime/collect.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
#define TYPETAG(type) "Lbl'Hash'ffi'Unds'" #type "{}"

#define TAG_TYPE(NAME)                                                         \
  static uint64_t tag_type_##NAME() {                                          \
    static uint64_t tag = -1;                                                  \
    if (tag == -1) {                                                           \
      tag = (uint64_t)leaf_block(getTagForSymbolName(TYPETAG(NAME)));          \
    }                                                                          \
    return tag;                                                                \
  }

static block *dotK = leaf_block(getTagForSymbolName("dotk{}"));

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
#ifdef FFI_TARGET_HAS_COMPLEX_TYPE
TAG_TYPE(complexfloat)
TAG_TYPE(complexdouble)
TAG_TYPE(complexlongdouble)
#endif

char *getTerminatedString(string *str);

size_t hook_LIST_size_long(list *l);
block *hook_LIST_get_long(list *l, ssize_t idx);

static void *so_lib_handle() {
  static void *handle = nullptr;

  if (handle == nullptr) {
    handle = dlopen(nullptr, RTLD_LAZY);

    if (handle == nullptr) {
      KLLVM_HOOK_INVALID_ARGUMENT("dlopen returned nullptr");
    }
  }

  return handle;
}

// NOLINTBEGIN(*-else-after-return,*-cognitive-complexity)
static ffi_type *getTypeFromBlock(block *elem) {
  if (is_leaf_block(elem)) {
    auto symbol = (uint64_t)elem;

    // Some of these types are aliased to each other and so the branch clone
    // check produces false positives; it's clearer to keep it this way.
    // NOLINTBEGIN(*-branch-clone)
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
#ifdef FFI_TARGET_HAS_COMPLEX_TYPE
    else if (symbol == tag_type_complexfloat()) {
      return &ffi_type_complex_float;
    } else if (symbol == tag_type_complexdouble()) {
      return &ffi_type_complex_double;
    } else if (symbol == tag_type_complexlongdouble()) {
      return &ffi_type_complex_longdouble;
    }
#endif
  } else if (
      tag_hdr(elem->h.hdr) == (uint64_t)getTagForSymbolName(TYPETAG(struct))) {
    list *elements = (list *)*elem->children;
    size_t numFields = hook_LIST_size_long(elements);
    block *structField = nullptr;

    auto *structType = (ffi_type *)malloc(sizeof(ffi_type));
    structType->size = 0;
    structType->alignment = 0;
    structType->type = FFI_TYPE_STRUCT;
    structType->elements
        = (ffi_type **)malloc(sizeof(ffi_type *) * (numFields + 1));

    for (int j = 0; j < numFields; j++) {
      structField = hook_LIST_get_long(elements, j);

      if (tag_hdr(structField->h.hdr)
          != (uint64_t)getTagForSymbolName("inj{SortFFIType{}, SortKItem{}}")) {
        KLLVM_HOOK_INVALID_ARGUMENT("Struct list contains invalid FFI type");
      }

      structType->elements[j]
          = getTypeFromBlock((block *)*(structField->children));
    }

    structType->elements[numFields] = nullptr;

    structTypes.push_back(structType);

    return structType;
  }
  // NOLINTEND(*-branch-clone)

  KLLVM_HOOK_INVALID_ARGUMENT("Arg is not a supported type");
}
// NOLINTEND(*-else-after-return,*-cognitive-complexity)

// NOLINTNEXTLINE(*-cognitive-complexity)
string *ffiCall(
    bool isVariadic, mpz_t addr, list *args, list *fixtypes, list *vartypes,
    block *ret) {
  ffi_cif cif;
  ffi_type **argtypes = nullptr;
  ffi_type *rtype = nullptr;
  void (*address)() = nullptr;

  if (!mpz_fits_ulong_p(addr)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Addr is too large: {}", intToString(addr));
  }

  address = (void (*)())mpz_get_ui(addr);

  size_t nargs = hook_LIST_size_long(args);
  size_t nfixtypes = hook_LIST_size_long(fixtypes);
  size_t nvartypes = 0;

  if (isVariadic) {
    nvartypes = hook_LIST_size_long(vartypes);
  }

  if (nargs != (nfixtypes + nvartypes)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Args size does not match types size: args={}, types={}", nargs,
        (nfixtypes + nvartypes));
  }

  argtypes = (ffi_type **)malloc(sizeof(ffi_type *) * nargs);

  block *elem = nullptr;
  for (int i = 0; i < nfixtypes; i++) {
    elem = hook_LIST_get_long(fixtypes, i);
    if (tag_hdr(elem->h.hdr)
        != (uint64_t)getTagForSymbolName("inj{SortFFIType{}, SortKItem{}}")) {
      KLLVM_HOOK_INVALID_ARGUMENT("Fix types list contains invalid FFI type");
    }

    argtypes[i] = getTypeFromBlock((block *)*elem->children);
  }

  for (int i = 0; i < nvartypes; i++) {
    elem = hook_LIST_get_long(vartypes, i);
    if (tag_hdr(elem->h.hdr)
        != (uint64_t)getTagForSymbolName("inj{SortFFIType{}, SortKItem{}}")) {
      KLLVM_HOOK_INVALID_ARGUMENT("Var types list contains invalid FFI type");
    }

    argtypes[i + nfixtypes] = getTypeFromBlock((block *)*elem->children);
  }

  void **avalues = (void **)malloc(sizeof(void *) * nargs);
  for (int i = 0; i < nargs; i++) {
    elem = hook_LIST_get_long(args, i);
    if (tag_hdr(elem->h.hdr)
        != (uint64_t)getTagForSymbolName("inj{SortBytes{}, SortKItem{}}")) {
      KLLVM_HOOK_INVALID_ARGUMENT("Args list contains non-bytes type");
    }
    avalues[i] = ((string *)*elem->children)->data;
  }

  rtype = getTypeFromBlock(ret);

  ffi_status status = FFI_OK;
  if (isVariadic) {
    status = ffi_prep_cif_var(
        &cif, FFI_DEFAULT_ABI, nfixtypes, nargs, rtype, argtypes);
  } else {
    status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, rtype, argtypes);
  }

  switch (status) {
  case FFI_OK: break;
  case FFI_BAD_TYPEDEF:
    KLLVM_HOOK_INVALID_ARGUMENT("Types list contains invalid FFI type");
    break;
  case FFI_BAD_ABI:
    KLLVM_HOOK_INVALID_ARGUMENT("Invalid ABI mode");
    break;
    // The default case here is a hack to allow us to support
    // two different versions of libffi. From version 3.4
    // onwards, an enum variant FFI_BAD_ARGTYPE is defined. Our
    // CI doesn't yet use this version, so we use the default
    // instead.
  default: KLLVM_HOOK_INVALID_ARGUMENT("Bad FFI argument type"); break;
  }

  auto *rvalue
      = static_cast<string *>(koreAllocToken(sizeof(string) + rtype->size));
  ffi_call(&cif, address, (void *)(rvalue->data), avalues);

  free(argtypes);
  init_with_len(rvalue, rtype->size);
  free(avalues);

  for (auto &s : structTypes) {
    free(s->elements);
    free(s);
  }

  structTypes.clear();

  return rvalue;
}

SortBytes
hook_FFI_call(SortInt addr, SortList args, SortList types, SortFFIType ret) {
  return ffiCall(false, addr, args, types, nullptr, ret);
}

SortBytes hook_FFI_call_variadic(
    SortInt addr, SortList args, SortList fixtypes, SortList vartypes,
    SortFFIType ret) {
  return ffiCall(true, addr, args, fixtypes, vartypes, ret);
}

static std::map<std::string, void *> getPrivateSymbols() {
  std::map<std::string, void *> m;
  m["atexit"] = (void *)atexit;
#ifndef __APPLE__
  m["at_quick_exit"] = (void *)at_quick_exit;
  m["stat64"] = (void *)stat64;
  m["fstat64"] = (void *)fstat64;
  m["lstat64"] = (void *)lstat64;
  m["fstatat64"] = (void *)fstatat64;
  m["mknodat"] = (void *)mknodat;
#endif
  // Disabled: not present on Ubuntu Bionic
  // m["pthread_atfork"] = (void *)pthread_atfork;
  m["stat"] = (void *)stat;
  m["fstat"] = (void *)fstat;
  m["lstat"] = (void *)lstat;
  m["fstatat"] = (void *)fstatat;
  m["mknod"] = (void *)mknod;

  return m;
}

SortInt hook_FFI_address(SortString fn) {
  char *func = getTerminatedString(fn);

  std::string funcStr = func;
  static std::map<std::string, void *> const privateSymbols
      = getPrivateSymbols();

  void *address = nullptr;
  if (auto it = privateSymbols.find(funcStr); it != privateSymbols.end()) {
    address = it->second;
  } else {
    void *handle = so_lib_handle();
    address = dlsym(handle, func);
  }

  mpz_t result;
  mpz_init_set_ui(result, (uintptr_t)address);
  return move_int(result);
}

static std::pair<
    std::vector<block **>::iterator, std::vector<block **>::iterator>
firstBlockEnumerator() {
  // NOLINTBEGIN(*-const-cast)
  static std::vector<block **> blocks;

  blocks.clear();

  for (auto &keyVal : allocatedKItemPtrs) {
    blocks.push_back(const_cast<block **>(&(keyVal.first)));
  }

  return std::make_pair(blocks.begin(), blocks.end());
  // NOLINTEND(*-const-cast)
}

static std::pair<
    std::vector<block **>::iterator, std::vector<block **>::iterator>
secondBlockEnumerator() {
  // NOLINTBEGIN(*-const-cast)
  static std::vector<block **> blocks;

  blocks.clear();

  for (auto &keyVal : allocatedBytesRefs) {
    blocks.push_back(const_cast<block **>(&(keyVal.second)));
  }

  return std::make_pair(blocks.begin(), blocks.end());
  // NOLINTEND(*-const-cast)
}

string *hook_FFI_alloc(block *kitem, mpz_t size, mpz_t align) {
  static int registered = -1;

  if (registered == -1) {
    registerGCRootsEnumerator(firstBlockEnumerator);
    registerGCRootsEnumerator(secondBlockEnumerator);
    registered = 0;
  }

  if (!mpz_fits_ulong_p(size)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Size is too large: {}", intToString(size));
  }
  if (!mpz_fits_ulong_p(align)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Alignment is too large: {}", intToString(align));
  }

  size_t a = mpz_get_ui(align);

  if (allocatedKItemPtrs.find(kitem) != allocatedKItemPtrs.end()) {
    string *result = allocatedKItemPtrs[kitem];
    if ((((uintptr_t)result) & (a - 1)) != 0) {
      KLLVM_HOOK_INVALID_ARGUMENT("Memory is not aligned");
    }
    return allocatedKItemPtrs[kitem];
  }

  size_t s = mpz_get_ui(size);

  string *ret = nullptr;
  int result = posix_memalign(
      (void **)&ret, a < sizeof(void *) ? sizeof(void *) : a,
      sizeof(string *) + s);
  if (result) {
    KLLVM_HOOK_INVALID_ARGUMENT("Could not allocate");
  }
  memset(ret, 0, sizeof(string *) + s);
  init_with_len(ret, s);
  ret->h.hdr |= NOT_YOUNG_OBJECT_BIT;

  allocatedKItemPtrs[kitem] = ret;
  allocatedBytesRefs[ret] = kitem;

  return ret;
}

block *hook_FFI_free(block *kitem) {
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

block *hook_FFI_freeAll(void) {
  for (auto &allocatedKItemPtr : allocatedKItemPtrs) {
    hook_FFI_free(allocatedKItemPtr.first);
  }

  return dotK;
}

block *hook_FFI_bytes_ref(string *bytes) {
  auto refIter = allocatedBytesRefs.find(bytes);

  if (refIter == allocatedBytesRefs.end()) {
    KLLVM_HOOK_INVALID_ARGUMENT("Bytes have no reference");
  }

  return allocatedBytesRefs[bytes];
}

mpz_ptr hook_FFI_bytes_address(string *bytes) {
  mpz_t addr;
  mpz_init_set_ui(addr, (uintptr_t)bytes->data);
  return move_int(addr);
}

bool hook_FFI_allocated(block *kitem) {
  return allocatedKItemPtrs.find(kitem) != allocatedKItemPtrs.end();
}

SortK hook_FFI_read(SortInt addr, SortBytes mem) {
  unsigned long l = mpz_get_ui(addr);
  auto intptr = (uintptr_t)l;
  char *ptr = (char *)intptr;
  memcpy(mem->data, ptr, len(mem));
  return dotK;
}

SortK hook_FFI_write(SortInt addr, SortBytes mem) {
  unsigned long l = mpz_get_ui(addr);
  auto intptr = (uintptr_t)l;
  char *ptr = (char *)intptr;
  for (size_t i = 0; i < len(mem); ++i) {
    if (ptr[i] != mem->data[i]) {
      ptr[i] = mem->data[i];
    }
  }
  return dotK;
}
}
