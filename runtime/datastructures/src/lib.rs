pub mod decls;
pub mod hook_map;
pub mod hook_set;

#[cfg(not(test))]
use decls::KoreAllocator;

#[cfg(not(test))]
#[global_allocator]
static A: KoreAllocator = KoreAllocator;
