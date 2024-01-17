#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <mpfr.h>
#include <time.h>

#include "runtime/alloc.h"
#include "runtime/header.h"

#include "fcntl.h"
#include "unistd.h"

#define KCHAR char

void *constructCompositePattern(uint32_t tag, std::vector<void *> &arguments) {
  return nullptr;
}

extern "C" {

char kompiled_directory[] = "some/test/directory/path";

#define GETTAG(symbol) "Lbl'Hash'" #symbol "{}"
#define ERRBLOCK(tag) (uint64_t)(leaf_block(tag))
#define NUM_SYMBOLS 8
char const *symbols[NUM_SYMBOLS]
    = {GETTAG(EOF),
       GETTAG(ENOENT),
       GETTAG(EBADF),
       "inj{SortInt{}, SortIOInt{}}",
       "inj{SortIOError{}, SortKItem{}}",
       "inj{SortString{}, SortKItem{}}",
       "kseq{}",
       GETTAG(systemResult)};

uint32_t const first_inj_tag = 3;
uint32_t const last_inj_tag = 5;

uint32_t getTagForSymbolName(char const *s) {
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (0 == strcmp(symbols[i], s)) {
      return i;
    }
  }

  return 0;
}

struct blockheader getBlockHeaderForSymbol(uint32_t tag) {
  return blockheader{tag};
}

bool during_gc() {
  return false;
}

void add_hash64(void *, uint64_t) { }

void flush_IO_logs();
string *makeString(const KCHAR *, int64_t len = -1);
blockheader header_err();
block *hook_IO_open(string *filename, string *control);
block *hook_IO_tell(mpz_t i);
block *hook_IO_getc(mpz_t i);
block *hook_IO_read(mpz_t i, mpz_t len);
block *hook_IO_close(mpz_t i);
block *hook_IO_seek(mpz_t i, mpz_t loc);
block *hook_IO_seekEnd(mpz_t i, mpz_t loc);
block *hook_IO_putc(mpz_t i, mpz_t c);
block *hook_IO_write(mpz_t i, string *str);
block *hook_IO_lock(mpz_t i, mpz_t len);
block *hook_IO_unlock(mpz_t i, mpz_t len);
block *hook_IO_log(string *path, string *msg);
block *hook_IO_system(string *cmd);
mpz_ptr hook_IO_time(void);
string *hook_KREFLECTION_kompiledDir(void);
list hook_KREFLECTION_argv();

extern int llvm_backend_argc;
extern char const **llvm_backend_argv;

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
  int fd = ::open(
      "test.txt", O_RDWR | O_CREAT | O_TRUNC,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  BOOST_CHECK_EQUAL(::write(fd, "hello world!", 12), 12);
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

  block *b1 = hook_IO_open(realFilename, control);
  BOOST_CHECK_EQUAL(
      b1->h.hdr, getBlockHeaderForSymbol(
                     getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                     .hdr);
  BOOST_CHECK(0 < mpz_cmp_si((mpz_ptr) * (b1->children), 0));

  block *b2 = hook_IO_open(fakeFilename, control);
  BOOST_CHECK_EQUAL(
      b2->h.hdr, getBlockHeaderForSymbol(
                     getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
                     .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (b2->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(ENOENT))));
}

BOOST_AUTO_TEST_CASE(tell) {
  mpz_t f;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  block *b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      0,
      mpz_cmp_si((mpz_ptr) * (b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), 5, SEEK_CUR);

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), 5));

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      0,
      mpz_cmp_si((mpz_ptr) * (b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  lseek(mpz_get_si(f), -4, SEEK_CUR);

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), 1));

  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      0,
      mpz_cmp_si((mpz_ptr) * (b->children), lseek(mpz_get_si(f), 0, SEEK_CUR)));

  ::close(fd);

  mpz_set_si(f, -1);
  b = hook_IO_tell(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (b->children), ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(getc) {
  mpz_t f;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  block *b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), int('h')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), int('e')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), int('l')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), int('l')));

  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (b->children), int('o')));

  ::lseek(fd, 0, SEEK_END);
  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
                    .hdr);
  char const *temp = GETTAG(EOF);
  BOOST_CHECK(std::string(temp) != "");
  BOOST_CHECK_EQUAL(
      (uint64_t) * (b->children), ERRBLOCK(getTagForSymbolName(GETTAG(EOF))));

  ::close(fd);

  mpz_set_si(f, -1);
  b = hook_IO_getc(f);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (b->children), ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(read) {
  mpz_t f;
  mpz_t length;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init_set_si(length, 6);

  block *b = hook_IO_read(f, length);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortString{}, SortIOString{}}"))
                    .hdr);
  string *str = (string *)*(b->children);

  BOOST_CHECK_EQUAL(0, strncmp(str->data, "hello ", 6));

  b = hook_IO_read(f, length);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortString{}, SortIOString{}}"))
                    .hdr);
  str = (string *)*(b->children);
  BOOST_CHECK_EQUAL(0, strncmp(str->data, "world!", 6));

  ::lseek(fd, 0, SEEK_END);

  b = hook_IO_read(f, length);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortString{}, SortIOString{}}"))
                    .hdr);
  str = (string *)*(b->children);
  BOOST_CHECK_EQUAL(0, len(str));

  ::close(fd);

  b = hook_IO_read(f, length);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(
                    getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
                    .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (b->children), ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
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
  BOOST_CHECK_EQUAL(EBADF, errno);
  BOOST_CHECK_EQUAL(-1, fcntl(fd2, F_GETFD));
  BOOST_CHECK_EQUAL(EBADF, errno);

  block *b = hook_IO_close(f1);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
  b = hook_IO_close(f2);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
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
  BOOST_CHECK_EQUAL(::read(fd, ret, 5), 5);

  BOOST_CHECK_EQUAL(0, strncmp("howdy", ret, 2));

  ::close(fd);

  block *b = hook_IO_putc(f, c);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
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

  mpz_set_si(f, -1);
  block *b = hook_IO_seek(f, loc);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}
