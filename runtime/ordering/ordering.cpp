#include <cstdint>
#include <cstring>
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
  extern int64_t hook_MAP_cmp(map*, map*);
  extern int64_t hook_LIST_cmp(list*, list*);
  extern int64_t hook_SET_cmp(set*, set*);
  extern int64_t hook_INT_cmp(mpz_t, mpz_t);
  extern int64_t hook_BUFFER_cmp(stringbuffer*, stringbuffer*);

constexpr unsigned long long HEADER_MASK = 0xffff1fffffffffffLL;


int64_t hook_KEQUAL_cmp(block *a, block *b){
     if (a == b) {
        return 0LL;
     }
     auto aptr = reinterpret_cast<int64_t>(a);
     auto bptr = reinterpret_cast<int64_t>(b);
     bool aleastbit = aptr & 1LL;
     bool bleastbit = bptr & 1LL;
     if(aleastbit != bleastbit) {
         return aleastbit - bleastbit;
     }
     if (aleastbit) {
         return memcmp(&a, &b, sizeof(a));
     }
     //auto alen = len(a);
     //auto blen = len(b);
     auto ahdr = a->h.hdr;
     auto bhdr = b->h.hdr;
     int64_t alen = ahdr & HEADER_MASK;
     int64_t blen = bhdr & HEADER_MASK;
     if (alen != blen) {
       return alen - blen;
     }
     int64_t layout = layout_hdr(ahdr);
     if (!layout) {
       return memcmp(((string *)a)->data, ((string *)b)->data, alen);
     }
     auto layoutData = getLayoutData(layout);
     for (unsigned i = 0; i < layoutData->nargs; ++i) {
       auto ArgData = layoutData->args[i];
       void *aArg = ((char *)a) + ArgData.offset;
       void *bArg = ((char *)b) + ArgData.offset;
       int64_t res;
       switch (ArgData.cat) {
            case MAP_LAYOUT: {
                 res = hook_MAP_cmp((map*)aArg, (map*)bArg);
                 break;
            }
            case SET_LAYOUT: {
                 res = hook_SET_cmp((set*)aArg, (set*)bArg);
                 break;
            }
            case LIST_LAYOUT: {
                 res = hook_LIST_cmp((list*)aArg, (list*)bArg);
                 break;
            }
            case STRINGBUFFER_LAYOUT: {
                 res = hook_BUFFER_cmp(*(stringbuffer**)aArg, *(stringbuffer**)bArg);
                 break;
            }
            case SYMBOL_LAYOUT: {
                 res = hook_KEQUAL_cmp(*(block**)aArg, *(block**)bArg);
                 break;
            }
            case INT_LAYOUT: {
                 res = hook_INT_cmp(*(mpz_ptr*)aArg, *(mpz_ptr*)bArg);
                 break;
            }
            case BOOL_LAYOUT: {
                 auto abool = *(bool*)aArg;
                 auto bbool = *(bool*)bArg;
                 res = abool - bbool;
                 break;
            }
            default: {
                 abort();
                 break;
            }
       }
       if (res != 0) {
         return res;
       }
     }
     return 0LL;
  }
}
