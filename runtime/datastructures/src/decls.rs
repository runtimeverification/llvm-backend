extern crate im;
extern crate libc;

use std::cell::UnsafeCell;
use std::hash::{Hash,Hasher};
use decls::im::hashmap::HashMap;
use decls::im::hashset::HashSet;
use decls::im::vector::Vector;
use self::libc::{FILE,c_char,c_void};
use std::alloc::{GlobalAlloc, Layout};

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
#[allow(non_camel_case_types)]
pub enum mp_limb_t {}

#[repr(C)]
#[allow(non_camel_case_types)]
pub struct Int(
  pub i32, // _mp_alloc
  pub i32, // _mp_size
  pub *const mp_limb_t, // _mp_d
);

pub type K = *const Block;

#[derive(Debug)]
pub struct KElem(
  pub UnsafeCell<K>
);

impl Clone for KElem {
  fn clone(&self) -> KElem {
    unsafe { KElem(UnsafeCell::new(*self.0.get())) }
  }
}

impl KElem {
  pub fn new(k: K) -> KElem {
    KElem(UnsafeCell::new(k))
  }
}

impl PartialEq for KElem {
  fn eq(&self, other: &KElem) -> bool {
    unsafe { hook_KEQUAL_eq(*self.0.get(), *other.0.get()) }
  }
}

impl Eq for KElem {}

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

pub type Map = HashMap<KElem, KElem>;
pub type Set = HashSet<KElem>;
pub type List = Vector<KElem>;

#[link(name="gmp")]
extern "C" {
  pub fn __gmpz_init_set_ui(rop: *mut Int, op: usize);
  pub fn __gmpz_fits_ulong_p(op: *const Int) -> i32;
  pub fn __gmpz_fits_slong_p(op: *const Int) -> i32;
  pub fn __gmpz_get_ui(op: *const Int) -> usize;
  pub fn __gmpz_get_si(op: *const Int) -> isize;
}

extern "C" {
  pub fn move_int(result: *mut Int) -> *mut Int;
  pub fn printConfigurationInternal(file: *mut FILE, subject: *const Block, sort: *const c_char);
  pub fn hook_KEQUAL_eq(k1: K, k2: K) -> bool;
  pub fn k_hash<'a>(k1: K, h: *mut c_void) -> u64;
  pub fn hash_enter() -> bool;
  pub fn hash_exit();
  pub fn koreAllocNoGC(size: usize) -> *mut u8;
  pub fn during_gc() -> bool;
  pub fn malloc(size: usize) -> *mut u8;
  pub fn free(ptr: *mut u8);
}

#[cfg(test)]
pub mod testing {
  use super::{K,Int};
  use std::ptr;
  use std::collections::hash_map::DefaultHasher;
  use std::hash::{Hash,Hasher};

  #[link(name="gmp")]
  extern "C" {
    pub fn __gmpz_clear(rop: *mut Int);
    pub fn __gmpz_cmp_ui(op1: *const Int, op2: u64) -> i32;
    pub fn __gmpz_init_set_si(rop: *mut Int, op: i64);
  }

  #[repr(C)]
  pub struct DummyBlock {
    header: u64,
  }

  pub const DUMMY0: K = &DummyBlock{header: 0} as *const DummyBlock as K;
  pub const DUMMY1: K = &DummyBlock{header: 1} as *const DummyBlock as K;
  pub const DUMMY2: K = &DummyBlock{header: 2} as *const DummyBlock as K;

  pub fn alloc_k() -> *mut K {
    let b = Box::new(ptr::null());
    Box::into_raw(b)
  }

  pub unsafe fn free_k(ptr: *mut K) {
    Box::from_raw(ptr);
  }

  pub unsafe fn alloc_int() -> *mut Int {
    let b = Box::new(Int(0,0,ptr::null()));
    let res = Box::into_raw(b);
    res
  }

  #[no_mangle]
  pub unsafe extern "C" fn move_int(result: *mut Int) -> *mut Int {
    let ptr = alloc_int();
    let int = ptr::replace(result, Int(0, 0, ptr::null()));
    *ptr = int;
    ptr
  }

  #[no_mangle]
  pub unsafe extern "C" fn hook_KEQUAL_eq(k1: K, k2: K) -> bool {
    k1 == k2
  }

  #[no_mangle]
  pub unsafe extern "C" fn k_hash(k: K) -> u64 {
    let mut hasher = DefaultHasher::new();
    k.hash(&mut hasher);
    hasher.finish()
  }

  #[no_mangle]
  pub unsafe extern "C" fn hash_enter() -> bool {
    true
  }

  #[no_mangle]
  pub unsafe extern "C" fn hash_exit() {}

  pub unsafe fn free_int(ptr: *mut Int) {
    __gmpz_clear(ptr);
    Box::from_raw(ptr);
  }
}
