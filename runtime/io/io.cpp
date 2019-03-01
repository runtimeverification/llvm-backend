#include <gmp.h>
#include <cstring>
#include <stdexcept>

#include "fcntl.h"
#include "unistd.h"
#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
  
  mpz_ptr move_int(mpz_t);

  char * getTerminatedString(string * str) {
    char * buf = (char *) malloc(sizeof(KCHAR) * (len(str) + 1));
    strncpy(buf, str->data, len(str));
    buf[len(str)] = '\0';
    return buf;
  }
  
  mpz_ptr hook_IO_open(string * filename, string * control) {
    int flags = 0;
    char * ctl = getTerminatedString(control);

    switch (*ctl) {
      case 'r':
        flags = O_RDONLY;
        if (len(control) > 1 && *(++ctl) == '+'){
          flags = O_RDWR;
        }
      case 'w':
        flags = O_WRONLY | O_TRUNC | O_CREAT;
        if (len(control) > 1 && *(++ctl) == '+'){
          flags = O_RDWR | O_TRUNC | O_CREAT;
        }
      case 'a':
        flags = O_WRONLY | O_APPEND | O_CREAT;
        if (len(control) > 1 && *(++ctl) == '+'){
          flags = O_RDWR | O_APPEND | O_CREAT;
        }
      default:
        flags = O_RDONLY;
    }

    mpz_t result;
    char * f = getTerminatedString(filename);
    int fd = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    mpz_init_set_si(result, fd);

    free(f);
    free(ctl);

    return move_int(result);
  }

  mpz_ptr hook_IO_tell(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);

    off_t loc = lseek(fd, 0, SEEK_CUR);

    mpz_t result;
    mpz_init_set_si(result, int(loc));
    return move_int(result);
  }

  mpz_ptr hook_IO_getc(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);
    char c;
    read(fd, &c, sizeof(char));

    mpz_t result;
    mpz_init_set_si(result, int(c));
    return move_int(result);
  }

  string * hook_IO_read(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int length = mpz_get_si(len);

    char * buf = new char[length];
    int bytes = read(fd, buf, length);

    auto result = static_cast<string *>(koreAllocToken(sizeof(string) + bytes));
    set_len(result, bytes);
    memcpy(&(result->data), buf, bytes * sizeof(KCHAR));

    delete[] buf;

    return result;
  }

  void hook_IO_close(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int64_t");
    }

    int fd = mpz_get_si(i);
    close(fd);
  }

  void hook_IO_seek(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int l = mpz_get_si(loc);
    lseek(fd, l, SEEK_SET);
  }

  void hook_IO_seekEnd(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int l = mpz_get_si(loc);
    lseek(fd, l, SEEK_END);
  }

  void hook_IO_putc(mpz_t i, mpz_t c) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int ch = mpz_get_si(c);

    write(fd, &ch, 1);
  }

  void hook_IO_write(mpz_t i, string * str) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);
    char * s = getTerminatedString(str);

    write(fd, s, len(str));
  }

  void hook_IO_lock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int l = mpz_get_si(len);

    lockf(fd, F_LOCK, l);
  }

  void hook_IO_unlock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int l = mpz_get_si(len);

    lockf(fd, F_ULOCK, l);
  }
}
