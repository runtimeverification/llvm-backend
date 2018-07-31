use super::decls::List;

#[no_mangle]
pub extern "C" fn alloc_list() -> *mut List {
  let b = Box::new(List::new());
  Box::into_raw(b)
}

#[no_mangle]
pub unsafe extern "C" fn free_list(ptr: *mut List) {
  Box::from_raw(ptr);
}
