#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sys/mman.h>

#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/collect.h"
#include "runtime/header.h"

extern size_t const VAR_BLOCK_SIZE = BLOCK_SIZE;

#ifdef __MACH__
//
//	thread_local disabled for Apple
//
bool time_for_collection = false;
bool gc_enabled = true;
#else
thread_local bool time_for_collection = false;
thread_local bool gc_enabled = true;
#endif

void arena::initialize_semispace() {
  //
  //	Current semispace is uninitialized so mmap() a big chuck of address space.
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
  //	std::align() may modify addr and request.
  //
  auto *start_block = reinterpret_cast<char *>(addr);
  auto *end_block = start_block + request;
  //
  //	We allocated 2 * HYPERBLOCK_SIZE worth of address space but we're only going to use 1, aligned on a
  //	HYPERBLOCK_SIZE boundry. This is so we can get end of the hyperblock by setting the low bits of any
  //	address within the space to 1.
  //
  current_addr_ptr = reinterpret_cast<char *>(
      std::align(HYPERBLOCK_SIZE, HYPERBLOCK_SIZE, addr, request));
  //
  //	Release any unused address space at the start of the mmap()ed block.
  //
  if (size_t front_slop = current_addr_ptr - start_block) {
    munmap(start_block, front_slop);
  }
  //
  //	Release any unused address space at the end of the mmap()ed block.
  //
  auto *end_aligned = current_addr_ptr + HYPERBLOCK_SIZE;
  if (size_t back_slop = end_block - end_aligned) {
    munmap(end_aligned, back_slop);
  }
  //
  //	We put a semispace id in the last byte of the hyperblock so we can identify which semispace an address
  //	belongs to by setting the low bits to 1 to access this id.
  //
  current_addr_ptr[HYPERBLOCK_SIZE - 1] = allocation_semispace_id;
  allocation_ptr = current_addr_ptr;
  //
  //	We set the tripwire for this space so we get trigger a garbage collection when we pass BLOCK_SIZE of memory
  //	allocated from this space.
  //
  tripwire = current_addr_ptr + BLOCK_SIZE;
  num_blocks = 1;
}
