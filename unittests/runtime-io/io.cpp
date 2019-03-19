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
  
  struct blockheader getBlockHeaderForSymbol(uint32_t tag) {
    return blockheader {0};
  }

  void add_hash64(void*, uint64_t) {}

  string * makeString(const KCHAR *, int64_t len = -1);
  blockheader header_err();
  block * hook_IO_open(string * filename, string * control);
  block * hook_IO_tell(mpz_t i);
  block * hook_IO_getc(mpz_t i);
  block * hook_IO_read(mpz_t i, mpz_t len);
  block * hook_IO_close(mpz_t i);
  block * hook_IO_seek(mpz_t i, mpz_t loc);
  block * hook_IO_seekEnd(mpz_t i, mpz_t loc);
  block * hook_IO_putc(mpz_t i, mpz_t c);
  block * hook_IO_write(mpz_t i, string * str);

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

int overwriteTestFile() {
  int fd = ::open("test.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  ::write(fd, "hello world!", 12);
  ::lseek(fd, 0, SEEK_SET);
  return fd;
}

BOOST_AUTO_TEST_SUITE(IoTest)

BOOST_AUTO_TEST_CASE(open) {
  int fd = overwriteTestFile();
  ::close(fd);

  auto realFilename = makeString("test.txt");
  auto fakeFilename = makeString("testFake.txt");
  auto control = makeString("r");

  block * b1 = hook_IO_open(realFilename, control);
  BOOST_CHECK(0 < mpz_cmp_si((mpz_ptr) *(b1->children), 0));

  block * b2 = hook_IO_open(fakeFilename, control);
  BOOST_CHECK_EQUAL(errno, EACCES);
  BOOST_CHECK_EQUAL(b2->h.hdr, header_err().hdr);
}

BOOST_AUTO_TEST_CASE(tell) {
  mpz_t f;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  block * b = hook_IO_tell(f);

  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), 5, SEEK_CUR);

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), 5));

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), -4, SEEK_CUR);

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), 1));

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  ::close(fd);
}

BOOST_AUTO_TEST_CASE(getc) {
  mpz_t f;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  block * b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), int('h')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), int('e')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), int('l')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), int('l')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(b->children), int('o')));

  ::close(fd);
}

BOOST_AUTO_TEST_CASE(read) {
  mpz_t f;
  mpz_t len;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init_set_si(len, 6);

  block * b = hook_IO_read(f, len);
  string * str = (string *) *(b->children);

  BOOST_CHECK_EQUAL(0, strncmp(str->data, "hello ", 6));

  b = hook_IO_read(f, len);
  str = (string *) *(b->children);
  BOOST_CHECK_EQUAL(0, strncmp(str->data, "world!", 6));

  ::close(fd);
}

BOOST_AUTO_TEST_CASE(close) {
  mpz_t f1;
  mpz_t f2;
  int fd1 = overwriteTestFile();
  mpz_init_set_si(f1, fd1);
  int fd2 = overwriteTestFile();
  mpz_init_set_si(f2, fd2);

  hook_IO_close(f1);
  hook_IO_close(f2);

  BOOST_CHECK_EQUAL(-1, fcntl(fd1, F_GETFD));
  BOOST_CHECK_EQUAL(-1, fcntl(fd2, F_GETFD));
}

BOOST_AUTO_TEST_CASE(putc) {
  mpz_t f;
  mpz_t c;

  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  lseek(fd, 0, SEEK_SET);

  mpz_init_set_si(c, int('h'));
  hook_IO_putc(f, c);
  mpz_set_si(c, int('o'));
  hook_IO_putc(f, c);
  mpz_set_si(c, int('w'));
  hook_IO_putc(f, c);
  mpz_set_si(c, int('d'));
  hook_IO_putc(f, c);
  mpz_set_si(c, int('y'));
  hook_IO_putc(f, c);

  char ret[5];

  lseek(fd, 0, SEEK_SET);
  ::read(fd, ret, 5);

  BOOST_CHECK_EQUAL(0, strncmp("howdy", ret, 2));

  ::close(fd);
}

BOOST_AUTO_TEST_CASE(seek) {
  mpz_t f;
  mpz_t loc;
  mpz_init(loc);

  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  int deltas[5] = {3, 0, 2, 5, 1};

  for (int d : deltas) {
    mpz_set_si(loc, d);
    hook_IO_seek(f, loc);
    BOOST_CHECK_EQUAL(d, lseek(fd, 0, SEEK_CUR));
  }
}
BOOST_AUTO_TEST_CASE(seekEnd) {
  mpz_t f;
  mpz_t loc;

  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init(loc);

  int cur, end;
  int deltas[5] = {-5, 0, -3, -4, -2};

  for (int d: deltas) {
    mpz_set_si(loc, d);
    hook_IO_seekEnd(f, loc);
    cur = lseek(fd, 0, SEEK_CUR);
    end = lseek(fd, d, SEEK_END);
    BOOST_CHECK_EQUAL(cur, end);
  }
}
BOOST_AUTO_TEST_CASE(write) {
}
BOOST_AUTO_TEST_CASE(lock) {
}
BOOST_AUTO_TEST_CASE(unlock) {
}

BOOST_AUTO_TEST_SUITE_END()
