#include<stdint.h>
#include<stdbool.h>
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

static size_t get_size(block *block, uint16_t layout) {
  if (!layout) {
    return (block->h.hdr + sizeof(block) + 7) & ~7;
  } else {
    return ((block->h.hdr >> 32) & 0xff) * 8;
  }
}

static void migrate(block** blockPtr) {
  block* currBlock = *blockPtr;
  uintptr_t intptr = (uintptr_t)currBlock;
  if (intptr & 1) {
    return;
  }
  bool hasForwardingAddress = currBlock->h.hdr & (1LL << 47);
  uint16_t layout = currBlock->h.hdr >> 48;
  if (!layout) {
    bool isNotOnKoreHeap = currBlock->h.hdr & (1LL << 46);
    if (isNotOnKoreHeap) {
      return;
    }
  }
  size_t lenInBytes = get_size(currBlock, layout);
  if (!hasForwardingAddress) {
    block *newBlock = koreAlloc(lenInBytes);
    memcpy(newBlock, currBlock, lenInBytes);
    memcpy((block **)(currBlock+1), &newBlock, sizeof(block *));
    currBlock->h.hdr |= (1LL << 47);
  }
  *blockPtr = *(block **)(currBlock+1);
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
    memcpy(buffer->contents, &newBuffer, sizeof(stringbuffer *));
    buffer->contents->h.hdr |= (1LL << 47);
  }
  *bufferPtr = *(stringbuffer **)(buffer->contents);
}

static char* get_next(char* scan_ptr, size_t size) {
  char *next_ptr = scan_ptr + size;
  if (next_ptr < current_tospace_end) {
    if (arena_ptr() == current_tospace_start && next_ptr >= alloc_ptr()) {
      return 0;
    } else {
      return next_ptr;
    }
  }
  char *next_block;
  memcpy(&next_block, current_tospace_start, sizeof(char *));
  if (!next_block) {
    return 0;
  }
  current_tospace_start = next_block;
  current_tospace_end = next_block + BLOCK_SIZE;
  return current_tospace_start + sizeof(memory_block_header);
}

void koreCollect(block** root) {
  koreAllocSwap();
  migrate(root);
  current_tospace_start = fromspace_ptr();
  current_tospace_end = fromspace_ptr() + BLOCK_SIZE;
  char *scan_ptr = current_tospace_start + sizeof(memory_block_header);
  while(scan_ptr) {
    block *currBlock = (block *)scan_ptr;
    uint16_t layoutInt = currBlock->h.hdr >> 48;
    if (layoutInt) {
      layout *layoutData = getLayoutData(layoutInt);
      for (unsigned i = 0; i < layoutData->nargs; i++) {
        layoutitem *argData = layoutData->args + i;
        void *arg = ((char *)currBlock) + argData->offset;
        switch(argData->cat) {
        case 1: // map
          map_foreach(arg, migrate);
	  break;
        case 2: // list
          list_foreach(arg, migrate); 
	  break;
        case 3: // set
          set_foreach(arg, migrate);
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
    scan_ptr = get_next(scan_ptr, get_size(currBlock, layoutInt));
  }
}
