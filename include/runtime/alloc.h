#ifndef ALLOC_H
#define ALLOC_H

extern "C" {

// The maximum single allocation size in bytes.
// A contiguous area larger than that size cannot be allocated in any arena.
extern const size_t BLOCK_SIZE;

#define YOUNGSPACE_ID 0
#define OLDSPACE_ID 1
#define ALWAYSGCSPACE_ID 3

char youngspace_collection_id(void);
char oldspace_collection_id(void);

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
// allocates exactly requested bytes into the always garbage-collected arena.
// objects that can potentially survive a collection (i.e. can be reached from the
// root during collection) should never be allocated in this arena.
void* koreAllocAlwaysGC(size_t requested);
// swaps the two semispace of the young generation as part of garbage collection
// if the swapOld flag is set, it also swaps the two semispaces of the old generation
void koreAllocSwap(bool swapOld);
// resizes the last allocation into the young generation
void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t oldrequest);
// allocator hook for the GMP library
void* koreAllocMP(size_t);
// reallocator hook for the GMP library
void* koreReallocMP(void*, size_t, size_t);
// deallocator hook for the GMP library
void koreFree(void*, size_t);

// helper allocators for integers and floats
// they allocate enough space into the corresponding generation and initialize the blockheader
// with the correct length. The size argument is ignored but exists for uniformity with
// the standard malloc signature. The caller has to set the appropriate gc bits.
void* koreAllocInteger(size_t requested);
void* koreAllocFloating(size_t requested);
void* koreAllocIntegerOld(size_t requested);
void* koreAllocFloatingOld(size_t requested);

#ifdef ALLOC_DBG
#define MEM_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define MEM_LOG(...)
#endif

}

#endif // ALLOC_H
