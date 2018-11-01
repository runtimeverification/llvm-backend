#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "runtime/alloc.h"
#include "runtime/header.h"

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

char **alloc_ptr(void);
char **old_alloc_ptr(void);
char* fromspace_ptr(void);
char* oldspace_ptr(void);
char fromspace_id(void);
layout *getLayoutData(uint16_t);
void map_foreach(void *, void(block**));
void set_foreach(void *, void(block**));
void list_foreach(void *, void(block**));

static size_t get_size(uint64_t hdr, uint16_t layout) {
  if (!layout) {
    size_t size = (len_hdr(hdr)  + sizeof(block) + 7) & ~7;
    return hdr == NOT_YOUNG_OBJECT_BIT ? 8 : size < 16 ? 16 : size;
  } else {
    return size_hdr(hdr);
  }
}

static void migrate(block** blockPtr) {
  block* currBlock = *blockPtr;
  uintptr_t intptr = (uintptr_t)currBlock;
  if (intptr & 1) {
    return;
  }
  const uint64_t hdr = currBlock->h.hdr;
  bool isNotOnKoreHeap = hdr & NOT_YOUNG_OBJECT_BIT;
  if (isNotOnKoreHeap) {
    return;
  }
  bool shouldPromote = hdr & YOUNG_AGE_BIT;
  uint64_t mask = shouldPromote ? NOT_YOUNG_OBJECT_BIT : YOUNG_AGE_BIT;
  bool hasForwardingAddress = hdr & FWD_PTR_BIT;
  uint16_t layout = layout_hdr(hdr);
  size_t lenInBytes = get_size(hdr, layout);
  block** forwardingAddress = (block**)(currBlock + 1);
  if (!hasForwardingAddress) {
    block *newBlock;
    if (shouldPromote) {
      newBlock = koreAllocOld(lenInBytes);
    } else {
      newBlock = koreAlloc(lenInBytes);
    }
    memcpy(newBlock, currBlock, lenInBytes);
    newBlock->h.hdr |= mask;
    *forwardingAddress = newBlock;
    currBlock->h.hdr |= FWD_PTR_BIT;
    *blockPtr = newBlock;
  } else {
    *blockPtr = *forwardingAddress;
  }
}

// call this function instead of migrate on objects directly referenced by shared objects (like collection nodes)
// that are not tracked by gc
static void migrate_once(block** blockPtr) {
  block* currBlock = *blockPtr;
  memory_block_header *hdr = mem_block_header(currBlock);
  if (fromspace_id() == hdr->semispace) {
    migrate(blockPtr);
  }
}

static void migrate_string_buffer(stringbuffer** bufferPtr) {
  stringbuffer* buffer = *bufferPtr;
  bool shouldPromote = buffer->contents->h.hdr & YOUNG_AGE_BIT;
  uint64_t mask = shouldPromote ? NOT_YOUNG_OBJECT_BIT : YOUNG_AGE_BIT;
  bool hasForwardingAddress = buffer->contents->h.hdr & FWD_PTR_BIT;
  if (!hasForwardingAddress) {
    stringbuffer *newBuffer;
    string *newContents;
    if (shouldPromote) {
      newBuffer = koreAllocOld(sizeof(stringbuffer));
      memcpy(newBuffer, buffer, sizeof(uint64_t)); // contents is written below
      newContents = koreAllocTokenOld(sizeof(string) + buffer->capacity);
    } else {
      newBuffer = koreAlloc(sizeof(stringbuffer));
      memcpy(newBuffer, buffer, sizeof(uint64_t)); // contents is written below
      newContents = koreAllocToken(sizeof(string) + buffer->capacity);
    }
    memcpy(newContents, buffer->contents, len(buffer->contents));
    newContents->h.hdr |= mask;
    newBuffer->contents = newContents;
    *(stringbuffer **)(buffer->contents) = newBuffer;
    buffer->contents->h.hdr |= FWD_PTR_BIT;
  }
  *bufferPtr = *(stringbuffer **)(buffer->contents);
}

static char* get_next(char* scan_ptr, size_t size, char* alloc_ptr) {
  char *next_ptr = scan_ptr + size;
  if (next_ptr == alloc_ptr) {
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

static char* evacuate(char* scan_ptr, char** alloc_ptr) {
  block *currBlock = (block *)scan_ptr;
  const uint64_t hdr = currBlock->h.hdr;
  uint16_t layoutInt = layout_hdr(hdr);
  if (layoutInt) {
    layout *layoutData = getLayoutData(layoutInt);
    for (unsigned i = 0; i < layoutData->nargs; i++) {
      layoutitem *argData = layoutData->args + i;
      void *arg = ((char *)currBlock) + argData->offset;
      switch(argData->cat) {
      case MAP_LAYOUT:
        map_foreach(arg, migrate_once);
	break;
      case LIST_LAYOUT:
        list_foreach(arg, migrate_once); 
	break;
      case SET_LAYOUT:
        set_foreach(arg, migrate_once);
	break;
      case STRINGBUFFER_LAYOUT:
        migrate_string_buffer(arg);
        break;
      case SYMBOL_LAYOUT:
        migrate(arg);
        break;
      case INT_LAYOUT:
      case FLOAT_LAYOUT:
      case BOOL_LAYOUT:
      default: //mint
        break;
      }
    }
  }
  return get_next(scan_ptr, get_size(hdr, layoutInt), *alloc_ptr);
}

void koreCollect(block** root) {
  MEM_LOG("Starting garbage collection\n");
  koreAllocSwap();
  char* oldspace_start = *old_alloc_ptr();
  migrate(root);
  current_tospace_start = fromspace_ptr();
  current_tospace_end = fromspace_ptr() + BLOCK_SIZE;
  char *scan_ptr = current_tospace_start + sizeof(memory_block_header);
  MEM_LOG("Evacuating young generation\n");
  while(scan_ptr) {
    scan_ptr = evacuate(scan_ptr, alloc_ptr());
  }
  scan_ptr = oldspace_start == 0 ? oldspace_ptr() == 0 ? 0 : oldspace_ptr() + sizeof(memory_block_header) : oldspace_start;
  uintptr_t oldspace_block_start = (uintptr_t)scan_ptr;
  if (!(oldspace_block_start & (BLOCK_SIZE-1))) {
    // this happens when oldspace_start is at the exact end of a block
    current_tospace_end = oldspace_start;
    current_tospace_start = oldspace_start - BLOCK_SIZE;
    scan_ptr = get_next(scan_ptr, 0, *old_alloc_ptr());
  } else {
    oldspace_block_start = oldspace_block_start & ~(BLOCK_SIZE-1);
    current_tospace_start = (char*) oldspace_block_start;
    current_tospace_end = current_tospace_start + BLOCK_SIZE;
  }
  if (scan_ptr != *old_alloc_ptr()) {
    MEM_LOG("Evacuating promoted objects\n");
    while(scan_ptr) {
      scan_ptr = evacuate(scan_ptr, old_alloc_ptr());
    }
  }
  MEM_LOG("Finishing garbage collection\n");
}
