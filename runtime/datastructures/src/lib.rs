pub mod decls;
pub mod hook_map;
pub mod hook_set;
pub mod hook_list;

#[cfg(not(test))]
use decls::KoreAllocator;

#[cfg(not(test))]
#[global_allocator]
static A: KoreAllocator = KoreAllocator;