BOOST_AUTO_TEST_CASE(seekEnd) {
  mpz_t f;
  mpz_t loc;

  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init(loc);

  int cur, end;
  int deltas[5] = {-5, 0, -3, -4, -2};

  for (int d : deltas) {
    mpz_set_si(loc, d);
    hook_IO_seekEnd(f, loc);
    cur = lseek(fd, 0, SEEK_CUR);
    end = lseek(fd, d, SEEK_END);
    BOOST_CHECK_EQUAL(cur, end);
  }

  mpz_set_si(f, -1);
  block *b = hook_IO_seekEnd(f, loc);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(write) {
  mpz_t f;
  string *msg = makeString("This is a test message\n");
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);

  hook_IO_write(f, msg);

  FILE *file = fopen("test.txt", "r");
  char buf[23];
  BOOST_CHECK_EQUAL(fread(buf, sizeof(char), 23, file), 23);
  fclose(file);

  BOOST_CHECK_EQUAL(0, strncmp(buf, "This is a test message\n", 23));

  mpz_set_si(f, -1);
  block *b = hook_IO_write(f, msg);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(lock) {
  mpz_t f, len;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init_set_si(len, 12);

  block *b = hook_IO_lock(f, len);

  struct flock lockp = {0};
  lockp.l_type = F_WRLCK;
  lockp.l_whence = SEEK_CUR;
  lockp.l_start = 0;
  lockp.l_len = 12;
  fcntl(fd, F_SETLK, &lockp);

  BOOST_CHECK(lockp.l_type != F_UNLCK);

  mpz_set_si(f, -1);
  b = hook_IO_lock(f, len);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(unlock) {
  mpz_t f, len;
  int fd = overwriteTestFile();
  mpz_init_set_si(f, fd);
  mpz_init_set_si(len, 12);

  block *b = hook_IO_unlock(f, len);

  struct flock lockp = {0};
  lockp.l_type = F_WRLCK;
  lockp.l_whence = SEEK_CUR;
  lockp.l_start = 0;
  lockp.l_len = 12;
  fcntl(fd, F_SETLK, &lockp);

  BOOST_CHECK(lockp.l_type != F_UNLCK);

  mpz_set_si(f, -1);
  b = hook_IO_unlock(f, len);
  BOOST_CHECK_EQUAL(
      b->h.hdr, getBlockHeaderForSymbol(getTagForSymbolName("kseq{}")).hdr);
  BOOST_CHECK_EQUAL(
      ((block *)*(b->children))->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"))
          .hdr);
  BOOST_CHECK_EQUAL(
      (uint64_t) * (((block *)*(b->children))->children),
      ERRBLOCK(getTagForSymbolName(GETTAG(EBADF))));
}

BOOST_AUTO_TEST_CASE(log) {
  std::string strPath = "logFile";
  std::string strMsg = "Log1\nLog2\n";
  string *path = makeString(strPath.c_str());
  string *msg = makeString(strMsg.c_str());

  hook_IO_log(path, msg);
  msg = makeString("Log3\n");
  hook_IO_log(path, msg);
  flush_IO_logs();

  std::string PID = std::to_string(getpid());
  FILE *f = fopen((PID + "_" + strPath).c_str(), "r");
  char buf[15];
  BOOST_CHECK_EQUAL(fread(buf, sizeof(char), 15, f), 15);
  fclose(f);

  BOOST_CHECK_EQUAL(0, strncmp(buf, (strMsg + "Log3\n").c_str(), 10));
}

BOOST_AUTO_TEST_CASE(system) {
  std::string command = "echo \"hello\"";
  string *cmd = makeString(command.c_str());

  block *ret = hook_IO_system(cmd);

  BOOST_CHECK(ret != NULL);
  BOOST_CHECK(ret->children != NULL);
  BOOST_CHECK((ret->children + 1) != NULL);
  BOOST_CHECK((ret->children + 2) != NULL);

  BOOST_CHECK_EQUAL(
      ret->h.hdr,
      getBlockHeaderForSymbol(getTagForSymbolName(GETTAG(systemResult))).hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (ret->children), 0));

  string *out = (string *)*(ret->children + 1);
  BOOST_CHECK_EQUAL(0, strncmp(out->data, "hello\n", 6));
  BOOST_CHECK_EQUAL(6, len(out));

  /* Check if shell is available */
  command = "";
  cmd = makeString(command.c_str());
  ret = hook_IO_system(cmd);

  BOOST_CHECK(ret != NULL);
  BOOST_CHECK(ret->children != NULL);
  BOOST_CHECK((ret->children + 1) != NULL);
  BOOST_CHECK((ret->children + 2) != NULL);
  BOOST_CHECK_EQUAL(
      ret->h.hdr,
      getBlockHeaderForSymbol(getTagForSymbolName(GETTAG(systemResult))).hdr);
  BOOST_CHECK(mpz_cmp_si((mpz_ptr) * (ret->children), 0) > 0);

  /* Execute program that segfaults */
  command = "./IOTest 1";
  cmd = makeString(command.c_str());
  ret = hook_IO_system(cmd);
  out = (string *)*(ret->children + 1);
  BOOST_CHECK(ret != NULL);
  BOOST_CHECK(ret->children != NULL);
  BOOST_CHECK((ret->children + 1) != NULL);
  BOOST_CHECK((ret->children + 2) != NULL);
  BOOST_CHECK_EQUAL(
      ret->h.hdr,
      getBlockHeaderForSymbol(getTagForSymbolName(GETTAG(systemResult))).hdr);
  // this assertion fails on some platforms
  // BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) *(ret->children), 139));

  /* Execute program that prints to stderr */
  command = "./IOTest";
  cmd = makeString(command.c_str());
  ret = hook_IO_system(cmd);
  out = (string *)*(ret->children + 1);
  BOOST_CHECK(ret != NULL);
  BOOST_CHECK(ret->children != NULL);
  BOOST_CHECK((ret->children + 1) != NULL);
  BOOST_CHECK((ret->children + 2) != NULL);
  BOOST_CHECK_EQUAL(
      ret->h.hdr,
      getBlockHeaderForSymbol(getTagForSymbolName(GETTAG(systemResult))).hdr);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si((mpz_ptr) * (ret->children), 0));

  string *err = (string *)*(ret->children + 2);
  BOOST_CHECK_EQUAL(0, strncmp(err->data, "Error", 5));
  BOOST_CHECK_EQUAL(5, len(err));
}

