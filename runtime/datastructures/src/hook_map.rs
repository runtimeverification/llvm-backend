extern crate libc;

use super::decls::{Map,Set,List,Int,K,__gmpz_init_set_ui,move_int,printConfigurationInternal};
use std::iter::FromIterator;
use std::ptr;
use std::mem;
use std::ffi::CString;
use self::libc::{FILE,c_char,fprintf};

#[no_mangle]
pub extern "C" fn size_map() -> usize {
  mem::size_of::<Map>()
}

#[no_mangle]
pub unsafe extern "C" fn drop_map(ptr: *mut Map) {
  ptr::drop_in_place(ptr)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_element(key: K, value: K) -> Map {
  Map::singleton(key, value)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_unit() -> Map {
  Map::new()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_concat(m1: *const Map, m2: *const Map) -> Map {
  (*m1).clone().union_with((*m2).clone(), |_, _| { panic!("Duplicate keys") })
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup(m: *const Map, key: K) -> K {
  match (*m).get(&key) {
    Some(v) => { *v }
    None => panic!("key not found")
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookupOrDefault(m: *const Map, key: K, default: K) -> K {
  match (*m).get(&key) {
    Some(v) => *v,
    None => default
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_update(m: *const Map, key: K, value: K) -> Map {
  (*m).update(key, value)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_remove(m: *const Map, key: K) -> Map {
  (*m).without(&key)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_difference(m1: *const Map, m2: *const Map) -> Map {
  (*m1).clone().difference_with((*m2).clone(), |v1, v2| if v1 == v2 { None } else { Some(v1) })
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys(m: *const Map) -> Set {
  Set::from_iter((*m).keys().map(|k| *k))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys_list(m: *const Map) -> List {
  List::from_iter((*m).keys().map(|k| *k))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_in_keys(key: K, m: *const Map) -> bool {
  (*m).contains_key(&key)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_values(m: *const Map) -> List {
  List::from_iter((*m).values().map(|k| *k))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_choice(m: *const Map) -> K {
  if (*m).is_empty() {
    panic!("Map is empty")
  }
  *(*m).keys().next().unwrap()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size(m: *const Map) -> *mut Int {
  let mut result = Int(0, 0, ptr::null());
  __gmpz_init_set_ui(&mut result, (*m).len());
  move_int(result)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_inclusion(m1: *const Map, m2: *const Map) -> bool {
  (*m1).is_submap(&*m2)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_updateAll(m1: *const Map, m2: *const Map) -> Map {
  (*m2).clone().union((*m1).clone())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_removeAll(map: *const Map, set: *const Set) -> Map {
  let mut tmp = (*map).clone();
  for key in (*set).iter() {
    tmp.remove(key);
  }
  tmp
}

#[no_mangle]
pub unsafe extern "C" fn printMap(file: *mut FILE, map: *const Map, unit: *const c_char, element: *const c_char, concat: *const c_char) {
  if (*map).len() == 0 {
    let fmt = CString::new("%s()").unwrap();
    fprintf(file, fmt.as_ptr(), unit);
    return;
  }
  let mut i = 1;
  let parens = CString::new(")").unwrap();
  for (key, value) in (*map).iter() {
    let fmt = CString::new("%s(").unwrap();
    if i < (*map).len() {
      fprintf(file, fmt.as_ptr(), concat);
    }
    fprintf(file, fmt.as_ptr(), element);
    let sort = CString::new("K").unwrap();
    printConfigurationInternal(file, *key, sort.as_ptr());
    let comma = CString::new(",").unwrap();
    fprintf(file, comma.as_ptr());
    printConfigurationInternal(file, *value, sort.as_ptr());
    fprintf(file, parens.as_ptr());
    if i < (*map).len() {
      fprintf(file, parens.as_ptr());
    }
    i += 1
  }
  for _ in 0..(*map).len()-1 {
    fprintf(file, parens.as_ptr());
  }
}

#[cfg(test)]
mod tests {
  extern crate libc;

  use decls::testing::*;
  use hook_map::*;

  #[test]
  fn test_element() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let result = hook_MAP_choice(&map);
      assert_eq!(result, DUMMY0);
      let result = hook_MAP_lookup(&map, DUMMY0);
      assert_eq!(result, DUMMY0);
    }
  }

  #[test]
  fn test_unit() {
    unsafe {
      let map = hook_MAP_unit();
      let result = hook_MAP_size(&map);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_concat_success() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY1);
      let m2 = hook_MAP_element(DUMMY1, DUMMY2);
      let map = hook_MAP_concat(&m1, &m2);
      let result = hook_MAP_size(&map);
      assert_eq!(__gmpz_cmp_ui(result, 2), 0);
      free_int(result);
    }
  }

  #[test]
  #[should_panic(expected = "Duplicate keys")]
  fn test_concat_failure() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY1);
      let m2 = hook_MAP_element(DUMMY0, DUMMY2);
      let _ = hook_MAP_concat(&m1, &m2);
    }
  }

  #[test]
  fn test_lookup_or_default() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let result = hook_MAP_lookupOrDefault(&map, DUMMY0, DUMMY1);
      assert_eq!(result, DUMMY0);
      let result = hook_MAP_lookupOrDefault(&map, DUMMY1, DUMMY1);
      assert_eq!(result, DUMMY1);
    }
  }

  #[test]
  fn test_update() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let result = hook_MAP_lookup(&map, DUMMY0);
      assert_eq!(result, DUMMY0);
      let map2 = hook_MAP_update(&map, DUMMY0, DUMMY1);
      let result = hook_MAP_lookup(&map, DUMMY0);
      assert_eq!(result, DUMMY0);
      let result = hook_MAP_lookup(&map2, DUMMY0);
      assert_eq!(result, DUMMY1);
    }
  }

  #[test]
  fn test_remove() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let map2 = hook_MAP_remove(&map, DUMMY0);
      let result = hook_MAP_size(&map);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      let result = hook_MAP_size(&map2);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_difference() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_element(DUMMY0, DUMMY0);
      let map = hook_MAP_difference(&m1, &m2);
      let result = hook_MAP_size(&map);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      let m2 = hook_MAP_update(&m2, DUMMY0, DUMMY1);
      let map = hook_MAP_difference(&m1, &m2);
      let result = hook_MAP_size(&map);
      assert_eq!(__gmpz_cmp_ui(result, 1), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_keys() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let set = hook_MAP_keys(&map);
      assert!((set).contains(&DUMMY0));
    }
  }

  #[test]
  fn test_keys_list() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let list = hook_MAP_keys_list(&map);
      assert_eq!((list).get(0).unwrap(), &DUMMY0);
    }
  }

  #[test]
  fn test_in_keys() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let result = hook_MAP_in_keys(DUMMY0, &map);
      assert!(result);
      let result = hook_MAP_in_keys(DUMMY1, &map);
      assert!(!result);
    }
  }

  #[test]
  fn test_values() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let list = hook_MAP_values(&map);
      assert_eq!((list).get(0).unwrap(), &DUMMY0);
    }
  }

  #[test]
  fn test_inclusion() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_element(DUMMY1, DUMMY1);
      let result = hook_MAP_inclusion(&m1, &m2);
      assert!(!result);
      let m2 = hook_MAP_update(&m2, DUMMY0, DUMMY0);
      let result = hook_MAP_inclusion(&m1, &m2);
      assert!(result);
      let m2 = hook_MAP_element(DUMMY0, DUMMY1);
      let result = hook_MAP_inclusion(&m1, &m2);
      assert!(!result);
    }
  }

  #[test]
  fn test_update_all() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_element(DUMMY1, DUMMY1);
      let m2 = hook_MAP_update(&m2, DUMMY0, DUMMY1);
      let map = hook_MAP_updateAll(&m1, &m2);
      let result = hook_MAP_lookup(&map, DUMMY0);
      assert_eq!(result, DUMMY1);
      let result = hook_MAP_lookup(&map, DUMMY1);
      assert_eq!(result, DUMMY1);
    }
  }

  #[test]
  fn test_remove_all() {
    unsafe {
      let set = Set::singleton(DUMMY0);
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_removeAll(&m1, &set);
      let result = hook_MAP_size(&m2);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
    }
  }
}
