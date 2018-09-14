extern crate libc;

use super::decls::{Set,List,Int,K,KElem,__gmpz_init_set_ui,move_int,printConfigurationInternal};
use std::iter::FromIterator;
use std::hash::Hash;
use std::collections::hash_map::DefaultHasher;
use std::ptr;
use std::mem;
use std::ffi::CString;
use self::libc::{FILE,c_char,c_void,fprintf};

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
  (*set).contains(&KElem(value))
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_unit() -> Set {
  Set::new()
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_element(value: K) -> Set {
  Set::singleton(KElem(value))
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
  (*s).iter().next().unwrap().0
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_size(s: *const Set) -> *mut Int {
  let mut result = Int(0, 0, ptr::null());
  __gmpz_init_set_ui(&mut result, (*s).len());
  move_int(&result)
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_set2list(s: *const Set) -> List {
  List::from_iter((*s).iter().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_list2set(l: *const List) -> Set {
  Set::from_iter((*l).iter().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_SET_eq(s1: *const Set, s2: *const Set) -> bool {
  *s1 == *s2
}

#[no_mangle]
pub unsafe extern "C" fn set_hash(s: *const Set, h: *mut c_void) {
  let hasher = h as *mut &mut DefaultHasher;
  s.hash(*hasher)
}

#[no_mangle]
pub unsafe extern "C" fn printSet(file: *mut FILE, set: *const Set, unit: *const c_char, element: *const c_char, concat: *const c_char) {
  if (*set).len() == 0 {
    let fmt = CString::new("%s()").unwrap();
    fprintf(file, fmt.as_ptr(), unit);
    return;
  }
  let mut i = 1;
  let parens = CString::new(")").unwrap();
  for KElem(value) in (*set).iter() {
    let fmt = CString::new("%s(").unwrap();
    if i < (*set).len() {
      fprintf(file, fmt.as_ptr(), concat);
    }
    fprintf(file, fmt.as_ptr(), element);
    let sort = CString::new("K").unwrap();
    printConfigurationInternal(file, *value, sort.as_ptr());
    fprintf(file, parens.as_ptr());
    if i < (*set).len() {
      fprintf(file, parens.as_ptr());
    }
    i += 1
  }
  for _ in 0..(*set).len()-1 {
    fprintf(file, parens.as_ptr());
  }
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
      (list).push_back(KElem(DUMMY0));
      (list).push_back(KElem(DUMMY1));
      (list).push_back(KElem(DUMMY2));
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

  #[test]
  fn test_eq() {
    unsafe {
      let set = hook_SET_element(DUMMY0);
      let set2 = hook_SET_element(DUMMY1);
      let result = hook_SET_eq(&set, &set2);
      assert!(!result);
      let set2 = hook_SET_element(DUMMY0);
      let result = hook_SET_eq(&set, &set2);
      assert!(result);
    }
  }
}
