pub mod decls;

#[cfg(not(test))]
use decls::KoreAllocator;

#[cfg(not(test))]
#[global_allocator]
static A: KoreAllocator = KoreAllocator;
