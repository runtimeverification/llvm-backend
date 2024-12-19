#ifndef OPAQUE_CPP_H
#define OPAQUE_CPP_H

/* This header file declares a string containing the textual bitcode of
 * runtime/opaque/opaque.ll. The reason we use a separate IR file for the
 * functions in opaque.ll is that the instructions they include confuse the
 * RewriteStatepointsForGC pass. Therefore, we first perform all the regular
 * optimization passes, then link, and then run the inline pass again.
 */
extern unsigned char opaque_ll[];
extern unsigned int opaque_ll_len;

#endif // define OPAQUE_CPP_H
