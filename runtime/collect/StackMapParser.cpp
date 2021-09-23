#include <map>

#include "runtime/collect.h"

extern "C" {
  char *getStackMap();
  extern char gc_stackmap_layoutinfo[];
  extern unsigned int gc_stackmap_num_relocations;
}

std::map<void *, std::vector<layoutitem>> StackMap;

struct StackMapFunction {
  uint64_t FunctionAddress;
  uint64_t NumRecords;
};

//see https://llvm.org/docs/StackMaps.html
void parseStackMap() {
  char *stackMap = getStackMap();
  char version = *stackMap;
  if (version != 3) {
    abort();
  }
  uint32_t NumFunctions = *(uint32_t *)(stackMap+4);
  uint32_t NumConstants = *(uint32_t *)(stackMap+8);
  std::vector<StackMapFunction> functions;
  for (int i = 0; i < NumFunctions; i++) {
    uint64_t address = *(uint64_t *)(stackMap+16+i*24);
    uint64_t records = *(uint64_t *)(stackMap+32+i*24);
    functions.push_back({address, records});
  }
  char *stackMapRecord = stackMap+16+NumFunctions*24+NumConstants*8;
  for (StackMapFunction func : functions) {
    for (int i = 0; i < func.NumRecords; i++) {
      uint64_t StatepointId = *(uint64_t *)(stackMapRecord);
      uint32_t InstructionOffset = *(uint32_t *)(stackMapRecord+8);
      uint16_t NumLocations = *(uint16_t *)(stackMapRecord+14);
      void *ip = (void *)(func.FunctionAddress + InstructionOffset);
      int32_t NumDeopts = *(int32_t *)(stackMapRecord+24+2*12);
      uint16_t RelocationOffset = 0;
      for (uint16_t j = 4 + NumDeopts; j < NumLocations; j+= 2) {
        uint8_t type = *(uint8_t *)(stackMapRecord+16+j*12);
        if (type == 5) {
          // a ConstantOffset gc root is one which corresponds to something the
          // compiler was able to statically determine was a constructor with
          // zero children. Such terms do not actually live on the heap and
          // thus do not need to be relocated.
          RelocationOffset++;
          continue;
        }
        if (type != 3) {
          abort();
        }
        int32_t offset = *(int32_t *)(stackMapRecord+24+j*12);
        layoutitem layout;
        layout.offset = offset;
        size_t layout_offset = gc_stackmap_num_relocations*StatepointId + RelocationOffset;
        layout.cat = gc_stackmap_layoutinfo[layout_offset];
        StackMap[ip].push_back(layout);
        RelocationOffset++;
      }
      stackMapRecord += 16+NumLocations*12;
      if (NumLocations % 2 == 1) {
        stackMapRecord += 4;
      }
      uint16_t NumLiveOuts = *(uint16_t *)(stackMapRecord+2);
      stackMapRecord += 4 + 4*NumLiveOuts;
      if (NumLiveOuts % 2 == 0) {
        stackMapRecord += 4;
      }
    }
  }
}
