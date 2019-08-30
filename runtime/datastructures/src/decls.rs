extern crate im_rc;
extern crate libc;

use std::cell::UnsafeCell;
use std::hash::{Hash,Hasher};
use self::libc::{c_void};
use std::alloc::{GlobalAlloc, Layout};
use std::collections::hash_map::DefaultHasher;

pub struct KoreAllocator;

unsafe impl GlobalAlloc for KoreAllocator {
  #[inline(always)]
  unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
    if during_gc() {
      malloc(_layout.size())
    } else {
      koreAllocNoGC(_layout.size())
    }
  }
  unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
    if during_gc() {
      free(_ptr);
    }
  }
}

pub enum Block {}

pub type K = *const Block;

#[repr(C)]
#[derive(Debug)]
pub struct KElem(
  pub UnsafeCell<K>
);

impl KElem {
  pub fn new(k: K) -> KElem {
    KElem(UnsafeCell::new(k))
  }
}

impl Hash for KElem {
  fn hash<H: Hasher>(&self, state: &mut H) {
    unsafe {
      if hash_enter() {
        k_hash(*self.0.get(), &mut (state as &mut Hasher) as *mut &mut Hasher as *mut c_void);
      }
      hash_exit();
    }
  }
}


#[no_mangle]
pub unsafe extern "C" fn add_hash8(h: *mut c_void, data: u8) {
  let hasher = h as *mut &mut Hasher;
  (*hasher).write_u8(data)
}
#[no_mangle]
pub unsafe extern "C" fn add_hash64(h: *mut c_void, data: u64) {
  let hasher = h as *mut &mut Hasher;
  (*hasher).write_u64(data)
}

#[no_mangle]
pub unsafe extern "C" fn hash_k(block: K) -> u64 {
  let mut h = DefaultHasher::new();
  KElem::new(block).hash(&mut h);
  h.finish()
}

extern "C" {
  pub fn hook_KEQUAL_eq(k1: K, k2: K) -> bool;
  pub fn k_hash<'a>(k1: K, h: *mut c_void) -> u64;
  pub fn hash_enter() -> bool;
  pub fn hash_exit();
  pub fn koreAllocNoGC(size: usize) -> *mut u8;
  pub fn during_gc() -> bool;
  pub fn malloc(size: usize) -> *mut u8;
  pub fn free(ptr: *mut u8);
}
