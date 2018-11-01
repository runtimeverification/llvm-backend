#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#define mem_block_header(ptr) ((memory_block_header *)(((uintptr_t)(ptr)) & ~(BLOCK_SIZE-1)))

extern const size_t BLOCK_SIZE;
// allocates exactly requested bytes into the young generation
void* koreAlloc(size_t requested);
// allocates enough space for a string token whose raw size is requested into the young generation.
// rounds up to the nearest 8 bytes and always allocates at least 16 bytes
void* koreAllocToken(size_t requested);
// allocates exactly requested bytes into the old generation
void* koreAllocOld(size_t requested);
// allocates enough space for a string token whose raw size is requested into the old generation.
// rounds up to the nearest 8 bytes and always allocates at least 16 bytes
void* koreAllocTokenOld(size_t requested);
// swaps the two semispace of the young generation as part of garbage collection
void koreAllocSwap(void);
// resizes the last allocation into the young generation
void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t oldrequest);

typedef struct {
  char* next_block;
  char semispace;
} memory_block_header;

#ifdef ALLOC_DBG
#define MEM_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define MEM_LOG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif // ALLOC_H
