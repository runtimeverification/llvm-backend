use super::decls::{List,Int,K,__gmpz_fits_ulong_p,__gmpz_get_ui,__gmpz_init_set_ui};
use std::ptr;
use std::mem;

#[no_mangle]
pub extern "C" fn size_list() -> usize {
  mem::size_of::<List>()
}

#[no_mangle]
pub unsafe extern "C" fn drop_list(ptr: *mut List) {
  ptr::drop_in_place(ptr)
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_unit(result: *mut List) -> bool {
  ptr::write(result, List::new());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_element(result: *mut List, value: K) -> bool {
  ptr::write(result, List::singleton(value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_concat(result: *mut List, l1: *const List, l2: *const List) -> bool {
  let mut tmp = (*l1).clone();
  tmp.append((*l2).clone());
  ptr::write(result, tmp);
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_in(result: *mut bool, value: K, list: *const List) -> bool {
  ptr::write(result, (*list).contains(&value));
  true
}

unsafe fn get_long(i: *const Int) -> (bool, usize) {
  if !(__gmpz_fits_ulong_p(i) != 0) {
    return (false, 0);
  }
  (true, __gmpz_get_ui(i) as usize)
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_get(result: *mut K, list: *const List, index: *const Int) -> bool {
  let (status, index_long) = get_long(index);
  if !status {
    return false;
  }
  match (*list).get(index_long) {
    Some(elem) => { ptr::write(result, *elem); true }
    None => false
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_range(result: *mut List, list: *const List, from_front: *const Int, from_back: *const Int) -> bool {
  let (status, front_long) = get_long(from_front);
  if !status {
    return false;
  }
  let (status, back_long) = get_long(from_back);
  if !status {
    return false;
  }
  let old_len = (*list).len();
  if old_len < front_long + back_long {
    return false;
  }
  ptr::write(result, (*list).skip(front_long).take(old_len - front_long - back_long));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_size(result: *mut Int, l: *const List) -> bool {
  __gmpz_init_set_ui(result, (*l).len());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_make(result: *mut List, len: *const Int, value: K) -> bool {
  let mut tmp = List::new();
  let (status, len_long) = get_long(len);
  if !status {
    return false;
  }
  for _ in 0..len_long {
    tmp.push_back(value);
  }
  ptr::write(result, tmp);
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_update(result: *mut List, list: *const List, index: *const Int, value: K) -> bool {
  let (status, index_long) = get_long(index);
  if !status {
    return false;
  }
  if index_long >= (*list).len() {
    return false;
  }
  ptr::write(result, (*list).update(index_long, value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_LIST_updateAll(result: *mut List, l1: *const List, index: *const Int, l2: *const List) -> bool {
  let (status, index_long) = get_long(index);
  if !status {
    return false;
  }
  if index_long != 0 && (*l2).len() != 0 {
    if index_long + (*l2).len() - 1 >= (*l1).len() {
      return false;
    }
  }
  let mut before = (*l1).take(index_long);
  let after = (*l1).skip(index_long + (*l2).len());
  before.append((*l2).clone());
  before.append(after);
  ptr::write(result, before);
  true
}

#[cfg(test)]
pub mod tests {
  extern crate libc;

  use decls::testing::*;
  use hook_list::*;

  pub unsafe fn alloc_list() -> *mut List {
    let ptr = libc::malloc(size_list()) as *mut List;
    ptr
  }

  pub unsafe fn free_list(ptr: *mut List) {
    drop_list(ptr);
    libc::free(ptr as *mut libc::c_void);
  }

  #[test]
  fn test_element() {
    unsafe {
      let list = alloc_list();
      assert!(hook_LIST_element(list, DUMMY0));
      let result = alloc_k();
      let index = alloc_int();
      __gmpz_init_set_ui(index, 0);
      assert!(hook_LIST_get(result, list, index));
      assert_eq!(*result, DUMMY0);
      free_list(list);
      free_k(result);
      free_int(index);
    }
  }

  #[test]
  fn test_unit() {
    unsafe {
      let list = alloc_list();
      assert!(hook_LIST_unit(list));
      let result = alloc_int();
      assert!(hook_LIST_size(result, list));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_list(list);
      free_int(result);
    }
  }

  #[test]
  fn test_concat() {
    unsafe {
      let l1 = alloc_list();
      let l2 = alloc_list();
      let list = alloc_list();
      assert!(hook_LIST_element(l1, DUMMY0));
      assert!(hook_LIST_element(l2, DUMMY1));
      assert!(hook_LIST_concat(list, l1, l2));
      let result = alloc_k();
      let index = alloc_int();
      __gmpz_init_set_ui(index, 0);
      assert!(hook_LIST_get(result, list, index));
      assert_eq!(*result, DUMMY0);
      __gmpz_clear(index);
      __gmpz_init_set_ui(index, 1);
      assert!(hook_LIST_get(result, list, index));
      assert_eq!(*result, DUMMY1);
      __gmpz_clear(index);
      assert!(hook_LIST_size(index, list));
      assert_eq!(__gmpz_cmp_ui(index, 2), 0);
      free_list(l1);
      free_list(l2);
      free_list(list);
      free_k(result);
      free_int(index);
    }
  }

  #[test]
  fn test_in() {
    let mut result = false;
    unsafe {
      let list = alloc_list();
      assert!(hook_LIST_element(list, DUMMY0));
      assert!(hook_LIST_in(&mut result, DUMMY0, list));
      assert!(result);
      assert!(hook_LIST_in(&mut result, DUMMY1, list));
      assert!(!result);
      free_list(list);
    }
  }

  #[test]
  fn test_get() {
    unsafe {
      let index = alloc_int();
      __gmpz_init_set_si(index, -1);
      let list = alloc_list();
      let result = alloc_k();
      assert!(hook_LIST_element(list, DUMMY0));
      assert!(!hook_LIST_get(result, list, index));
      __gmpz_clear(index);
      __gmpz_init_set_ui(index, 1);
      assert!(!hook_LIST_get(result, list, index));
      free_int(index);
      free_list(list);
      free_k(result);
    }
  }

  #[test]
  fn test_range() {
    unsafe {
      let neg = alloc_int();
      let zero = alloc_int();
      let one = alloc_int();
      __gmpz_init_set_si(neg, -1);
      __gmpz_init_set_ui(zero, 0);
      __gmpz_init_set_ui(one, 1);
      let list = alloc_list();
      let result = alloc_list();
      assert!(hook_LIST_element(list, DUMMY0));
      assert!(!hook_LIST_range(result, list, neg, zero));
      assert!(!hook_LIST_range(result, list, zero, neg));
      assert!(!hook_LIST_range(result, list, one, one));
      assert!(hook_LIST_range(result, list, zero, one));
      __gmpz_clear(zero);
      assert!(hook_LIST_size(zero, result));
      assert_eq!(__gmpz_cmp_ui(zero, 0), 0);
      free_int(neg);
      free_int(zero);
      free_int(one);
      free_list(list);
      free_list(result);
    }
  }

  #[test]
  fn test_make() {
    unsafe {
      let neg = alloc_int();
      let zero = alloc_int();
      let ten = alloc_int();
      __gmpz_init_set_si(neg, -1);
      __gmpz_init_set_ui(zero, 0);
      __gmpz_init_set_ui(ten, 10);
      let list = alloc_list();
      assert!(!hook_LIST_make(list, neg, DUMMY0));
      assert!(hook_LIST_make(list, ten, DUMMY0));
      __gmpz_clear(ten);
      let result = alloc_k();
      assert!(hook_LIST_get(result, list, zero));
      assert_eq!(*result, DUMMY0);
      assert!(hook_LIST_size(ten, list));
      assert_eq!(__gmpz_cmp_ui(ten, 10), 0);
      free_int(neg);
      free_int(zero);
      free_int(ten);
      free_list(list);
      free_k(result);
    }
  }

  #[test]
  fn test_update() {
    unsafe {
      let list = alloc_list();
      assert!(hook_LIST_element(list, DUMMY0));
      let neg = alloc_int();
      let index = alloc_int();
      let one = alloc_int();
      __gmpz_init_set_si(neg, -1);
      __gmpz_init_set_ui(index, 0);
      __gmpz_init_set_ui(one, 1);
      assert!(!hook_LIST_update(list, list, neg, DUMMY1));
      assert!(!hook_LIST_update(list, list, one, DUMMY1));
      assert!(hook_LIST_update(list, list, index, DUMMY1));
      let result = alloc_k();
      assert!(hook_LIST_get(result, list, index));
      assert_eq!(*result, DUMMY1);
      free_k(result);
      free_int(index);
      free_int(neg);
      free_int(one);
      free_list(list);
    }
  }

  #[test]
  fn test_update_all() {
    unsafe {
      let l1 = alloc_list();
      let l2 = alloc_list();
      let list = alloc_list();
      let neg = alloc_int();
      let zero = alloc_int();
      let one = alloc_int();
      __gmpz_init_set_si(neg, -1);
      __gmpz_init_set_ui(zero, 0);
      __gmpz_init_set_ui(one, 1);
      assert!(hook_LIST_element(l1, DUMMY0));
      assert!(hook_LIST_unit(l2));
      assert!(!hook_LIST_updateAll(list, l1, neg, l2));
      assert!(hook_LIST_updateAll(list, l1, one, l2));
      let result = alloc_k();
      assert!(hook_LIST_get(result, list, zero));
      assert_eq!(*result, DUMMY0);
      assert!(hook_LIST_updateAll(list, l1, zero, l2));
      assert!(hook_LIST_get(result, list, zero));
      assert_eq!(*result, DUMMY0);
      assert!(hook_LIST_element(l2, DUMMY1));
      assert!(hook_LIST_updateAll(list, l1, zero, l2));
      assert!(hook_LIST_get(result, list, zero));
      assert_eq!(*result, DUMMY1);
      assert!(!hook_LIST_updateAll(list, l1, one, l2));
      free_list(l1);
      free_list(l2);
      free_list(list);
      free_int(neg);
      free_int(zero);
      free_int(one);
      free_k(result);
    }
  }
}
