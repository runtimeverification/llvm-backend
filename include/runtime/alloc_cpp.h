#ifndef ALLOC_CPP_H
#define ALLOC_CPP_H

/* This header file declares a string containing the textual bitcode of
 * runtime/lto/alloc.cpp. The cpp file that defines these symbols is generated
 * from that library by the lto library. The reason we do this is to ensure
 * that code generation can inline the bump allocator fully into the functions
 * that it is called from, which is quite important for performance.
 */
extern unsigned char alloc_cpp_o_ll[];
extern unsigned int alloc_cpp_o_ll_len;

#endif
