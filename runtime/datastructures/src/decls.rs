extern crate im_rc;
extern crate libc;

use std::cell::UnsafeCell;
use std::collections::BinaryHeap;
use std::cmp::Ordering;
use std::hash::{Hash,Hasher};
use decls::im_rc::hashmap::HashMap;
use decls::im_rc::ordmap::OrdMap;
use decls::im_rc::hashset::HashSet;
use decls::im_rc::ordset::OrdSet;
use decls::im_rc::vector::Vector;
use self::libc::{FILE,c_char,c_void};
use std::alloc::{GlobalAlloc, Layout};

pub struct KoreAllocator;

unsafe impl GlobalAlloc for KoreAllocator {
  #[inline(always)]
  unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
    koreAllocOld(_layout.size())
  }
  unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {}
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

// This helper makes it simpler to switch between Ord/Hash in the hook.
pub fn ord_map_compare<K,V>(a: &OrdMap<K, V>, b: &OrdMap<K, V>) -> Ordering
where K: Ord + Clone, V: Ord + Clone {
    a.cmp(b)
}

pub fn hash_map_compare<K,V>(a: &HashMap<K, V>, b: &HashMap<K, V>) -> Ordering
where K: Ord + Hash + Clone, V: Ord + Clone {
    let mut a_keys : BinaryHeap<_> = a.keys().collect();
    let mut b_keys : BinaryHeap<_> = b.keys().collect();
    while !(a_keys.is_empty() || b_keys.is_empty()) {
        let (a_top, b_top) = (a_keys.pop(), b_keys.pop());
        if a_top > b_top {
            return Ordering::Greater;
        }
        if b_top > a_top {
            return Ordering::Less;
        }
        let (a_top_elem, b_top_elem) = (a.get(a_top.unwrap()), b.get(b_top.unwrap()));
        if a_top_elem > b_top_elem {
            return Ordering::Greater;
        }
        if b_top_elem > a_top_elem {
            return Ordering::Less;
        }
    }
    if a_keys.is_empty() {
        if b_keys.is_empty() {
            return Ordering::Equal;
        }
        return Ordering::Less;
    }
    return Ordering::Greater;
}

// This helper makes it simpler to switch between Ord/Hash in the hook.
pub fn ord_set_compare<K>(a: &OrdSet<K>, b: &OrdSet<K>) -> Ordering
where K: Ord + Clone {
    a.cmp(b)
}

pub fn hash_set_compare<K>(a: &HashSet<K>, b: &HashSet<K>) -> Ordering
where K: Ord + Hash + Clone {
    let mut a_vals : BinaryHeap<_> = a.iter().collect();
    let mut b_vals : BinaryHeap<_> = b.iter().collect();
    while !(a_vals.is_empty() || b_vals.is_empty()) {
        let (a_top, b_top) = (a_vals.pop(), b_vals.pop());
        if a_top > b_top {
            return Ordering::Greater;
        }
        if b_top > a_top {
            return Ordering::Less;
        }
    }
    if a_vals.is_empty() {
        if b_vals.is_empty() {
            return Ordering::Equal;
        }
        return Ordering::Less;
    }
    return Ordering::Greater;
}


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

impl PartialOrd for KElem {
    fn partial_cmp(&self, other: &KElem) -> Option<Ordering> {
        let ret: i64 = unsafe {
            hook_KEQUAL_cmp(*self.0.get(), *other.0.get())
        };
        if ret < 0 {
            Some(Ordering::Less)
        } else if ret == 0 {
            Some(Ordering::Equal)
        } else {
            Some(Ordering::Greater)
        }
    }
}

impl Ord for KElem {
    fn cmp(&self, other: &KElem) -> Ordering {
        self.partial_cmp(other).unwrap()
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

pub type Map = OrdMap<KElem, KElem>;
pub type Set = OrdSet<KElem>;
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
  pub fn hook_KEQUAL_cmp(k1: K, k2: K) -> i64;
  pub fn k_hash<'a>(k1: K, h: *mut c_void) -> u64;
  pub fn hash_enter() -> bool;
  pub fn hash_exit();
  pub fn koreAllocOld(size: usize) -> *mut u8;
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
  pub const DUMMY3: K = &DummyBlock{header: 3} as *const DummyBlock as K;
  pub const DUMMY4: K = &DummyBlock{header: 4} as *const DummyBlock as K;

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

  // Dummy hook_KEQUAL_cmp for cargo test use.
  #[no_mangle]
  pub unsafe extern "C" fn hook_KEQUAL_cmp(k1: K, k2: K) -> i64 {
     let kd1 = std::mem::transmute::<K, *const DummyBlock>(k1);
     let kd2 = std::mem::transmute::<K, *const DummyBlock>(k2);
     if (*kd1).header < (*kd2).header {
        return -1;
     } else if (*kd1).header > (*kd2).header {
        return 1;
     }
     0
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
