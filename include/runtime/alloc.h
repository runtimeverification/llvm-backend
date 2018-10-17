#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

extern const size_t BLOCK_SIZE;
void* koreAlloc(size_t requested);
void koreAllocSwap(void);
void* koreAllocToken(size_t requested);
void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t oldrequest);

#define len(s) ((s)->b.len & 0xffff3fffffffffff)
#define set_len(s, l) ((s)->b.len = (l) | (l > BLOCK_SIZE - sizeof(char *) ? 0x400000000000 : 0))

#ifdef ALLOC_DBG
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif // ALLOC_H