BOOST_AUTO_TEST_CASE(time) {
  auto mpz = hook_IO_time();
  time_t tt = mpz_get_si(mpz);
  time_t time2 = ::time(NULL);
  BOOST_CHECK(time2 >= tt);
  BOOST_CHECK(time2 - tt < 5);
  BOOST_CHECK(tt >= 1573756117);
}

BOOST_AUTO_TEST_CASE(kompiledDir) {
  auto dir = hook_KREFLECTION_kompiledDir();
  auto len = strlen(kompiled_directory);
  BOOST_CHECK_EQUAL(0, memcmp(dir->data, kompiled_directory, len));

  auto returned_len = strnlen(dir->data, len);
  BOOST_CHECK_EQUAL(returned_len, len);
}

BOOST_AUTO_TEST_CASE(argv) {
  char const *argv[] = {"./a.out", "--help", nullptr};
  llvm_backend_argv = argv;
  llvm_backend_argc = 2;
  list ret = hook_KREFLECTION_argv();
  BOOST_CHECK(ret.size() == 2);

  BOOST_CHECK(ret[0].elem != nullptr);
  BOOST_CHECK_EQUAL(
      ret[0].elem->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortString{}, SortKItem{}}"))
          .hdr);
  string *arg0 = (string *)*(ret[0].elem->children);
  BOOST_CHECK_EQUAL(0, strncmp(arg0->data, argv[0], strlen(argv[0])));
  BOOST_CHECK_EQUAL(strlen(argv[0]), len(arg0));

  BOOST_CHECK(ret[1].elem != nullptr);
  BOOST_CHECK_EQUAL(
      ret[1].elem->h.hdr,
      getBlockHeaderForSymbol(
          getTagForSymbolName("inj{SortString{}, SortKItem{}}"))
          .hdr);
  string *arg1 = (string *)*(ret[1].elem->children);
  BOOST_CHECK_EQUAL(0, strncmp(arg1->data, argv[1], strlen(argv[1])));
  BOOST_CHECK_EQUAL(strlen(argv[1]), len(arg1));
}

BOOST_AUTO_TEST_SUITE_END()
