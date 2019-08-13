#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <stdint.h>
#include <gmp.h>
#include <mpfr.h>
#include "config/macros.h"

// the actual length is equal to the block header with the gc bits masked out.

#define len(s) len_hdr((s)->h.hdr)
#define len_hdr(s) ((s) & 0xffffffffff)
#define set_len(s, l) ((s)->h.hdr = (l) | (l > BLOCK_SIZE - sizeof(char *) ? NOT_YOUNG_OBJECT_BIT : 0))
#define size_hdr(s) ((((s) >> 32) & 0xff) * 8)
#define layout(s) layout_hdr((s)->h.hdr)
#define layout_hdr(s) ((s) >> LAYOUT_OFFSET)
#define tag_hdr(s) (s & 0xffffffffLL)
#define is_in_young_gen_hdr(s) (!((s) & NOT_YOUNG_OBJECT_BIT))
#define is_in_old_gen_hdr(s) \
        (((s) & NOT_YOUNG_OBJECT_BIT) && ((s) & YOUNG_AGE_BIT))
#define reset_gc(s) ((s)->h.hdr = (s)->h.hdr & ~(NOT_YOUNG_OBJECT_BIT | YOUNG_AGE_BIT | FWD_PTR_BIT))
#define struct_base(struct_type, member_name, member_addr) \
        ((struct_type *)((char *)(member_addr) - offsetof(struct_type, member_name)))

#ifdef __cplusplus
extern "C" {
#endif
  // llvm: blockheader = type { i64 } 
  typedef struct blockheader {
    uint64_t hdr;
  } blockheader;

  // A value b of type block* is either a constant or a block.
  // if (((uintptr_t)b) & 3) == 3, then it is a bound variable and
  // ((uintptr_t)b) >> 32 is the debruijn index. If ((uintptr_t)b) & 3 == 1)
  // then it is a symbol with 0 arguments and ((uintptr_t)b) >> 32 is the tag
  // of the symbol. Otherwise, if ((uintptr_t)b) & 1 == 0 then it is a pointer to
  // a block.
  // llvm: block = type { %blockheader, [0 x i64 *] }
  typedef struct block {
    blockheader h;
    uint64_t *children[];
  } block;

  
  // llvm: string = type { %blockheader, [0 x i8] }
  typedef struct string {
    blockheader h;
    char data[];
  } string;
  
  // llvm: stringbuffer = type { i64, i64, %string* }
  typedef struct stringbuffer {
    blockheader h;
    uint64_t strlen;
    string *contents;
  } stringbuffer;

  // llvm: map = type { i64, i8 *, i8 * }
  typedef struct map {
    uint64_t a;
    void *b;
    void *c;
  } map;

  // llvm: set = type { i8 *, i8 *, i64 }
  typedef struct set {
    void *a;
    void *b;
    uint64_t c;
  } set;

  typedef struct iter {
    uint64_t a;
    struct {
      struct {
        uint64_t *b;
        uint64_t c;
      };
      uint64_t d;
    };
    struct {
      struct {
        uint64_t e;
        uint32_t f;
      };
      void *g;
    };
    struct {
      uint64_t h;
      uint64_t i[3];
    };
  } iter;

  typedef struct list {
    uint64_t size;
    uint32_t shift;
    uint8_t * root;
    uint8_t * tail;
  } list;

  typedef struct mpz_hdr {
    blockheader h;
    mpz_t i;
  } mpz_hdr;

  typedef struct floating {
    uint64_t exp; // number of bits in exponent range
    mpfr_t f;
  } floating;

  typedef struct floating_hdr {
    blockheader h;
    floating f;
  } floating_hdr;

  typedef struct {
    uint64_t offset;
    uint16_t cat;
  } layoutitem;

  typedef struct {
    uint8_t nargs;
    layoutitem *args;
  } layout;

  // This function is exported to be used by the interpreter 
  #ifdef __cplusplus
  extern "C++" {
    std::string floatToString(const floating *);
    void init_float2(floating *, std::string);
  }
  #endif
  block *parseConfiguration(const char *filename);
  void printConfiguration(const char *filename, block *subject);
  void printConfigurationInternal(FILE *file, block *subject, const char *sort, bool);
  mpz_ptr move_int(mpz_t);

  // The following functions have to be generated at kompile time
  // and linked with the interpreter.
  uint32_t getTagForSymbolName(const char *symbolname);
  struct blockheader getBlockHeaderForSymbol(uint32_t tag);
  bool isSymbolAFunction(uint32_t tag);
  bool isSymbolABinder(uint32_t tag);
  void storeSymbolChildren(block *symbol, void *children[]);
  void *evaluateFunctionSymbol(uint32_t tag, void *arguments[]);
  void *getToken(const char *sortname, uint64_t len, const char *tokencontents);
  layout *getLayoutData(uint16_t);
  uint32_t getInjectionForSortOfTag(uint32_t tag);

  bool hook_STRING_eq(const string *, const string *);

  const char *getSymbolNameForTag(uint32_t tag);
  const char *topSort(void);
  void printMap(FILE *, map *, const char *, const char *, const char *);
  void printSet(FILE *, set *, const char *, const char *, const char *);
  void printList(FILE *, list *, const char *, const char *, const char *);
  void visitChildren(block *subject, FILE *file,
      void visitConfig(FILE *, block *, const char *, bool), 
      void visitMap(FILE *, map *, const char *, const char *, const char *), 
      void visitList(FILE *, list *, const char *, const char *, const char *), 
      void visitSet(FILE *, set *, const char *, const char *, const char *), 
      void visitInt(FILE *, mpz_t, const char *),
      void visitFloat(FILE *, floating *, const char *),
      void visitBool(FILE *, bool, const char *),
      void visitStringBuffer(FILE *, stringbuffer *, const char *),
      void visitMInt(FILE *, void *, const char *),
      void visitSeparator(FILE *));

  block *debruijnize(block *);

  iter set_iterator(set *);
  block *set_iterator_next(iter *);
  iter map_iterator(set *);
  block *map_iterator_next(iter *);

  extern const uint32_t first_inj_tag, last_inj_tag;

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_HEADER_H
