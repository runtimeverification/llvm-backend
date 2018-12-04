#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "runtime/header.h"

extern "C" {

  struct layoutitem {
    uint64_t offset;
    uint64_t cat;
  };

  struct layout {
    uint8_t nargs;
    layoutitem *args;
  };

  extern layout *getLayoutData(uint16_t);
  extern int64_t hook_MAP_cmp(void*, void*);
  extern int64_t hook_LIST_cmp(void*, void*);
  extern int64_t hook_SET_cmp(void*, void*);
  extern int64_t hook_INT_cmp(void*, void*);
  extern int64_t hook_FLOAT_cmp(void*, void*);
  extern int64_t hook_STRING_cmp(void*, void*);
  extern int64_t hook_BUFFER_cmp(void*, void*);

  // ptr_compare compares entirely based on ptr address.
  // used to compare constants.
  static inline int64_t ptr_compare(int64_t aptr, int64_t bptr){
    if (aptr < bptr) {
       return -1LL;
    } else if (aptr > bptr) {
       return 1LL;
    }
    return 0LL;
  }

constexpr unsigned long long HEADER_MASK = 0xffff1fffffffffffLL;


int64_t hook_KEQUAL_cmp(block *a, block *b){
    std::cout << "CMP " << a << "  " << b << std::endl;
     if (a == b) {
        std::cout << "EQUAL PTRS" << std::endl;
        return 0LL;
     }
     auto aptr = reinterpret_cast<int64_t>(a);
     auto bptr = reinterpret_cast<int64_t>(b);
     auto aleastbit = aptr & 1LL;
     auto bleastbit = bptr & 1LL;
     if(aleastbit != bleastbit) {
         std::cout << "PTR CMP" << std::endl;
         return ptr_compare(aptr, bptr);
     }
     if (aleastbit == 1LL) {
         std::cout << "PTR CMP 2" << std::endl;
         return ptr_compare(aptr, bptr);
     }
     //auto alen = len(a);
     //auto blen = len(b);
     auto ahdr = a->h.hdr;
     auto bhdr = b->h.hdr;
     auto alen = ahdr & HEADER_MASK;
     auto blen = bhdr & HEADER_MASK;
     std::cout << "ALEN: " << alen << " -- BLEN: " << blen << std::endl;
     if(alen < blen) {
         std::cout << "LEN LT " << alen << " -- " << blen << std::endl;
         return -1LL;
     } else if (blen < alen) {
         std::cout << "LEN GT" << std::endl;
         return 1LL;
     }
     uint64_t alayoutInt = layout_hdr(ahdr);
     uint64_t blayoutInt = layout_hdr(bhdr);
     if (!alayoutInt) {
         std::cout << "NOT AINT" << std::endl;
         return -1LL;
     }
     if (!blayoutInt) {
         std::cout << "NOT BINT" << std::endl;
         return 1LL;
     }
     auto alayoutData = getLayoutData(alayoutInt);
     auto blayoutData = getLayoutData(blayoutInt);
     if (alayoutData->nargs < blayoutData->nargs) {
         std::cout << "AARGS LT" << std::endl;
        return -1LL;
     }
     if (alayoutData->nargs > blayoutData->nargs) {
         std::cout << "AARGS GT" << std::endl;
        return 1LL;
     }
     std::cout << "$$$ ARG COUNT: " << (int) alayoutData->nargs << std::endl;
     for (unsigned i = 0; i < alayoutData->nargs; ++i) {
       auto aArgData = alayoutData->args[i];
       auto bArgData = blayoutData->args[i];
       void *aArg = &(((char *)a)[aArgData.offset]);
       void *bArg = &(((char *)b)[bArgData.offset]);
       // if any children types are different, we order
       // based on child type (number), which should be consist
       // in any given run.
       if (aArgData.cat < bArgData.cat) {
           std::cout << "ARG: " << i << " CAT LT" << std::endl;
           return -1LL;
       }
       if (aArgData.cat > bArgData.cat) {
           std::cout << "ARG: " << i << " CAT GT" << std::endl;
           return 1LL;
       }
       // Here the types must be the same, so we switch on
       // type category to decide which other cmp procedure to use.
       switch (aArgData.cat) {
            case MAP_LAYOUT: {
                 auto res = hook_MAP_cmp(aArg, bArg);
                     std::cout << "MAP: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case SET_LAYOUT: {
                 auto res = hook_SET_cmp(aArg, bArg);
                     std::cout << "SET: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case LIST_LAYOUT: {
                 auto res = hook_LIST_cmp(aArg, bArg);
                     std::cout << "LIST: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case STRINGBUFFER_LAYOUT: {
                 auto res = hook_BUFFER_cmp(aArg, bArg);
                     std::cout << "SBUFF: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case SYMBOL_LAYOUT: {
                 std::cout << "SYMBOL" << std::endl;
                 auto res = hook_KEQUAL_cmp(static_cast<block *>(aArg),
                                            static_cast<block *>(bArg));
                     std::cout << "SYMBOL: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case INT_LAYOUT: {
                 auto res = hook_INT_cmp(aArg, bArg);
                     std::cout << "INT: " << i << " RES: " << res << std::endl;
                 if (res != 0) {
                     return res;
                 }
                 break;
            }
            case FLOAT_LAYOUT: {
                 abort();
                 break;
            }
            case BOOL_LAYOUT: {
                 auto abool = *reinterpret_cast<bool*>(aArg);
                 auto bbool = *reinterpret_cast<bool*>(bArg);
                 if (abool != bbool) {
                     return (abool)?1:-1;
                 }
            }
            // MINT
            default: {
                 auto aint = *reinterpret_cast<int64_t*>(aArg);
                 auto bint = *reinterpret_cast<int64_t*>(bArg);
                 if (aint < bint) {
                     return -1LL;
                 }
                 if (aint > bint) {
                     return 1LL;
                 }
            }
       }
     }
     return 0LL;
  }
}
