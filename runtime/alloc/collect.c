#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "runtime/alloc.h"

extern const size_t BLOCK_SIZE;
extern bool true_is_fromspace;

static char* current_tospace_start = 0;
static char* current_tospace_end = 0;

typedef struct {
  uint64_t offset;
  uint16_t cat;
} layoutitem;

typedef struct {
  uint8_t nargs;
  layoutitem *args;
} layout;

typedef struct {
  uint64_t hdr;
} blockheader;

typedef struct block {
  blockheader h;
  struct block* children[];
} block;

typedef struct {
  uint64_t capacity;
  block *contents;
} stringbuffer;

char *alloc_ptr(void);
char *arena_ptr(void);
char* fromspace_ptr(void);
layout *getLayoutData(uint16_t);
void map_foreach(void *, void(block**));
void set_foreach(void *, void(block**));
void list_foreach(void *, void(block**));

static size_t get_size(uint64_t hdr, uint16_t layout) {
  if (!layout) {
    size_t size = (hdr + sizeof(block) + 7) & ~7;
    return hdr & 0x400000000000LL ? 8 : size < 16 ? 16 : size;
  } else {
    return ((hdr >> 32) & 0xff) * 8;
  }
}

static void migrate(block** blockPtr) {
  block* currBlock = *blockPtr;
  uintptr_t intptr = (uintptr_t)currBlock;
  if (intptr & 1) {
    return;
  }
  const uint64_t hdr = currBlock->h.hdr;
  bool isNotOnKoreHeap = hdr & (1LL << 46);
  if (isNotOnKoreHeap) {
    return;
  }
  bool hasForwardingAddress = hdr & (1LL << 47);
  uint16_t layout = hdr >> 48;
  size_t lenInBytes = get_size(hdr, layout);
  block** forwardingAddress = (block**)(currBlock + 1);
  if (!hasForwardingAddress) {
    block *newBlock = koreAlloc(lenInBytes);
    memcpy(newBlock, currBlock, lenInBytes);
    *forwardingAddress = newBlock;
    currBlock->h.hdr |= (1LL << 47);
    *blockPtr = newBlock;
  } else {
    *blockPtr = *forwardingAddress;
  }
}

static void migrate_once(block** blockPtr) {
  block* currBlock = *blockPtr;
  uintptr_t intptr = (uintptr_t) currBlock;
  intptr = intptr & ~(BLOCK_SIZE-1);
  memory_block_header *hdr = (memory_block_header *)intptr;
  // bit has been flipped by now, so we need != and not ==
  if (true_is_fromspace != hdr->semispace) {
    migrate(blockPtr);
  }
}

static void migrate_string_buffer(stringbuffer** bufferPtr) {
  stringbuffer* buffer = *bufferPtr;
  bool hasForwardingAddress = buffer->contents->h.hdr & (1LL << 47);
  if (!hasForwardingAddress) {
    stringbuffer *newBuffer = koreAlloc(sizeof(stringbuffer));
    memcpy(newBuffer, buffer, sizeof(stringbuffer));
    block *newContents = koreAllocToken(sizeof(block) + buffer->capacity);
    memcpy(newContents, buffer->contents, buffer->contents->h.hdr);
    newBuffer->contents = newContents;
    *(stringbuffer **)(buffer->contents) = newBuffer;
    buffer->contents->h.hdr |= (1LL << 47);
  }
  *bufferPtr = *(stringbuffer **)(buffer->contents);
}

static char* get_next(char* scan_ptr, size_t size) {
  char *next_ptr = scan_ptr + size;
  if (next_ptr == alloc_ptr()) {
    return 0;
  }
  if (next_ptr != current_tospace_end) {
    return next_ptr;
  }
  char *next_block = *(char **)current_tospace_start;
  if (!next_block) {
    return 0;
  }
  current_tospace_start = next_block;
  current_tospace_end = next_block + BLOCK_SIZE;
  return current_tospace_start + sizeof(memory_block_header);
}

void koreCollect(block** root) {
  DBG("Starting garbage collection\n");
  koreAllocSwap();
  migrate(root);
  current_tospace_start = fromspace_ptr();
  current_tospace_end = fromspace_ptr() + BLOCK_SIZE;
  char *scan_ptr = current_tospace_start + sizeof(memory_block_header);
  while(scan_ptr) {
    block *currBlock = (block *)scan_ptr;
    const uint64_t hdr = currBlock->h.hdr;
    uint16_t layoutInt = hdr >> 48;
    if (layoutInt) {
      layout *layoutData = getLayoutData(layoutInt);
      for (unsigned i = 0; i < layoutData->nargs; i++) {
        layoutitem *argData = layoutData->args + i;
        void *arg = ((char *)currBlock) + argData->offset;
        switch(argData->cat) {
        case 1: // map
          map_foreach(arg, migrate_once);
	  break;
        case 2: // list
          list_foreach(arg, migrate_once); 
	  break;
        case 3: // set
          set_foreach(arg, migrate_once);
	  break;
        case 6:  // stringbuffer
          migrate_string_buffer(arg);
          break;
        case 8:  // block
          migrate(arg);
          break;
        case 4: //int
        case 5: //float
        case 7: // bool
        default: //mint
          break;
        }
      }
    }
    scan_ptr = get_next(scan_ptr, get_size(hdr, layoutInt));
  }
  DBG("Finishing garbage collection\n");
}
