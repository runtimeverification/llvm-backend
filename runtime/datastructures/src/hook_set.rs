use super::decls::Set;

#[no_mangle]
pub extern "C" fn alloc_set() -> *mut Set {
  let b = Box::new(Set::new());
  Box::into_raw(b)
}

#[no_mangle]
pub unsafe extern "C" fn free_set(ptr: *mut Set) {
  Box::from_raw(ptr);
}
