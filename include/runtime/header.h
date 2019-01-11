#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <stdint.h>
#include <gmp.h>
#include <mpfr.h>

// the actual length is equal to the block header with the gc bits masked out.
#define len(s) len_hdr((s)->h.hdr)
#define len_hdr(s) ((s) & 0x1fffffffffff)
#define set_len(s, l) ((s)->h.hdr = (l) | (l > BLOCK_SIZE - sizeof(char *) ? 0x400000000000 : 0))
#define size_hdr(s) ((((s) >> 32) & 0xff) * 8)
#define layout_hdr(s) ((s) >> 48)
#define NOT_YOUNG_OBJECT_BIT 0x400000000000
#define YOUNG_AGE_BIT 0x200000000000
#define FWD_PTR_BIT 0x800000000000

#define MAP_LAYOUT 1
#define LIST_LAYOUT 2
#define SET_LAYOUT 3
#define INT_LAYOUT 4
#define FLOAT_LAYOUT 5
#define STRINGBUFFER_LAYOUT 6
#define BOOL_LAYOUT 7
#define SYMBOL_LAYOUT 8

#ifdef __cplusplus
extern "C" {
#endif
  // llvm: blockheader = type { i64 } 
  typedef struct blockheader {
    uint64_t hdr;
  } blockheader;

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
  
  // llvm: stringbuffer = type { i64, %string* }
  typedef struct stringbuffer {
    uint64_t capacity;
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

  // llvm: list = type { i64, i64, i8 *, i8 *, i8 *, i8 *, i8 * }
  typedef struct list {
    uint64_t a;
    uint64_t b;
    void *c;
    void *d;
    void *e;
    void *f;
    char *g;
  } list;

  typedef struct floating {
    uint64_t exp;
    mpfr_t f;
  } floating;
 
  // This function is exported to be used by the interpreter 
  #ifdef __cplusplus
  extern "C++" {
    std::string floatToString(const floating *);
    void init_float2(floating *, std::string);
  }
  #endif
  block *parseConfiguration(const char *filename);
  void printConfiguration(const char *filename, block *subject);
  void printConfigurationInternal(FILE *file, block *subject, const char *sort);

  // The following functions have to be generated at kompile time
  // and linked with the interpreter.
  uint32_t getTagForSymbolName(const char *symbolname);
  struct blockheader getBlockHeaderForSymbol(uint32_t tag);
  bool isSymbolAFunction(uint32_t tag);
  void storeSymbolChildren(block *symbol, void *children[]);
  void *evaluateFunctionSymbol(uint32_t tag, void *arguments[]);
  void *getToken(const char *sortname, uint64_t len, const char *tokencontents);

  const char *getSymbolNameForTag(uint32_t tag);
  const char *topSort(void);
  void printMap(FILE *, map *, const char *, const char *, const char *);
  void printSet(FILE *, set *, const char *, const char *, const char *);
  void printList(FILE *, list *, const char *, const char *, const char *);
  void visitChildren(block *subject, FILE *file,
      void visitConfig(FILE *, block *, const char *), 
      void visitMap(FILE *, map *, const char *, const char *, const char *), 
      void visitList(FILE *, list *, const char *, const char *, const char *), 
      void visitSet(FILE *, set *, const char *, const char *, const char *), 
      void visitInt(FILE *, mpz_t, const char *),
      void visitFloat(FILE *, floating *, const char *),
      void visitBool(FILE *, bool, const char *),
      void visitMInt(FILE *, void *, const char *),
      void visitSeparator(FILE *));

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_HEADER_H
