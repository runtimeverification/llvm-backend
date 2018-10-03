#ifndef ALLOC_H
#define ALLOC_H

void* koreAlloc(size_t requested);
void* koreResizeLastAlloc(void* oldptr, size_t newrequest);

#endif // ALLOC_H
