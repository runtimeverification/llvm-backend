#include <map>
//#include <iostream>

#include "runtime/collect.h"

#define UNW_LOCAL_ONLY
#include <libunwind.h>

extern "C" {
char *getStackMap();
extern char gc_stackmap_layoutinfo[];
extern unsigned int gc_stackmap_num_relocations;
}

std::map<void *, std::vector<gc_relocation>> StackMap;

struct StackMapFunction {
  uint64_t FunctionAddress;
  uint64_t NumRecords;
};

// see https://llvm.org/docs/StackMaps.html
void parseStackMap() {
  char *stackMap = getStackMap();
  // Header[0]
  char version = *stackMap;
  if (version != 3) {
    abort();
  }
  // NumFunctions
  uint32_t NumFunctions = *(uint32_t *)(stackMap + 4);
  // NumConstants
  uint32_t NumConstants = *(uint32_t *)(stackMap + 8);
  std::vector<StackMapFunction> functions;
  for (int i = 0; i < NumFunctions; i++) {
    // StkSizeRecord[i].Function Address
    uint64_t address = *(uint64_t *)(stackMap + 16 + i * 24);
    // StkSizeRecord[i].Record Count
    uint64_t records = *(uint64_t *)(stackMap + 32 + i * 24);
    functions.push_back({address, records});
  }
  // StkMapRecord[0]
  char *stackMapRecord = stackMap + 16 + NumFunctions * 24 + NumConstants * 8;
  for (StackMapFunction func : functions) {
    for (int i = 0; i < func.NumRecords; i++) {
      // StkMapRecord[i].PatchPoint ID
      uint64_t StatepointId = *(uint64_t *)(stackMapRecord);
      // StkMapRecord[i].Instruction Offset
      uint32_t InstructionOffset = *(uint32_t *)(stackMapRecord + 8);
      // StkMapRecord[i].NumLocations
      uint16_t NumLocations = *(uint16_t *)(stackMapRecord + 14);
      void *ip = (void *)(func.FunctionAddress + InstructionOffset);
      // third location record in a statepoint is always the number of Deopt
      // Locations StkMapRecord[i].Location[2].SmallConstant
      int32_t NumDeopts = *(int32_t *)(stackMapRecord + 24 + 2 * 12);
      uint16_t RelocationOffset = 0;
      // after the three constant Locations in a statepoint, and after any Deopt
      // Locations that exist, the remaining locations are in pairs and
      // represent relocations
      for (uint16_t j = 3 + NumDeopts; j < NumLocations; j += 2) {
        // StkMapRecord[i].Location[j].Type
        uint8_t base_type = *(uint8_t *)(stackMapRecord + 16 + j * 12);
        if (base_type == 5 /* ConstIndex */) {
          // a ConstIndex gc root is one which corresponds to something the
          // compiler was able to statically determine was a constructor with
          // zero children. Such terms do not actually live on the heap and
          // thus do not need to be relocated.
          RelocationOffset++;
          continue;
        }
        if (base_type != 3 /* Indirect */) {
          abort();
        }
        // StkMapRecord[i].Location[j+1].Type
        uint8_t derived_type = *(uint8_t *)(stackMapRecord + 16 + (j + 1) * 12);
        if (derived_type != base_type) {
          abort();
        }
        // StkMapRecord[i].Location[j].Offset
        int32_t base_offset = *(int32_t *)(stackMapRecord + 24 + j * 12);
        // StkMapRecord[i].Location[j+1].Offset
        int32_t derived_offset
            = *(int32_t *)(stackMapRecord + 24 + (j + 1) * 12);
        // StkMapRecord[i].Location[j].DwarfRegNum
        uint16_t base_dwarf_regnum
            = *(uint16_t *)(stackMapRecord + 20 + j * 12);
        // StkMapRecord[i].Location[j+1].DwarfRegNum
        uint16_t derived_dwarf_regnum
            = *(uint16_t *)(stackMapRecord + 20 + (j + 1) * 12);
        assert(base_dwarf_regnum == derived_dwarf_regnum);
        assert(base_dwarf_regnum == UNW_X86_64_RBP || base_dwarf_regnum == UNW_REG_SP);
        layoutitem layout;
        layout.offset = base_offset;
        size_t layout_offset
            = gc_stackmap_num_relocations * StatepointId + RelocationOffset;
        layout.cat = gc_stackmap_layoutinfo[layout_offset];
        gc_relocation reloc;
        reloc.base = layout;
        reloc.derived_offset = derived_offset;
        reloc.dwarf_regnum = base_dwarf_regnum;
        StackMap[ip].push_back(reloc);
//uint16_t base_location_size = *(uint16_t *)(stackMapRecord + 16 + j * 12 + 2);
//uint16_t derived_location_size = *(uint16_t *)(stackMapRecord + 16 + (j + 1) * 12 + 2);
//uint16_t base_dwarf_regnum = *(uint16_t *)(stackMapRecord + 16 + j * 12 + 4);
//uint16_t derived_dwarf_regnum = *(uint16_t *)(stackMapRecord + 16 + (j + 1) * 12 + 4);
//assert(base_location_size == 8);
//assert(derived_location_size == 8);
//assert(base_dwarf_regnum == 7);
//assert(derived_dwarf_regnum == 7);
//bool printline = false;
//if (base_location_size != derived_location_size) {
//std::cout << "Location sizes differ:\n";
//std::cout << "Base location size: "<< base_location_size << "\n";
//std::cout << "Derived location size: "<< derived_location_size << "\n";
//printline = true;
//}
//if (base_dwarf_regnum != derived_dwarf_regnum) {
//std::cout << "Dwarf registers differ:\n";
//std::cout << "Base DWARF RegNum: "<< base_dwarf_regnum << "\n";
//std::cout << "Derived DWARF RegNum: "<< derived_dwarf_regnum << "\n";
//printline = true;
//}
//if (base_dwarf_regnum != 7) {
//std::cout << "Base DWARF RegNum was not 7: "<< base_dwarf_regnum << "\n";
//printline = true;
//}
//if (derived_dwarf_regnum != 7) {
//std::cout << "Derived DWARF RegNum was not 7: "<< derived_dwarf_regnum << "\n";
//printline = true;
//}
//if (printline) {
//std::cout << "=============================================================\n";
//}
        RelocationOffset++;
      }
      // StkMapRecord[i].Location[NumLocations] (ie, end of Locations)
      stackMapRecord += 16 + NumLocations * 12;
      if (NumLocations % 2 == 1) {
        // StkMapRecord[i].Padding
        stackMapRecord += 4;
      }
      // StkMapRecord[i].NumLiveOuts
      uint16_t NumLiveOuts = *(uint16_t *)(stackMapRecord + 2);
      // StkMapRecord[i].LiveOuts[NumLiveOuts] (ie, end of LiveOuts)
      stackMapRecord += 4 + 4 * NumLiveOuts;
      if (NumLiveOuts % 2 == 0) {
        // StkMapRecord[i].Padding
        stackMapRecord += 4;
      }
      // stackMapRecord is now at StkMapRecord[i+1]
    }
  }
//assert(false);
}
