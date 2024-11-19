
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

__attribute__((always_inline)) arena::memory_block_header *
arena::mem_block_header(void *ptr) {
  // NOLINTNEXTLINE(*-reinterpret-cast)
  return reinterpret_cast<arena::memory_block_header *>(
      ((uintptr_t)(ptr)-1) & ~(BLOCK_SIZE - 1));
}

__attribute__((always_inline)) char
arena::get_arena_collection_semispace_id() const {
  return ~allocation_semispace_id;
}

__attribute__((always_inline)) char
arena::get_arena_semispace_id_of_object(void *ptr) {
  return mem_block_header(ptr)->semispace;
}

//
//	We will reserve enough address space for 1 million 1MB blocks. Might want to increase this on a > 1TB server.
//
size_t const HYPERBLOCK_SIZE = (size_t)BLOCK_SIZE * 1024 * 1024;

void *arena::megabyte_malloc() {
  //
  //	Return pointer to a BLOCK_SIZE chunk of memory with BLOCK_SIZE alignment.
  //
  if (current_block_ptr) {
    //
    //	We expect an page fault due to not being able to map physical memory to this block or the
    //	process to be killed by the OOM killer long before we run off the end of our address space.
    //
    current_block_ptr += BLOCK_SIZE;
  } else {
    //
    //	First call - need to reserve the address space.
    //
    size_t request = HYPERBLOCK_SIZE;
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
    //	We ask for one block worth of address space less than we allocated so alignment will always succeed.
    //	We don't worry about unused address space either side of our aligned address space because there will be no
    //	memory mapped to it.
    //
    current_block_ptr = reinterpret_cast<char *>(
        std::align(BLOCK_SIZE, HYPERBLOCK_SIZE - BLOCK_SIZE, addr, request));
  }
  return current_block_ptr;
}

#ifdef __MACH__
//
//	thread_local disabled for Apple
//
bool time_for_collection;
#else
thread_local bool time_for_collection;
#endif

void arena::fresh_block() {
  char *next_block = nullptr;
  if (block_start == nullptr) {
    next_block = (char *)megabyte_malloc();
    first_block = next_block;
    auto *next_header = (arena::memory_block_header *)next_block;
    next_header->next_block = nullptr;
    next_header->semispace = allocation_semispace_id;
    num_blocks++;
  } else {
    next_block = *(char **)block_start;
    if (block != block_end) {
      if (block_end - block == 8) {
        *(uint64_t *)block = NOT_YOUNG_OBJECT_BIT; // 8 bit sentinel value
      } else {
        *(uint64_t *)block
            = block_end - block - 8; // 16-bit or more sentinel value
      }
    }
    if (!next_block) {
      MEM_LOG(
          "Allocating new block for the first time in arena %d\n",
          allocation_semispace_id);
      next_block = (char *)megabyte_malloc();
      *(char **)block_start = next_block;
      auto *next_header = (arena::memory_block_header *)next_block;
      next_header->next_block = nullptr;
      next_header->semispace = allocation_semispace_id;
      num_blocks++;
      time_for_collection = true;
    }
  }
  if (!*(char **)next_block && num_blocks >= get_gc_threshold()) {
    time_for_collection = true;
  }
  block = next_block + sizeof(arena::memory_block_header);
  block_start = next_block;
  block_end = next_block + BLOCK_SIZE;
  MEM_LOG(
      "New block at %p (remaining %zd)\n", block,
      BLOCK_SIZE - sizeof(arena::memory_block_header));
}

#ifdef __MACH__
//
//	thread_local disabled for Apple
//
bool gc_enabled = true;
#else
thread_local bool gc_enabled = true;
#endif

__attribute__((noinline)) void *arena::do_alloc_slow(size_t requested) {
  MEM_LOG(
      "Block at %p too small, %zd remaining but %zd needed\n", block,
      block_end - block, requested);
  if (requested > BLOCK_SIZE - sizeof(arena::memory_block_header)) {
    return malloc(requested);
  }
  fresh_block();
  void *result = block;
  block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, block);
  return result;
}

__attribute__((always_inline)) void *
arena::arena_resize_last_alloc(ssize_t increase) {
  if (block + increase <= block_end) {
    block += increase;
    return block;
  }
  return nullptr;
}

__attribute__((always_inline)) void arena::arena_swap_and_clear() {
  std::swap(first_block, first_collection_block);
  std::swap(num_blocks, num_collection_blocks);
  std::swap(current_block_ptr, collection_block_ptr);
  allocation_semispace_id = ~allocation_semispace_id;
  arena_clear();
}

__attribute__((always_inline)) void arena::arena_clear() {
  block = first_block ? first_block + sizeof(arena::memory_block_header) : nullptr;
  block_start = first_block;
  block_end = first_block ? first_block + BLOCK_SIZE : nullptr;
}

__attribute__((always_inline)) char *arena::arena_start_ptr() const {
  return first_block ? first_block + sizeof(arena::memory_block_header) : nullptr;
}

__attribute__((always_inline)) char **arena::arena_end_ptr() {
  return &block;
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

ssize_t arena::ptr_diff(char *ptr1, char *ptr2) {
  if (MEM_BLOCK_START(ptr1) == MEM_BLOCK_START(ptr2)) {
    return ptr1 - ptr2;
  }
  arena::memory_block_header *hdr = mem_block_header(ptr2);
  ssize_t result = 0;
  while (hdr != mem_block_header(ptr1) && hdr->next_block) {
    if (ptr2) {
      result += ((char *)hdr + BLOCK_SIZE) - ptr2;
      ptr2 = nullptr;
    } else {
      result += (BLOCK_SIZE - sizeof(arena::memory_block_header));
    }
    hdr = (arena::memory_block_header *)hdr->next_block;
  }
  if (hdr == mem_block_header(ptr1)) {
    result += ptr1 - (char *)(hdr + 1);
    return result;
  } // reached the end of the arena and didn't find the block
  // it's possible that the result should be negative, in which
  // case the block will have been prior to the block we started
  // at. To handle this, we recurse with reversed arguments and
  // negate the result. This means that the code might not
  // terminate if the two pointers do not belong to the same
  // arena.
  return -ptr_diff(ptr2, ptr1);
}

size_t arena::arena_size() const {
  return (num_blocks > num_collection_blocks ? num_blocks
                                             : num_collection_blocks)
         * (BLOCK_SIZE - sizeof(arena::memory_block_header));
}
