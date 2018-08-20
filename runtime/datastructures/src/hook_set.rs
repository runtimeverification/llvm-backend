use super::decls::{Set,List,Int,K,__gmpz_init_set_ui,move_int};
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
pub unsafe extern "C" fn hook_SET_in(value: K, set: *const Set) -> bool {
  (*set).contains(&value)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_unit() -> Set {
  Set::new()
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_element(value: K) -> Set {
  Set::singleton(value)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_concat(s1: *const Set, s2: *const Set) -> Set {
  (*s1).clone().union((*s2).clone())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_difference(s1: *const Set, s2: *const Set) -> Set {
  (*s1).clone().difference((*s2).clone())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_inclusion(s1: *const Set, s2: *const Set) -> bool {
  (*s1).is_subset(&*s2)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_intersection(s1: *const Set, s2: *const Set) -> Set {
  (*s1).clone().intersection((*s2).clone())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_choice(s: *const Set) -> K {
  if (*s).is_empty() {
    panic!("Set is empty")
  }
  *(*s).iter().next().unwrap()
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_size(s: *const Set) -> *mut Int {
  let mut result = Int(0, 0, ptr::null());
  __gmpz_init_set_ui(&mut result, (*s).len());
  move_int(result)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_set2list(s: *const Set) -> List {
  List::from_iter((*s).iter().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_list2set(l: *const List) -> Set {
  Set::from_iter((*l).iter().cloned())
}

#[cfg(test)]
pub mod tests {
  extern crate libc;

  use decls::testing::*;
  use hook_set::*;

  #[test]
  fn test_element() {
    unsafe {
      let set = hook_SET_element(DUMMY0);
      let result = hook_SET_choice(&set);
      assert_eq!(result, DUMMY0);
      let contains = hook_SET_in(DUMMY0, &set);
      assert!(contains);
    }
  }

  #[test]
  fn test_unit() {
    unsafe {
      let set = hook_SET_unit();
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_concat() {
    unsafe {
      let s1 = hook_SET_element(DUMMY0);
      let s2 = hook_SET_element(DUMMY1);
      let set = hook_SET_concat(&s1, &s2);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 2), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_difference() {
    unsafe {
      let s1 = hook_SET_element(DUMMY0);
      let s2 = hook_SET_element(DUMMY0);
      let s3 = hook_SET_element(DUMMY1);
      let set = hook_SET_difference(&s1, &s2);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      let s1 = hook_SET_concat(&s1, &s3);
      let set = hook_SET_difference(&s1, &s2);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_inclusion() {
    unsafe {
      let s1 = hook_SET_element(DUMMY0);
      let s2 = hook_SET_element(DUMMY1);
      let result = hook_SET_inclusion(&s1, &s2);
      assert!(!result);
      let s2 = hook_SET_concat(&s2, &s1);
      let result = hook_SET_inclusion(&s1, &s2);
      assert!(result);
    }
  }

  #[test]
  fn test_intersection() {
    unsafe {
      let s1 = hook_SET_element(DUMMY0);
      let s2 = hook_SET_element(DUMMY1);
      let s3 = hook_SET_element(DUMMY2);
      let s3 = hook_SET_concat(&s3, &s1);
      let set = hook_SET_intersection(&s1, &s2);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      let s1 = hook_SET_concat(&s1, &s2);
      let set = hook_SET_intersection(&s1, &s3);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      let set = hook_SET_intersection(&s3, &s1);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      let set = hook_SET_intersection(&s2, &s2);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_set2list() {
    unsafe {
      let set = hook_SET_element(DUMMY0);
      let set2 = hook_SET_element(DUMMY1);
      let set = hook_SET_concat(&set, &set2);
      let set2 = hook_SET_element(DUMMY2);
      let set = hook_SET_concat(&set, &set2);
      let list = hook_SET_set2list(&set);
      assert_eq!((list).len(), 3);
    }
  }

  #[test]
  fn test_list2set() {
    unsafe {
      let mut list = List::new();
      (list).push_back(DUMMY0);
      (list).push_back(DUMMY1);
      (list).push_back(DUMMY2);
      let set = hook_SET_list2set(&list);
      let result = hook_SET_size(&set);
      assert_eq!(__gmpz_cmp_ui(result, 3), 0);
      let contains = hook_SET_in(DUMMY0, &set);
      assert!(contains);
      let contains = hook_SET_in(DUMMY1, &set);
      assert!(contains);
      let contains = hook_SET_in(DUMMY2, &set);
      assert!(contains);
    }
  }
}
