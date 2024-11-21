
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sys/mman.h>

#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"

extern size_t const VAR_BLOCK_SIZE = BLOCK_SIZE;
size_t const HYPERBLOCK_SIZE = (size_t)BLOCK_SIZE * 1024 * 1024;


__attribute__((always_inline)) arena::memory_block_header *
arena::mem_block_header(void *ptr) {
  // NOLINTNEXTLINE(*-reinterpret-cast)
  return reinterpret_cast<arena::memory_block_header *>(
      ((uintptr_t)(ptr)-1) & ~(HYPERBLOCK_SIZE - 1));
}

__attribute__((always_inline)) char
arena::get_arena_semispace_id_of_object(void *ptr) {
  return mem_block_header(ptr)->semispace;
}


#ifdef __MACH__
//
//	thread_local disabled for Apple
//
bool time_for_collection;
#else
thread_local bool time_for_collection;
#endif

#ifdef __MACH__
//
//	thread_local disabled for Apple
//
bool gc_enabled = true;
#else
thread_local bool gc_enabled = true;
#endif

__attribute__((always_inline)) void arena::arena_swap_and_clear() {
  std::swap(current_addr_ptr, collection_addr_ptr);
  std::swap(current_tripwire, collection_tripwire);
  allocation_semispace_id = ~allocation_semispace_id;
  arena_clear();
}

char *arena::move_ptr(char *ptr, size_t size, char const *arena_end_ptr) {
  char *next_ptr = ptr + size;
  if (next_ptr == arena_end_ptr) {
    return nullptr;
  }
  if (next_ptr != MEM_BLOCK_START(ptr) + BLOCK_SIZE) {
    return next_ptr;
  }
  char *next_block = *(char **)MEM_BLOCK_START(ptr);
  if (!next_block) {
    return nullptr;
  }
  return next_block + sizeof(arena::memory_block_header);
}

size_t arena::arena_size() const {
  size_t current_size = current_addr_ptr ? (BLOCK_SIZE + current_tripwire - current_addr_ptr) : 0;
  size_t collection_size = collection_addr_ptr ? (BLOCK_SIZE + collection_tripwire - collection_addr_ptr) : 0;
  return std::max(current_size, collection_size);
}

void *arena::slow_alloc(size_t requested) {
  //
  //	This allocation will push the allocation_ptr beyond the tripwire
  //	into or past the cushion area between allocation_ptr and the furthest
  //	allocated location.
  //
  if (current_tripwire == nullptr) {
    //
    //	No address space has been reserved for this semispace.
    //
    size_t request = 2 * HYPERBLOCK_SIZE;
    void *addr = mmap(
		      nullptr, // let OS choose the address
		      request, // Linux and MacOS both allow up to 64TB
		      PROT_READ | PROT_WRITE, // read, write but not execute
		      MAP_ANONYMOUS | MAP_PRIVATE
		      | MAP_NORESERVE, // allocate address space only
		      -1, // no file backing
		      0); // no offset
    if (addr == MAP_FAILED) {
      perror("mmap()");
      abort();
    }
    //
    //	We allocated 2 * HYPERBLOCK_SIZE worth of address space but we're only going to use 1, aligned on a
    //	HYPERBLOCK_SIZE boundry. This is so we can get the start of the hyperblock by masking any address within it.
    //	We don't worry about unused address space either side of our aligned address space because there will be no
    //	memory mapped to it.
    //
    current_addr_ptr = reinterpret_cast<char *>(
        std::align(HYPERBLOCK_SIZE, HYPERBLOCK_SIZE, addr, request));
    memory_block_header *header = reinterpret_cast<memory_block_header *>(current_addr_ptr);
    header->next_block = nullptr;
    header->semispace = allocation_semispace_id;
    allocation_ptr = current_addr_ptr + sizeof(arena::memory_block_header);
    current_tripwire = current_addr_ptr + BLOCK_SIZE;
  }
  else {
    //
    //	Need a garbage collection. We also move the tripwire so we don't hit it repeatedly.
    //	We always move the tripwire to a BLOCK_SIZE boundry.
    //
    time_for_collection = true;
    while (allocation_ptr + requested >= current_tripwire)
      current_tripwire += BLOCK_SIZE;
  }

  void *result = allocation_ptr;
  allocation_ptr += requested;
  MEM_LOG("Slow allocation at %p (size %zd), next alloc at %p\n", result, requested, block);
  return result;
}
