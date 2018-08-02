use super::decls::{Set,List,Int,K,__gmpz_init_set_ui};
use std::iter::FromIterator;
use std::ptr;
use std::mem;

#[no_mangle]
pub extern "C" fn size_set() -> usize {
  mem::size_of::<Set>()
}

#[no_mangle]
pub unsafe extern "C" fn drop_set(ptr: *mut Set) {
  ptr::drop_in_place(ptr)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_in(result: *mut bool, value: K, set: *const Set) -> bool {
  ptr::write(result, (*set).contains(&value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_unit(result: *mut Set) -> bool {
  ptr::write(result, Set::new());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_element(result: *mut Set, value: K) -> bool {
  ptr::write(result, Set::singleton(value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_concat(result: *mut Set, s1: *const Set, s2: *const Set) -> bool {
  ptr::write(result, (*s1).clone().union((*s2).clone()));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_difference(result: *mut Set, s1: *const Set, s2: *const Set) -> bool {
  ptr::write(result, (*s1).clone().difference((*s2).clone()));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_inclusion(result: *mut bool, s1: *const Set, s2: *const Set) -> bool {
  ptr::write(result, (*s1).is_subset(&*s2));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_intersection(result: *mut Set, s1: *const Set, s2: *const Set) -> bool {
  ptr::write(result, (*s1).clone().intersection((*s2).clone()));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_choice(result: *mut K, s: *const Set) -> bool {
  if (*s).is_empty() {
    return false;
  }
  ptr::write(result, *(*s).iter().next().unwrap());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_size(result: *mut Int, s: *const Set) -> bool {
  __gmpz_init_set_ui(result, (*s).len());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_set2list(result: *mut List, s: *const Set) -> bool {
  ptr::write(result, List::from_iter((*s).iter().map(|k| *k)));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_list2set(result: *mut Set, l: *const List) -> bool {
  ptr::write(result, Set::from_iter((*l).iter().map(|k| *k)));
  true
}

#[cfg(test)]
pub mod tests {
  extern crate libc;

  use decls::testing::*;
  use hook_set::*;
  use hook_list::tests::{alloc_list,free_list};

  pub unsafe fn alloc_set() -> *mut Set {
    let ptr = libc::malloc(size_set()) as *mut Set;
    ptr
  }

  pub unsafe fn free_set(ptr: *mut Set) {
    drop_set(ptr);
    libc::free(ptr as *mut libc::c_void);
  }

  #[test]
  fn test_element() {
    let mut contains = false;
    unsafe {
      let set = alloc_set();
      assert!(hook_SET_element(set, DUMMY0));
      let result = alloc_k();
      assert!(hook_SET_choice(result, set));
      assert_eq!(*result, DUMMY0);
      assert!(hook_SET_in(&mut contains, DUMMY0, set));
      assert!(contains);
      free_set(set); 
      free_k(result);
    }
  }

  #[test]
  fn test_unit() {
    unsafe {
      let set = alloc_set();
      assert!(hook_SET_unit(set));
      let result = alloc_int();
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_set(set);
      free_int(result);
    }
  }

  #[test]
  fn test_concat() {
    unsafe {
      let s1 = alloc_set();
      let s2 = alloc_set();
      let set = alloc_set();
      assert!(hook_SET_element(s1, DUMMY0));
      assert!(hook_SET_element(s2, DUMMY1));
      assert!(hook_SET_concat(set, s1, s2));
      let result = alloc_int();
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 2), 0);
      free_set(s1);
      free_set(s2);
      free_set(set);
      free_int(result);
    }
  }

  #[test]
  fn test_difference() {
    unsafe {
      let s1 = alloc_set();
      let s2 = alloc_set();
      let s3 = alloc_set();
      let set = alloc_set();
      assert!(hook_SET_element(s1, DUMMY0));
      assert!(hook_SET_element(s2, DUMMY0));
      assert!(hook_SET_element(s3, DUMMY1));
      assert!(hook_SET_difference(set, s1, s2));
      let result = alloc_int();
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      assert!(hook_SET_concat(s1, s1, s3));
      assert!(hook_SET_difference(set, s1, s2));
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_set(set);
      free_set(s1);
      free_set(s2);
      free_set(s3);
      free_int(result);
    }
  }

  #[test]
  fn test_inclusion() {
    let mut result = true;
    unsafe {
      let s1 = alloc_set();
      let s2 = alloc_set();
      assert!(hook_SET_element(s1, DUMMY0));
      assert!(hook_SET_element(s2, DUMMY1));
      assert!(hook_SET_inclusion(&mut result, s1, s2));
      assert!(!result);
      assert!(hook_SET_concat(s2, s2, s1));
      assert!(hook_SET_inclusion(&mut result, s1, s2));
      assert!(result);
      free_set(s1);
      free_set(s2);
    }
  }

  #[test]
  fn test_intersection() {
    unsafe {
      let s1 = alloc_set();
      let s2 = alloc_set();
      let s3 = alloc_set();
      let set = alloc_set();
      assert!(hook_SET_element(s1, DUMMY0));
      assert!(hook_SET_element(s2, DUMMY1));
      assert!(hook_SET_element(s3, DUMMY2));
      assert!(hook_SET_concat(s3, s3, s1));
      assert!(hook_SET_intersection(set, s1, s2));
      let result = alloc_int();
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      assert!(hook_SET_concat(s1, s1, s2));
      assert!(hook_SET_intersection(set, s1, s3));
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      assert!(hook_SET_intersection(set, s3, s1));
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      assert!(hook_SET_intersection(set, s2, s2));
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_set(s1);
      free_set(s2);
      free_set(s3);
      free_set(set);
      free_int(result);
    }
  }

  #[test]
  fn test_set2list() {
    unsafe {
      let set = alloc_set();
      let set2 = alloc_set();
      let list = alloc_list();
      assert!(hook_SET_element(set, DUMMY0));
      assert!(hook_SET_element(set2, DUMMY1));
      assert!(hook_SET_concat(set, set, set2));
      assert!(hook_SET_element(set2, DUMMY2));
      assert!(hook_SET_concat(set, set, set2));
      assert!(hook_SET_set2list(list, set));
      assert_eq!((*list).len(), 3);
      free_set(set);
      free_set(set2);
      free_list(list);
    }
  }

  #[test]
  fn test_list2set() {
    let mut contains = false;
    unsafe {
      let list = alloc_list();
      ptr::write(list, List::new());
      let set = alloc_set();
      (*list).push_back(DUMMY0);
      (*list).push_back(DUMMY1);
      (*list).push_back(DUMMY2);
      assert!(hook_SET_list2set(set, list));
      let result = alloc_int();
      assert!(hook_SET_size(result, set));
      assert_eq!(__gmpz_cmp_ui(result, 3), 0);
      assert!(hook_SET_in(&mut contains, DUMMY0, set));
      assert!(contains);
      contains = false;
      assert!(hook_SET_in(&mut contains, DUMMY1, set));
      assert!(contains);
      contains = false;
      assert!(hook_SET_in(&mut contains, DUMMY2, set));
      assert!(contains);
      free_set(set);
      free_list(list);
    }
  }
}
