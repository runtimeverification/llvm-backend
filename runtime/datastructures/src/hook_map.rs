extern crate libc;

use super::decls::{Map,Set,List,Int,K,KElem,ord_map_compare,__gmpz_init_set_ui,move_int,printConfigurationInternal};
use std::cmp::Ordering;
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
pub unsafe extern "C" fn hook_MAP_element(key: K, value: K) -> Map {
  println!("!!! ELE");
  Map::singleton(KElem::new(key), KElem::new(value))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_unit() -> Map {
   println!("UNIT");
   let x = Map::new();
   println!("NEW {:?}", x);
   x
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_concat(m1: *const Map, m2: *const Map) -> Map {
  println!("!!! CAT {:?} & {:?}", m1, m2);
  (*m1).clone().union_with((*m2).clone(), |_, _| { panic!("Duplicate keys") })
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup_null(m: *const Map, key: K) -> K {
  println!("!!! LUN {:?} ", m);
  match (*m).get(&KElem::new(key)) {
    Some(KElem(v)) => { *v.get() } 
    None => ptr::null()
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookup(m: *const Map, key: K) -> K {
  println!("!!! LU {:?} ", m);
  let res = hook_MAP_lookup_null(m, key);
  if res == ptr::null() {
    panic!("key not found")
  }
  res
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_lookupOrDefault(m: *const Map, key: K, default: K) -> K {
  println!("!!! LUD {:?} ", m);
  match (*m).get(&KElem::new(key)) {
    Some(KElem(v)) => *v.get(),
    None => default
  }
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_update(m: *const Map, key: K, value: K) -> Map {
  let view = m as *mut u64;
  println!("UPDATE ADDR {:?} with key: {:?}, value: {:?}", m, key, value);
  for i in 0..2 {
    println!("{}: {:08x} ", i, *view.offset(i));
  }
  (*m).update(KElem::new(key), KElem::new(value))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_remove(m: *const Map, key: K) -> Map {
  println!("!!! REM {:?} ", m);
  (*m).without(&KElem::new(key))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_difference(m1: *const Map, m2: *const Map) -> Map {
  println!("!!! DIFF {:?} & {:?}", m1, m2);
  (*m1).clone().difference_with((*m2).clone(), |v1, v2| if v1 == v2 { None } else { Some(v1) })
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys(m: *const Map) -> Set {
  println!("!!! KEYS {:?} ", m);
  Set::from_iter((*m).keys().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_keys_list(m: *const Map) -> List {
  println!("!!! KEYSL {:?} ", m);
  List::from_iter((*m).keys().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_in_keys(key: K, m: *const Map) -> bool {
  println!("!!! IN {:?} ", m);
  (*m).contains_key(&KElem::new(key))
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_values(m: *const Map) -> List {
  println!("!!! VALS {:?} ", m);
  List::from_iter((*m).values().cloned())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_choice(m: *const Map) -> K {
  println!("!!! CHOICE {:?} ", m);
  if (*m).is_empty() {
    panic!("Map is empty")
  }
  *(*m).keys().next().unwrap().0.get()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size_long(m: *const Map) -> usize {
  println!("!!! SIZEL {:?} ", m);
  (*m).len()
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_size(m: *const Map) -> *mut Int {
  println!("!!! KEYS {:?} ", m);
  let mut result = Int(0, 0, ptr::null());
  __gmpz_init_set_ui(&mut result, hook_MAP_size_long(m));
  move_int(&mut result)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_inclusion(m1: *const Map, m2: *const Map) -> bool {
  println!("!!! INC {:?} & {:?}", m1, m2);
  (*m1).is_submap(&*m2)
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_updateAll(m1: *const Map, m2: *const Map) -> Map {
  println!("!!! UPALL {:?} & {:?}", m1, m2);
  (*m2).clone().union((*m1).clone())
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_removeAll(map: *const Map, set: *const Set) -> Map {
  println!("!!! REMALL {:?}", map);
  let mut tmp = (*map).clone();
  for KElem(key) in (*set).iter() {
    tmp.remove(&KElem::new(*key.get()));
  }
  tmp
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_eq(m1: *const Map, m2: *const Map) -> bool {
  println!("!!! EQ {:?} & {:?}", m1, m2);
  *m1 == *m2
}

#[no_mangle]
pub unsafe extern "C" fn hook_MAP_cmp(m1: *const Map, m2: *const Map) -> i64 {
  println!("!!! EQ {:?} & {:?}", m1, m2);
  match ord_map_compare(std::mem::transmute::<*const Map, &Map>(m1),
                         std::mem::transmute::<*const Map, &Map>(m2)) {
    Ordering::Less => -1,
    Ordering::Equal => 0,
    Ordering::Greater => 1,
  }
}

#[no_mangle]
pub unsafe extern "C" fn map_hash(m: *const Map, h: *mut c_void) {
  println!("!!! HASH {:?}", m);
  let hasher = h as *mut &mut DefaultHasher;
  m.hash(*hasher)
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
    printConfigurationInternal(file, *key.get(), sort.as_ptr());
    fprintf(file, comma.as_ptr());
    printConfigurationInternal(file, *value.get(), sort.as_ptr());
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
  fn test_cmp() {
    unsafe {
      let m1 = hook_MAP_element(DUMMY0, DUMMY1);
      let m2 = hook_MAP_element(DUMMY1, DUMMY2);
      assert_eq!(hook_MAP_cmp(&m1, &m2), -1);
      assert_eq!(hook_MAP_cmp(&m2, &m1), 1);
      assert_eq!(hook_MAP_cmp(&m1, &m1), 0);
      assert_eq!(hook_MAP_cmp(&m2, &m2), 0);
      // These two show that we order correctly where
      // the key is the same but the value differs.
      let m3 = hook_MAP_element(DUMMY2, DUMMY3);
      let m4 = hook_MAP_element(DUMMY2, DUMMY4);
      assert_eq!(hook_MAP_cmp(&m3, &m4), -1);
      assert_eq!(hook_MAP_cmp(&m4, &m3), 1);
      assert_eq!(hook_MAP_cmp(&m3, &m3), 0);
      assert_eq!(hook_MAP_cmp(&m4, &m4), 0);
      let m5 = hook_MAP_concat(&m1, &m3);
      let m6 = hook_MAP_concat(&m2, &m4);
      assert_eq!(hook_MAP_cmp(&m5, &m6), -1);
      assert_eq!(hook_MAP_cmp(&m6, &m5), 1);
      assert_eq!(hook_MAP_cmp(&m5, &m5), 0);
      assert_eq!(hook_MAP_cmp(&m6, &m6), 0);
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

  #[test]
  fn test_keys_list() {
    unsafe {
      let map = hook_MAP_element(DUMMY0, DUMMY0);
      let list = hook_MAP_keys_list(&map);
      assert_eq!((list).get(0).unwrap(), &KElem::new(DUMMY0));
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
      assert_eq!((list).get(0).unwrap(), &KElem::new(DUMMY0));
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
      let set = Set::singleton(KElem::new(DUMMY0));
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
