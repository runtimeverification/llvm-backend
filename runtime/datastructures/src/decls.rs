extern crate im;

use decls::im::hashmap::HashMap;
use decls::im::hashset::HashSet;
use decls::im::vector::Vector;

pub enum Block {}
#[allow(non_camel_case_types)]
pub enum mp_limb_t {}

#[repr(C)]
#[allow(non_camel_case_types)]
pub struct Int(
  i32, // _mp_alloc
  i32, // _mp_size
  *const mp_limb_t, // _mp_d
);

pub type K = *const Block;
pub type Map = HashMap<K, K>;
pub type Set = HashSet<K>;
pub type List = Vector<K>;

#[link(name="gmp")]
extern "C" {
  pub fn __gmpz_set_ui(rop: *mut Int, op: usize);
}

#[cfg(test)]
pub mod testing {
  use super::{K,Int};
  use std::ptr;

  #[link(name="gmp")]
  extern "C" {
    pub fn __gmpz_init(rop: *mut Int);
    pub fn __gmpz_clear(rop: *mut Int);
    pub fn __gmpz_cmp_ui(op1: *const Int, op2: i64) -> i32;
  }

  #[repr(C)]
  pub struct DummyBlock {
    header: i64,
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
    __gmpz_init(res);
    res
  }

  pub unsafe fn free_int(ptr: *mut Int) {
    __gmpz_clear(ptr);
    Box::from_raw(ptr);
  }
}
