use super::decls::{Map,Set,List,Int,K,__gmpz_set_ui};
use std::iter::FromIterator;
use std::ptr;
use std::mem;

#[no_mangle]
pub extern "C" fn size_map() -> usize {
  mem::size_of::<Map>()
}

#[no_mangle]
pub unsafe extern "C" fn drop_map(ptr: *mut Map) {
  ptr::drop_in_place(ptr)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_element(result: *mut Map, key: K, value: K) -> bool {
  ptr::write(result, Map::singleton(key, value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_unit(result: *mut Map) -> bool {
  ptr::write(result, Map::new());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_concat(result: *mut Map, m1: *const Map, m2: *const Map) -> bool {
  let mut status = true;
  ptr::write(result, (*m1).clone().union_with((*m2).clone(), |v1, _| { status = false; v1 }));
  status
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup(result: *mut K, m: *const Map, key: K) -> bool {
  match (*m).get(&key) {
    Some(v) => { ptr::write(result, *v); true }
    None => false
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookupOrDefault(result: *mut K, m: *const Map, key: K, default: K) -> bool {
  match (*m).get(&key) {
    Some(v) => *result = *v,
    None => *result = default
  }
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_update(result: *mut Map, m: *const Map, key: K, value: K) -> bool {
  ptr::write(result, (*m).update(key, value));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_remove(result: *mut Map, m: *const Map, key: K) -> bool {
  ptr::write(result, (*m).without(&key));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_difference(result: *mut Map, m1: *const Map, m2: *const Map) -> bool {
  ptr::write(result, (*m1).clone().difference_with((*m2).clone(), |v1, v2| if v1 == v2 { None } else { Some(v1) }));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys(result: *mut Set, m: *const Map) -> bool {
  ptr::write(result, Set::from_iter((*m).keys().map(|k| *k)));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys_list(result: *mut List, m: *const Map) -> bool {
  ptr::write(result, List::from_iter((*m).keys().map(|k| *k)));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_in_keys(result: *mut bool, key: K, m: *const Map) -> bool {
  ptr::write(result, (*m).contains_key(&key));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_values(result: *mut List, m: *const Map) -> bool {
  ptr::write(result, List::from_iter((*m).values().map(|k| *k)));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_choice(result: *mut K, m: *const Map) -> bool {
  if (*m).is_empty() {
    return false;
  }
  ptr::write(result, *(*m).keys().next().unwrap());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size(result: *mut Int, m: *const Map) -> bool {
  __gmpz_set_ui(result, (*m).len());
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_inclusion(result: *mut bool, m1: *const Map, m2: *const Map) -> bool {
  ptr::write(result, (*m1).is_submap(&*m2));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_updateAll(result: *mut Map, m1: *const Map, m2: *const Map) -> bool {
  ptr::write(result, (*m2).clone().union((*m1).clone()));
  true
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_removeAll(result: *mut Map, map: *const Map, set: *const Set) -> bool {
  let mut tmp = (*map).clone();
  for key in (*set).iter() {
    tmp.remove(key);
  }
  ptr::write(result, tmp);
  true
}

#[cfg(test)]
mod tests {
  extern crate libc;

  use decls::testing::*;
  use hook_map::*;
  use hook_set::tests::{alloc_set,free_set};
  use hook_list::tests::{alloc_list,free_list};

  pub unsafe fn alloc_map() -> *mut Map {
    let ptr = libc::malloc(size_map()) as *mut Map;
    ptr
  }

  pub unsafe fn free_map(ptr: *mut Map) {
    drop_map(ptr);
    libc::free(ptr as *mut libc::c_void);
  }

  #[test]
  fn test_element() {
    unsafe {
      let map = alloc_map();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      let result = alloc_k();
      assert!(hook_MAP_choice(result, map));
      assert_eq!(*result, DUMMY0);
      assert!(hook_MAP_lookup(result, map, DUMMY0));
      assert_eq!(*result, DUMMY0);
      free_map(map);
      free_k(result);
    }
  }

  #[test]
  fn test_unit() {
    unsafe {
      let map = alloc_map();
      assert!(hook_MAP_unit(map));
      let result = alloc_int();
      assert!(hook_MAP_size(result, map));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_map(map);
      free_int(result);
    }
  }

  #[test]
  fn test_concat_success() {
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      let map = alloc_map();
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY1));
      assert!(hook_MAP_element(m2, DUMMY1, DUMMY2));
      assert!(hook_MAP_concat(map, m1, m2));
      let result = alloc_int();
      assert!(hook_MAP_size(result, map));
      assert_eq!(__gmpz_cmp_ui(result, 2), 0);
      free_map(m1);
      free_map(m2);
      free_map(map);
      free_int(result);
    }
  }

  #[test]
  fn test_concat_failure() {
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      let map = alloc_map();
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY1));
      assert!(hook_MAP_element(m2, DUMMY0, DUMMY2));
      assert!(!hook_MAP_concat(map, m1, m2));
      free_map(m1);
      free_map(m2);
      free_map(map);
    }
  }

  #[test]
  fn test_lookup_or_default() {
    unsafe {
      let map = alloc_map();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      let result = alloc_k();
      assert!(hook_MAP_lookupOrDefault(result, map, DUMMY0, DUMMY1));
      assert_eq!(*result, DUMMY0);
      assert!(hook_MAP_lookupOrDefault(result, map, DUMMY1, DUMMY1));
      assert_eq!(*result, DUMMY1);
      free_map(map);
      free_k(result);
    }
  }

  #[test]
  fn test_update() {
    unsafe {
      let map = alloc_map();
      let map2 = alloc_map();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      let result = alloc_k();
      assert!(hook_MAP_lookup(result, map, DUMMY0));
      assert_eq!(*result, DUMMY0);
      assert!(hook_MAP_update(map2, map, DUMMY0, DUMMY1));
      assert!(hook_MAP_lookup(result, map, DUMMY0));
      assert_eq!(*result, DUMMY0);
      assert!(hook_MAP_lookup(result, map2, DUMMY0));
      assert_eq!(*result, DUMMY1);
      free_map(map);
      free_map(map2);
      free_k(result);
    }
  }

  #[test]
  fn test_remove() {
    unsafe {
      let map = alloc_map();
      let map2 = alloc_map();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      assert!(hook_MAP_remove(map2, map, DUMMY0));
      let result = alloc_int();
      assert!(hook_MAP_size(result, map));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      assert!(hook_MAP_size(result, map2));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_map(map);
      free_map(map2);
      free_int(result);
    }
  }

  #[test]
  fn test_difference() {
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      let map = alloc_map();
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY0));
      assert!(hook_MAP_element(m2, DUMMY0, DUMMY0));
      assert!(hook_MAP_difference(map, m1, m2));
      let result = alloc_int();
      assert!(hook_MAP_size(result, map));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      assert!(hook_MAP_update(m2, m2, DUMMY0, DUMMY1));
      assert!(hook_MAP_difference(map, m1, m2));
      assert!(hook_MAP_size(result, map));
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_map(map);
      free_map(m1);
      free_map(m2);
      free_int(result);
    }
  }

  #[test]
  fn test_keys() {
    unsafe {
      let map = alloc_map();
      let set = alloc_set();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      assert!(hook_MAP_keys(set, map));
      assert!((*set).contains(&DUMMY0));
      free_set(set);
      free_map(map);
    }
  }

  #[test]
  fn test_keys_list() {
    unsafe {
      let map = alloc_map();
      let list = alloc_list();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      assert!(hook_MAP_keys_list(list, map));
      assert_eq!((*list).get(0).unwrap(), &DUMMY0);
      free_list(list);
      free_map(map);
    }
  }

  #[test]
  fn test_in_keys() {
    let mut result = false;
    unsafe {
      let map = alloc_map();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      assert!(hook_MAP_in_keys(&mut result, DUMMY0, map));
      assert!(result);
      assert!(hook_MAP_in_keys(&mut result, DUMMY1, map));
      assert!(!result);
      free_map(map);
    }
  }

  #[test]
  fn test_values() {
    unsafe {
      let map = alloc_map();
      let list = alloc_list();
      assert!(hook_MAP_element(map, DUMMY0, DUMMY0));
      assert!(hook_MAP_values(list, map));
      assert_eq!((*list).get(0).unwrap(), &DUMMY0);
      free_list(list);
      free_map(map);
    }
  }

  #[test]
  fn test_inclusion() {
    let mut result = true;
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY0));
      assert!(hook_MAP_element(m2, DUMMY1, DUMMY1));
      assert!(hook_MAP_inclusion(&mut result, m1, m2));
      assert!(!result);
      assert!(hook_MAP_update(m2, m2, DUMMY0, DUMMY0));
      assert!(hook_MAP_inclusion(&mut result, m1, m2));
      assert!(result);
      assert!(hook_MAP_element(m2, DUMMY0, DUMMY1));
      result = false;
      assert!(hook_MAP_inclusion(&mut result, m1, m2));
      assert!(!result);
      free_map(m1);
      free_map(m2);
    }
  }

  #[test]
  fn test_update_all() {
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      let map = alloc_map();
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY0));
      assert!(hook_MAP_element(m2, DUMMY1, DUMMY1));
      assert!(hook_MAP_update(m2, m2, DUMMY0, DUMMY1));
      assert!(hook_MAP_updateAll(map, m1, m2));
      let result = alloc_k();
      assert!(hook_MAP_lookup(result, map, DUMMY0));
      assert_eq!(*result, DUMMY1);
      assert!(hook_MAP_lookup(result, map, DUMMY1));
      assert_eq!(*result, DUMMY1);
      free_map(m1);
      free_map(m2);
      free_map(map);
      free_k(result);
    }
  }

  #[test]
  fn test_remove_all() {
    unsafe {
      let m1 = alloc_map();
      let m2 = alloc_map();
      let set = alloc_set();
      ptr::write(set, Set::singleton(DUMMY0));
      assert!(hook_MAP_element(m1, DUMMY0, DUMMY0));
      assert!(hook_MAP_removeAll(m2, m1, set));
      let result = alloc_int();
      assert!(hook_MAP_size(result, m2));
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
      free_set(set);
      free_map(m1);
      free_map(m2);
    }
  }
}
