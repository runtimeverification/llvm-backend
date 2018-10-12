#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void* koreAlloc(size_t requested);
void* koreResizeLastAlloc(void* oldptr, size_t newrequest);

#define len(s) ((s)->b.len & 0xffff3fffffffffff)
#define set_len(s, l) ((s)->b.len = (l) | ((s)->b.len & 0xc00000000000))

#ifdef __cplusplus
}
#endif

#endif // ALLOC_H
