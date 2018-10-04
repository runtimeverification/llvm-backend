#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void* koreAlloc(size_t requested);
void* koreResizeLastAlloc(void* oldptr, size_t newrequest);

#ifdef __cplusplus
}
#endif

#endif // ALLOC_H
