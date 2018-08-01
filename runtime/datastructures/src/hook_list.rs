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
  if index_long + (*l2).len() - 1 >= (*l1).len() {
    return false;
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

  use hook_list::*;

  pub unsafe fn alloc_list() -> *mut List {
    let ptr = libc::malloc(size_list()) as *mut List;
    ptr
  }

  pub unsafe fn free_list(ptr: *mut List) {
    drop_list(ptr);
    libc::free(ptr as *mut libc::c_void);
  }
}
