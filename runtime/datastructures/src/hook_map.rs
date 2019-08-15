extern crate libc;

use super::decls::{Map,MapIter,Set,List,Int,K,KElem,__gmpz_init_set_ui,move_int,printConfigurationInternal,list_push_back,hook_LIST_unit};
use std::iter::FromIterator;
use std::hash::Hash;
use std::collections::hash_map::DefaultHasher;
use std::ptr;
use std::mem;
use std::ffi::CString;
use self::libc::{FILE,c_char,c_void,fprintf};

#[no_mangle]
pub extern "C" fn size_map() -> usize {
  mem::size_of::<Map>()
}

#[no_mangle]
pub unsafe extern "C" fn drop_map(ptr: *mut Map) {
  ptr::drop_in_place(ptr)
}

#[no_mangle]
pub unsafe extern "C" fn map_iterator(map: *const Map) -> MapIter {
  (*map).iter()
}

#[no_mangle]
pub unsafe extern "C" fn map_iterator_next(iter: *mut MapIter) -> K {
  match (*iter).next() {
    Some((KElem(elem),_)) => { *elem.get() }
    None => ptr::null()
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_element(key: K, value: K) -> Map {
  Map::unit(KElem::new(key), KElem::new(value))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_unit() -> Map {
  Map::new()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_concat(m1: *const Map, m2: *const Map) -> Map {
  if (*m1).len() < (*m2).len() {
    (*m2).clone().union_with((*m1).clone(), |_, _| { panic!("Duplicate keys") })
  } else {
    (*m1).clone().union_with((*m2).clone(), |_, _| { panic!("Duplicate keys") })
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup_null(m: *const Map, key: K) -> K {
  match (*m).get(&KElem::new(key)) {
    Some(KElem(v)) => { *v.get() } 
    None => ptr::null()
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup(m: *const Map, key: K) -> K {
  let res = hook_MAP_lookup_null(m, key);
  if res == ptr::null() {
    panic!("key not found")
  }
  res
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookupOrDefault(m: *const Map, key: K, default: K) -> K {
  match (*m).get(&KElem::new(key)) {
    Some(KElem(v)) => *v.get(),
    None => default
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_update(m: *const Map, key: K, value: K) -> Map {
  (*m).update(KElem::new(key), KElem::new(value))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_remove(m: *const Map, key: K) -> Map {
  (*m).without(&KElem::new(key))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_difference(m1: *const Map, m2: *const Map) -> Map {
  (*m1).clone().difference_with((*m2).clone(), |v1, v2| if v1 == v2 { None } else { Some(v1) })
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys(m: *const Map) -> Set {
  Set::from_iter((*m).keys().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys_list(m: *const Map) -> List {
  let mut list = hook_LIST_unit();
  for item in (*m).keys().cloned() {
    list = list_push_back(&list, item);
  }
  list
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_in_keys(key: K, m: *const Map) -> bool {
  (*m).contains_key(&KElem::new(key))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_values(m: *const Map) -> List {
  let mut list = hook_LIST_unit();
  for item in (*m).values().cloned() {
    list = list_push_back(&list, item);
  }
  list

}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_choice(m: *const Map) -> K {
  if (*m).is_empty() {
    panic!("Map is empty")
  }
  *(*m).keys().next().unwrap().0.get()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size_long(m: *const Map) -> usize {
  (*m).len()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size(m: *const Map) -> *mut Int {
  let mut result = Int(0, 0, ptr::null());
  __gmpz_init_set_ui(&mut result, hook_MAP_size_long(m));
  move_int(&mut result)
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
  for KElem(key) in (*set).iter() {
    tmp.remove(&KElem::new(*key.get()));
  }
  tmp
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_eq(m1: *const Map, m2: *const Map) -> bool {
  *m1 == *m2
}

#[no_mangle]
pub unsafe extern "C" fn map_hash(m: *const Map, h: *mut c_void) {
  let hasher = h as *mut &mut DefaultHasher;
  (*m).hash(*hasher)
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
  let comma = CString::new(",").unwrap();
  let sort = CString::new("K").unwrap();
  let fmt = CString::new("%s(").unwrap();
  for (KElem(key), KElem(value)) in (*map).iter() {
    if i < (*map).len() {
      fprintf(file, fmt.as_ptr(), concat);
    }
    fprintf(file, fmt.as_ptr(), element);
    printConfigurationInternal(file, *key.get(), sort.as_ptr(), false);
    fprintf(file, comma.as_ptr());
    printConfigurationInternal(file, *value.get(), sort.as_ptr(), false);
    fprintf(file, parens.as_ptr());
    if i < (*map).len() {
      fprintf(file, comma.as_ptr());
    }
    i += 1
  }
  for _ in 0..(*map).len()-1 {
    fprintf(file, parens.as_ptr());
  }
}

#[no_mangle]
pub unsafe extern "C" fn map_foreach(map: *mut Map, process: extern fn(block: *mut K)) {
  for (key, value) in (*map).iter() {
    process(key.0.get());
    process(value.0.get());
  }
}

#[no_mangle]
pub unsafe extern "C" fn map_map(map: *const Map, process: extern fn(block: K) -> K) -> Map {
  (*map).clone().into_iter().map(|(key,value)| (key, KElem::new(process(*value.0.get())))).collect()
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
      assert!((set).contains(&KElem::new(DUMMY0)));
    }
  }

  /*
  #[test]
  fn test_keys_list() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let list = hook_MAP_keys_list(&map);
      assert_eq!((list).get(0).unwrap(), &KElem::new(DUMMY0));
    }
  }
  */

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

  /*
  #[test]
  fn test_values() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let list = hook_MAP_values(&map);
      assert_eq!((list).get(0).unwrap(), &KElem::new(DUMMY0));
    }
  }
  */

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
      let set = Set::unit(KElem::new(DUMMY0));
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_removeAll(&m1, &set);
      let result = hook_MAP_size(&m2);
      assert_eq!(__gmpz_cmp_ui(result, 0), 0);
      free_int(result);
    }
  }

  #[test]
  fn test_eq() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY0);
      let m2 = hook_MAP_element(DUMMY1, DUMMY1);
      let result = hook_MAP_eq(&m1, &m2);
      assert!(!result);
      let m2 = hook_MAP_element(DUMMY0, DUMMY0);
      let result = hook_MAP_eq(&m1, &m2);
      assert!(result);
    }
  }
}
