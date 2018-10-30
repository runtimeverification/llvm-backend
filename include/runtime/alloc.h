#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#define mem_block_header(ptr) ((memory_block_header *)(((uintptr_t)(ptr)) & ~(BLOCK_SIZE-1)))

extern const size_t BLOCK_SIZE;
void* koreAlloc(size_t requested);
void koreAllocSwap(void);
void* koreAllocToken(size_t requested);
void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t oldrequest);

typedef struct {
  char* next_block;
  bool semispace;
} memory_block_header;

#ifdef ALLOC_DBG
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif // ALLOC_H
