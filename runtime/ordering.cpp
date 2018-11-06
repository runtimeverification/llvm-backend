#include <cstdint>


extern "C" {


  struct blockheader {
    int64_t v;
  };

  struct block {
    blockheader hdr;
    int64_t data[0];
  };

  extern int64_t hook_MAP_cmp(void*, void*);
  extern int64_t hook_LIST_cmp(void*, void*);
  extern int64_t hook_SET_cmp(void*, void*);
  extern int64_t hook_INT_cmp(void*, void*);
  extern int64_t hook_STRING_cmp(void*, void*);

  static inline int64_t len(int64_t arg) {
     static const int64_t HDR_MASK = 0xffff1fffffffffff;
     return arg & HDR_MASK;
  }

  static inline int64_t ptr_compare(int64_t aptr, int64_t bptr){
    if (aptr < bptr) {
	   return -1LL;
	} else {
	   return 1LL;
	}
  }

  int64_t hook_KCMP_cmp(block *a, block *b){
     auto aptr = reinterpret_cast<int64_t>(a);
	 auto bptr = reinterpret_cast<int64_t>(b);
     auto aleastbit = aptr & 1LL;
     auto bleastbit = bptr & 1LL;
	 if(aleastbit != bleastbit) {
		 return ptr_compare(aptr, bptr);
	 }
	 if (aleastbit == 1LL) {
         return ptr_compare(aptr, bptr);
	 }
	 auto alen = len(a->hdr.v);
	 auto blen = len(b->hdr.v);
	 if(alen < blen) {
         return -1LL;
	 } else if (blen < alen) {
         return 1LL;
	 }
	 auto layout = a->hdr.v;
	 if((layout >> 48) == 0) {
         return hook_STRING_cmp(&(a->data[0]), &(b->data[0]));
	 }
	 //auto children = layout[1];
	 for(int64_t i = 0; i < alen; ++i) {
	 }
  }
}
