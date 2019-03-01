#include<boost/test/unit_test.hpp>
#include<gmp.h>
#include<mpfr.h>
#include<cstdint>
#include<cstdlib>
#include<cstring>

#include "runtime/header.h"
#include "runtime/alloc.h"

#include "fcntl.h"
#include "unistd.h"

#define KCHAR char
extern "C" {
  uint32_t getTagForSymbolName(const char *s) {
    return 0;
  }

  void add_hash64(void*, uint64_t) {}

  string * makeString(const KCHAR *, int64_t len = -1);
  mpz_ptr hook_IO_open(string * filename, string * control);
  mpz_ptr hook_IO_tell(mpz_t i);
  mpz_ptr hook_IO_getc(mpz_t i);
  string * hook_IO_read(mpz_t i, mpz_t len);
  void hook_IO_close(mpz_t i);
  void hook_IO_putc(mpz_t i, mpz_t c);

  mpz_ptr move_int(mpz_t i) {
    mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
    *result = *i;
    return result;
  }

  floating *move_float(floating *i) {
    floating *result = (floating *)malloc(sizeof(floating));
    *result = *i;
    return result;
  }
}

BOOST_AUTO_TEST_SUITE(IoTest)

BOOST_AUTO_TEST_CASE(open) {
  auto realFilename = makeString("test.txt");
  auto fakeFilename = makeString("testFake.txt");
  auto control = makeString("r");

  BOOST_CHECK(0 < mpz_cmp_si(hook_IO_open(realFilename, control), 0));
  BOOST_CHECK(0 > mpz_cmp_si(hook_IO_open(fakeFilename, control), 0));
}

BOOST_AUTO_TEST_CASE(tell) {
  mpz_t f;
  int fd = ::open("test.txt", O_RDWR);
  mpz_init_set_si(f, fd);

  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_tell(f), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), 5, SEEK_CUR);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_tell(f), 5));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_tell(f), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), -4, SEEK_CUR);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_tell(f), 1));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_tell(f), lseek(mpz_get_si(f), 0, SEEK_CUR)));
}

BOOST_AUTO_TEST_CASE(getc) {
  mpz_t f;
  int fd = ::open("test.txt", O_RDWR);
  mpz_init_set_si(f, fd);

  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_getc(f), int('h')));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_getc(f), int('e')));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_getc(f), int('l')));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_getc(f), int('l')));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_IO_getc(f), int('o')));
}

BOOST_AUTO_TEST_CASE(read) {
  mpz_t f;
  mpz_t len;
  int fd = ::open("test.txt", O_RDWR);
  mpz_init_set_si(f, fd);
  mpz_init_set_si(len, 6);

  auto str = hook_IO_read(f, len);

  BOOST_CHECK_EQUAL(0, strncmp(str->data, "hello ", 6));
  str = hook_IO_read(f, len);
  BOOST_CHECK_EQUAL(0, strncmp(str->data, "world!", 6));
}

BOOST_AUTO_TEST_CASE(close) {
  mpz_t f1;
  mpz_t f2;
  int fd1 = ::open("test.txt", O_RDONLY);
  mpz_init_set_si(f1, fd1);
  int fd2 = ::open("test.txt", O_RDONLY);
  mpz_init_set_si(f2, fd2);

  hook_IO_close(f1);
  hook_IO_close(f2);

  BOOST_CHECK_EQUAL(-1, fcntl(fd1, F_GETFD));
  BOOST_CHECK_EQUAL(-1, fcntl(fd2, F_GETFD));
}

BOOST_AUTO_TEST_CASE(putc) {
  /*
  char rc[2];
  mpz_t f;
  mpz_t c;

  int fd = ::open("test.txt", O_RDWR | O_TRUNC);
  mpz_init_set_si(f, fd);

  mpz_init_set_si(c, int('h'));
  hook_IO_putc(f, c);
  lseek(mpz_get_si(f), -1, SEEK_CUR);
  ::read(fd, &rc, 1);

  printf("%s\n", rc);

  BOOST_CHECK_EQUAL('h', rc[0]);

  mpz_set_si(c, int('e'));
  hook_IO_putc(f, c);
  lseek(mpz_get_si(f), -1, SEEK_CUR);
  ::read(fd, &rc, 1);
  printf("%s\n", rc);

  BOOST_CHECK_EQUAL('e', rc[0]);

  lseek(mpz_get_si(f), 0, SEEK_SET);
  ::read(fd, rc, 2);
  printf("%s\n", rc);

  BOOST_CHECK_EQUAL(0, strncmp("he", &(rc[0]), 2));
  */
}

BOOST_AUTO_TEST_SUITE_END()
