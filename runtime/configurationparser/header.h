#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <stdint.h>
#include <gmp.h>
#include <mpfr.h>

extern "C" {
  // llvm: blockheader = type { i64 } 
  struct blockheader {
    uint64_t header;
  };

  // llvm: block = type { %blockheader, [0 x i64 *] }
  struct block {
    blockheader header;
    uint64_t *children[];
  };

  
  // llvm: string = type { %blockheader, [0 x i8] }
  struct string {
    blockheader header;
    char bytes[];
  };

  // llvm: map = type { i64, i8 *, i8 * }
  struct map {
    uint64_t a;
    void *b;
    void *c;
  };

  // llvm: set = type { i8 *, i8 *, i64 }
  struct set {
    void *a;
    void *b;
    uint64_t c;
  };

  // llvm: list = type { i64, i64, i8 *, i8 *, i8 *, i8 *, i8 * }
  struct list {
    uint64_t a;
    uint64_t b;
    void *c;
    void *d;
    void *e;
    void *f;
    char *g;
  };
 
  // This function is exported to be used by the interpreter 
  block *parseConfiguration(const char *filename);
  void printConfiguration(int fd, block *subject);
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
      void visitFloat(FILE *, mpfr_t, const char *),
      void visitBool(FILE *, bool, const char *),
      void visitMInt(FILE *, void *, const char *),
      void visitSeparator(FILE *));
}

#endif // RUNTIME_HEADER_H
