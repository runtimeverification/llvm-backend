#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "runtime/alloc.h"
#include "runtime/header.h"
#include "runtime/arena.h"

char **young_alloc_ptr(void);
char **old_alloc_ptr(void);
char* youngspace_ptr(void);
char* oldspace_ptr(void);
char youngspace_collection_id(void);
char oldspace_collection_id(void);
void map_foreach(void *, void(block**));
void set_foreach(void *, void(block**));
void list_foreach(void *, void(block**));

static bool is_gc = false;
static bool collect_old = false;
static uint8_t num_collection_only_young = 0;

bool during_gc() {
  return is_gc;
}

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
  bool isNotInYoungGen = hdr & NOT_YOUNG_OBJECT_BIT;
  bool hasAged = hdr & YOUNG_AGE_BIT;
  if (isNotInYoungGen && !(hasAged && collect_old)) {
    return;
  }
  bool shouldPromote = !isNotInYoungGen && hasAged;
  uint64_t mask = shouldPromote ? NOT_YOUNG_OBJECT_BIT : YOUNG_AGE_BIT;
  bool hasForwardingAddress = hdr & FWD_PTR_BIT;
  uint16_t layout = layout_hdr(hdr);
  size_t lenInBytes = get_size(hdr, layout);
  block** forwardingAddress = (block**)(currBlock + 1);
  if (!hasForwardingAddress) {
    block *newBlock;
    if (shouldPromote || (hasAged && collect_old)) {
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
  if (youngspace_collection_id() == getArenaSemispaceIDOfObject((void *)currBlock) ||
      oldspace_collection_id() == getArenaSemispaceIDOfObject((void *)currBlock)) {
    migrate(blockPtr);
  }
}

static void migrate_string_buffer(stringbuffer** bufferPtr) {
  stringbuffer* buffer = *bufferPtr;
  const uint64_t hdr = buffer->contents->h.hdr;
  bool isNotInYoungGen = hdr & NOT_YOUNG_OBJECT_BIT;
  bool hasAged = hdr & YOUNG_AGE_BIT;
  if (isNotInYoungGen && !(hasAged && collect_old)) {
    return;
  }
  bool shouldPromote = !isNotInYoungGen && hasAged;
  uint64_t mask = shouldPromote ? NOT_YOUNG_OBJECT_BIT : YOUNG_AGE_BIT;
  bool hasForwardingAddress = hdr & FWD_PTR_BIT;
  if (!hasForwardingAddress) {
    stringbuffer *newBuffer;
    string *newContents;
    if (shouldPromote || (hasAged && collect_old)) {
      newBuffer = koreAllocOld(sizeof(stringbuffer));
      newBuffer->capacity = buffer->capacity; // contents is written below
      newContents = koreAllocTokenOld(sizeof(string) + buffer->capacity);
    } else {
      newBuffer = koreAlloc(sizeof(stringbuffer));
      newBuffer->capacity = buffer->capacity; // contents is written below
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

static char* evacuate(char* scan_ptr, char **alloc_ptr) {
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
      case VARIABLE_LAYOUT:
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
  return movePtr(scan_ptr, get_size(hdr, layoutInt), *alloc_ptr);
}

// Contains the decision logic for collecting the old generation.
// For now, we collect the old generation every 50 young generation collections.
static bool shouldCollectOldGen() {
  if (++num_collection_only_young == 50) {
    num_collection_only_young = 0;
    return true;
  }

  return false;
}

void koreCollect(block** root) {
  is_gc = true;
  collect_old = shouldCollectOldGen();
  MEM_LOG("Starting garbage collection\n");
  koreAllocSwap(collect_old);
  migrate(root);
  char *scan_ptr = youngspace_ptr();
  MEM_LOG("Evacuating young generation\n");
  while(scan_ptr) {
    scan_ptr = evacuate(scan_ptr, young_alloc_ptr());
  }
  scan_ptr = oldspace_ptr();
  if (scan_ptr != *old_alloc_ptr()) {
    MEM_LOG("Evacuating old generation\n");
    while(scan_ptr) {
      scan_ptr = evacuate(scan_ptr, old_alloc_ptr());
    }
  }
  MEM_LOG("Finishing garbage collection\n");
  is_gc = false;
}
