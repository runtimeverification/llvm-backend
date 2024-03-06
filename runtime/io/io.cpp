#include <cerrno>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <gmp.h>
#include <iostream>
#include <libgen.h>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fmt/format.h>

#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
#define GETTAG(symbol) "Lbl'Hash'" #symbol "{}"
#define IOBUFSIZE 1024

extern char kompiled_directory;

char *get_terminated_string(string *str);

static blockheader kseq_header = {
    get_block_header_for_symbol((uint64_t)get_tag_for_symbol_name("kseq{}"))};

static std::map<std::string, std::string> log_files;

static block *block_errno() {
  char const *err_str = nullptr;
  switch (errno) {
  case EOF: err_str = GETTAG(EOF); break;
  case E2BIG: err_str = GETTAG(E2BIG); break;
  case EACCES: err_str = GETTAG(EACCES); break;
  case EAGAIN: err_str = GETTAG(EAGAIN); break;
  case EBADF: err_str = GETTAG(EBADF); break;
  case EBUSY: err_str = GETTAG(EBUSY); break;
  case ECHILD: err_str = GETTAG(ECHILD); break;
  case EDEADLK: err_str = GETTAG(EDEADLK); break;
  case EDOM: err_str = GETTAG(EDOM); break;
  case EEXIST: err_str = GETTAG(EEXIST); break;
  case EFAULT: err_str = GETTAG(EFAULT); break;
  case EFBIG: err_str = GETTAG(EFBIG); break;
  case EINTR: err_str = GETTAG(EINTR); break;
  case EINVAL: err_str = GETTAG(EINVAL); break;
  case EIO: err_str = GETTAG(EIO); break;
  case EISDIR: err_str = GETTAG(EISDIR); break;
  case EMFILE: err_str = GETTAG(EMFILE); break;
  case EMLINK: err_str = GETTAG(EMLINK); break;
  case ENAMETOOLONG: err_str = GETTAG(ENAMETOOLONG); break;
  case ENFILE: err_str = GETTAG(ENFILE); break;
  case ENODEV: err_str = GETTAG(ENODEV); break;
  case ENOENT: err_str = GETTAG(ENOENT); break;
  case ENOEXEC: err_str = GETTAG(ENOEXEC); break;
  case ENOLCK: err_str = GETTAG(ENOLCK); break;
  case ENOMEM: err_str = GETTAG(ENOMEM); break;
  case ENOSPC: err_str = GETTAG(ENOSPC); break;
  case ENOSYS: err_str = GETTAG(ENOSYS); break;
  case ENOTDIR: err_str = GETTAG(ENOTDIR); break;
  case ENOTEMPTY: err_str = GETTAG(ENOTEMPTY); break;
  case ENOTTY: err_str = GETTAG(ENOTTY); break;
  case ENXIO: err_str = GETTAG(ENXIO); break;
  case EPERM: err_str = GETTAG(EPERM); break;
  case EPIPE: err_str = GETTAG(EPIPE); break;
  case ERANGE: err_str = GETTAG(ERANGE); break;
  case EROFS: err_str = GETTAG(EROFS); break;
  case ESPIPE: err_str = GETTAG(ESPIPE); break;
  case ESRCH: err_str = GETTAG(ESRCH); break;
  case EXDEV: err_str = GETTAG(EXDEV); break;
#if EWOULDBLOCK != EAGAIN
  case EWOULDBLOCK: errStr = GETTAG(EWOULDBLOCK); break;
#endif
  case EINPROGRESS: err_str = GETTAG(EINPROGRESS); break;
  case EALREADY: err_str = GETTAG(EALREADY); break;
  case ENOTSOCK: err_str = GETTAG(ENOTSOCK); break;
  case EDESTADDRREQ: err_str = GETTAG(EDESTADDRREQ); break;
  case EMSGSIZE: err_str = GETTAG(EMSGSIZE); break;
  case EPROTOTYPE: err_str = GETTAG(EPROTOTYPE); break;
  case ENOPROTOOPT: err_str = GETTAG(ENOPROTOOPT); break;
  case EPROTONOSUPPORT: err_str = GETTAG(EPROTONOSUPPORT); break;
  case ESOCKTNOSUPPORT: err_str = GETTAG(ESOCKTNOSUPPORT); break;
  case EOPNOTSUPP: err_str = GETTAG(EOPNOTSUPP); break;
  case EPFNOSUPPORT: err_str = GETTAG(EPFNOSUPPORT); break;
  case EAFNOSUPPORT: err_str = GETTAG(EAFNOSUPPORT); break;
  case EADDRINUSE: err_str = GETTAG(EADDRINUSE); break;
  case EADDRNOTAVAIL: err_str = GETTAG(EADDRNOTAVAIL); break;
  case ENETDOWN: err_str = GETTAG(ENETDOWN); break;
  case ENETUNREACH: err_str = GETTAG(ENETUNREACH); break;
  case ENETRESET: err_str = GETTAG(ENETRESET); break;
  case ECONNABORTED: err_str = GETTAG(ECONNABORTED); break;
  case ECONNRESET: err_str = GETTAG(ECONNRESET); break;
  case ENOBUFS: err_str = GETTAG(ENOBUFS); break;
  case EISCONN: err_str = GETTAG(EISCONN); break;
  case ENOTCONN: err_str = GETTAG(ENOTCONN); break;
  case ESHUTDOWN: err_str = GETTAG(ESHUTDOWN); break;
  case ETOOMANYREFS: err_str = GETTAG(ETOOMANYREFS); break;
  case ETIMEDOUT: err_str = GETTAG(ETIMEDOUT); break;
  case ECONNREFUSED: err_str = GETTAG(ECONNREFUSED); break;
  case EHOSTDOWN: err_str = GETTAG(EHOSTDOWN); break;
  case EHOSTUNREACH: err_str = GETTAG(EHOSTUNREACH); break;
  case ELOOP: err_str = GETTAG(ELOOP); break;
  case EOVERFLOW: err_str = GETTAG(EOVERFLOW); break;
  default:
    auto *ret_block
        = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(mpz_ptr)));
    ret_block->h = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("Lbl'Hash'unknownIOError{}"));
    mpz_t err;
    mpz_init_set_si(err, errno);
    mpz_ptr p = move_int(err);
    memcpy(ret_block->children, &p, sizeof(mpz_ptr));
    return ret_block;
  }

  return leaf_block(get_tag_for_symbol_name(err_str));
}

static blockheader header_int() {
  static blockheader header = {(uint64_t)-1};

  if (header.hdr == -1) {
    header = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("inj{SortInt{}, SortIOInt{}}"));
  }

  return header;
}

blockheader header_err() {
  static blockheader header = {(uint64_t)-1};

  if (header.hdr == -1) {
    header = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("inj{SortIOError{}, SortKItem{}}"));
  }

  return header;
}

static blockheader header_string() {
  static blockheader header = {(uint64_t)-1};

  if (header.hdr == -1) {
    header = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("inj{SortString{}, SortIOString{}}"));
  }

  return header;
}

static inline block *get_k_seq_error_block() {
  block *err = block_errno();
  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + 2 * sizeof(block *)));
  auto *inj = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  ret_block->h = kseq_header;
  inj->h = header_err();
  memcpy(inj->children, &err, sizeof(block *));
  memcpy(ret_block->children, &inj, sizeof(block *));

  auto *dot_k_block = dot_k();
  memcpy(ret_block->children + 1, &dot_k_block, sizeof(block *));
  return ret_block;
}

static inline block *get_inj_error_block() {
  block *p = block_errno();
  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
  ret_block->h = header_err();
  memcpy(ret_block->children, &p, sizeof(block *));
  return ret_block;
}

#define MODE_R 1
#define MODE_W 2
#define MODE_A 4
#define MODE_E 8
#define MODE_X 16
#define MODE_B 32
#define MODE_P 64

static int get_file_modes(string *modes) {
  int flags = 0;
  int length = len(modes);

  if (length <= 0) {
    return -1;
  }

  switch (modes->data[0]) {
  case 'r': flags |= MODE_R; break;
  case 'w': flags |= MODE_W; break;
  case 'a': flags |= MODE_A; break;
  default: return -1;
  }

  for (int i = 1; i < length; i++) {
    switch (modes->data[i]) {
    case 'e': flags |= (flags & MODE_E) ? -1 : MODE_E; break;
    case 'x': flags |= (flags & MODE_X) ? -1 : MODE_X; break;
    case 'b': flags |= (flags & MODE_B) ? -1 : MODE_B; break;
    case '+': flags |= (flags & MODE_P) ? -1 : MODE_P; break;
    default: return -1;
    }
  }

  return (flags < 0) ? -1 : flags;
}

SortIOInt hook_IO_open(SortString filename, SortString control) {
  int flags = 0;
  int access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int modes = get_file_modes(control);
  int fd = 0;
  mpz_t result;

  if (-1 != modes) {
    switch (modes & 7) {
    case MODE_R: flags = (modes & MODE_P) ? O_RDWR : O_RDONLY; break;
    case MODE_W:
      flags = (modes & MODE_P) ? O_RDWR : O_WRONLY;
      flags |= O_TRUNC | O_CREAT;
      break;
    case MODE_A:
      flags = (modes & MODE_P) ? O_RDWR : O_WRONLY;
      flags |= O_APPEND | O_CREAT;
      break;
    }

    if (modes & MODE_E) {
      flags |= O_CLOEXEC;
    }

    if (modes & MODE_X) {
      flags |= O_EXCL;
    }

    if (modes & MODE_B) {
    }

    char *f = get_terminated_string(filename);
    fd = open(f, flags, access); // NOLINT(*-vararg)
  } else {
    errno = EINVAL;
    fd = -1;
  }

  if (-1 == fd) {
    return get_inj_error_block();
  }

  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(mpz_ptr)));
  ret_block->h = header_int();
  mpz_init_set_si(result, fd);
  mpz_ptr p = move_int(result);
  memcpy(ret_block->children, &p, sizeof(mpz_ptr));
  return ret_block;
}

SortIOInt hook_IO_tell(SortInt i) {
  if (!mpz_fits_sint_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large for int: {}", int_to_string(i));
  }

  int fd = mpz_get_si(i);
  off_t loc = lseek(fd, 0, SEEK_CUR);

  if (-1 == loc) {
    return get_inj_error_block();
  }

  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(mpz_ptr)));
  ret_block->h = header_int();
  mpz_t result;
  mpz_init_set_si(result, (long)loc);
  mpz_ptr p = move_int(result);
  memcpy(ret_block->children, &p, sizeof(mpz_ptr));
  return ret_block;
}

SortIOInt hook_IO_getc(SortInt i) {
  if (!mpz_fits_sint_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large for int: {}", int_to_string(i));
  }

  int fd = mpz_get_si(i);
  char c = 0;
  ssize_t ret = read(fd, &c, sizeof(char));

  if (ret == 0) {
    block *p = leaf_block(get_tag_for_symbol_name(GETTAG(EOF)));
    auto *ret_block
        = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(block *)));
    ret_block->h = header_err();
    memcpy(ret_block->children, &p, sizeof(block *));
    return ret_block;
  }

  if (ret == -1) {
    return get_inj_error_block();
  }

  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(mpz_ptr)));
  ret_block->h = header_int();
  mpz_t result;
  mpz_init_set_si(result, (int)c);
  mpz_ptr p = move_int(result);
  memcpy(ret_block->children, &p, sizeof(mpz_ptr));
  return ret_block;
}

SortIOString hook_IO_read(SortInt i, SortInt len) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_ulong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, len={}", int_to_string(i), int_to_string(len));
  }

  int fd = mpz_get_si(i);
  size_t length = mpz_get_ui(len);

  auto *result
      = static_cast<string *>(kore_alloc_token(sizeof(string) + length));
  int bytes = read(fd, &(result->data), length);

  if (-1 == bytes) {
    return get_inj_error_block();
  }

  result = static_cast<string *>(kore_resize_last_alloc(
      result, sizeof(string) + bytes, sizeof(string) + length));
  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(string *)));
  ret_block->h = header_string();
  init_with_len(result, bytes);
  memcpy(ret_block->children, &result, sizeof(string *));
  return ret_block;
}

SortK hook_IO_close(SortInt i) {
  if (!mpz_fits_sint_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large for int: {}", int_to_string(i));
  }

  int fd = mpz_get_si(i);
  int ret = close(fd);

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_seek(SortInt i, SortInt loc) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, loc={}", int_to_string(i), int_to_string(loc));
  }

  int fd = mpz_get_si(i);
  off_t l = mpz_get_si(loc);
  int ret = lseek(fd, l, SEEK_SET);

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_seekEnd(SortInt i, SortInt loc) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, loc={}", int_to_string(i), int_to_string(loc));
  }

  int fd = mpz_get_si(i);
  off_t l = mpz_get_si(loc);
  int ret = lseek(fd, l, SEEK_END);

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_putc(SortInt i, SortInt c) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(c)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, c={}", int_to_string(i), int_to_string(c));
  }

  int fd = mpz_get_si(i);
  int ch = mpz_get_si(c);
  int ret = write(fd, &ch, 1);

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_write(SortInt i, SortString str) {
  if (!mpz_fits_sint_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large for int: {}", int_to_string(i));
  }

  int fd = mpz_get_si(i);
  int ret = write(fd, str->data, len(str));

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_lock(SortInt i, SortInt len) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, len={}", int_to_string(i), int_to_string(len));
  }

  int fd = mpz_get_si(i);
  off_t l = mpz_get_si(len);

  struct flock lockp = {0};
  lockp.l_type = F_WRLCK;
  lockp.l_whence = SEEK_CUR;
  lockp.l_start = 0;
  lockp.l_len = l;
  int ret = fcntl(fd, F_SETLKW, &lockp); // NOLINT(*-vararg)

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_unlock(SortInt i, SortInt len) {
  if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Arg too large: i={}, len={}", int_to_string(i), int_to_string(len));
  }

  int fd = mpz_get_si(i);
  off_t l = mpz_get_si(len);

  struct flock lockp = {0};
  lockp.l_type = F_UNLCK;
  lockp.l_whence = SEEK_CUR;
  lockp.l_start = 0;
  lockp.l_len = l;
  int ret = fcntl(fd, F_SETLKW, &lockp); // NOLINT(*-vararg)

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortK hook_IO_remove(SortString path) {
  char *p = get_terminated_string(path);

  int ret = unlink(p);
  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

SortIOInt hook_IO_accept(SortInt sock) {
  if (!mpz_fits_sint_p(sock)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large: {}", int_to_string(sock));
  }

  int fd = mpz_get_si(sock);
  int clientsock = accept(fd, nullptr, nullptr);

  if (clientsock == -1) {
    return get_inj_error_block();
  }

  auto *ret_block
      = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(mpz_ptr)));
  ret_block->h = header_int();

  mpz_t result;
  mpz_init_set_si(result, clientsock);
  mpz_ptr p = move_int(result);
  memcpy(ret_block->children, &p, sizeof(mpz_ptr));
  return ret_block;
}

SortK hook_IO_shutdownWrite(SortInt sock) {
  if (!mpz_fits_sint_p(sock)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large: {}", int_to_string(sock));
  }

  int fd = mpz_get_si(sock);
  int ret = shutdown(fd, SHUT_WR);

  if (ret == -1) {
    return get_k_seq_error_block();
  }

  return dot_k();
}

void flush_io_logs() {
  std::string pid = std::to_string(getpid());
  for (auto const &log : log_files) {
    std::string path_str = log.first;
    std::string msg = log.second;
    size_t length = path_str.length();
    char *path1 = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(path1, path_str.c_str(), length + 1);
    path1[length] = '\0';
    char *path2 = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(path2, path_str.c_str(), length + 1);
    path2[length] = '\0';
    char *dir = dirname(path1);
    if (getenv("K_LOG_DIR")) {
      dir = getenv("K_LOG_DIR");
    }
    char fulldir[PATH_MAX];
    if (!realpath(dir, fulldir)) {
      abort();
    }
    char *base = basename(path2);
    std::string prefix;
    if (getenv("K_LOG_PREFIX")) {
      prefix = getenv("K_LOG_PREFIX");
    }
    auto full_path = fmt::format("{}/{}{}_{}", fulldir, prefix, pid, base);
    FILE *f = fopen(full_path.c_str(), "a+");
    fwrite(msg.c_str(), sizeof(char), msg.length(), f);
    fclose(f);
    free(path1);
    free(path2);
  }
}

SortK hook_IO_log(SortString path, SortString msg) {
  char *p = get_terminated_string(path);
  char *m = get_terminated_string(msg);

  static bool flush_registered = false;
  if (!flush_registered) {
    atexit(&flush_io_logs);
    flush_registered = true;
  }

  log_files[p].append(m);

  return dot_k();
}

SortK hook_IO_logString(SortString msg) {
  char *m = get_terminated_string(msg);
  std::cerr << m << std::endl;
  return dot_k();
}

block *hook_KREFLECTION_parseKAST(string *kast) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.parseKast");
}

block *hook_KREFLECTION_configuration() {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.configuration");
}

string *hook_KREFLECTION_getenv(string *str) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.getenv");
}

string *hook_KREFLECTION_sort(block *k) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.sort");
}

block *hook_KREFLECTION_getKLabel(block *k) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.getKLabel");
}

block *hook_KREFLECTION_fresh(string *str) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: KREFLECTION.fresh");
}

string *hook_KREFLECTION_kompiledDir(void) {
  auto *str_ptr = &kompiled_directory;
  auto len = strlen(str_ptr);
  auto *ret = static_cast<string *>(kore_alloc_token(sizeof(string) + len));
  memcpy(ret->data, str_ptr, len);
  init_with_len(ret, len);
  return ret;
}

int llvm_backend_argc = 0;
char const **llvm_backend_argv = nullptr;

list hook_KREFLECTION_argv() {
  if (!llvm_backend_argv) {
    KLLVM_HOOK_INVALID_ARGUMENT("KREFLECTION.argv: no arguments given");
  }

  list l{};

  for (int i = 0; i < llvm_backend_argc; i++) {
    stringbuffer *buf = hook_BUFFER_empty();
    buf = hook_BUFFER_concat_raw(
        buf, llvm_backend_argv[i], strlen(llvm_backend_argv[i]));
    SortString str = hook_BUFFER_toString(buf);
    auto *b
        = static_cast<block *>(kore_alloc(sizeof(block) + sizeof(SortString)));
    b->h = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("inj{SortString{}, SortKItem{}}"));
    memcpy(b->children, &str, sizeof(SortString));
    l = l.push_back(k_elem(b));
  }

  return l;
}

SortIOFile hook_IO_mkstemp(SortString filename) {
  char *temp = get_terminated_string(filename);
  int ret = mkstemp(temp);

  if (ret == -1) {
    return get_inj_error_block();
  }

  auto *ret_block = static_cast<block *>(
      kore_alloc(sizeof(block) + sizeof(string *) + sizeof(mpz_ptr)));

  mpz_t result;
  mpz_init_set_si(result, ret);
  mpz_ptr p = move_int(result);
  size_t length = len(filename);
  auto *ret_string = static_cast<string *>(
      kore_alloc_token(sizeof(string) + sizeof(char) * length));
  memcpy(ret_string->data, temp, sizeof(char) * length);
  init_with_len(ret_string, length);
  memcpy(ret_block->children, &ret_string, sizeof(string *));
  memcpy(ret_block->children + 1, &p, sizeof(mpz_ptr));
  ret_block->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name(GETTAG(tempFile)));

  return ret_block;
}

// NOLINTNEXTLINE(*-cognitive-complexity)
SortKItem hook_IO_system(SortString cmd) {
  pid_t pid = 0;
  int ret = 0;
  int out[2];
  int err[2];
  stringbuffer *out_buffer = hook_BUFFER_empty();
  stringbuffer *err_buffer = hook_BUFFER_empty();
  char buf[IOBUFSIZE];

  // NOLINTNEXTLINE(*-assignment-in-if-condition)
  if (pipe(out) == -1 || pipe(err) == -1 || (pid = fork()) == -1) {
    return get_k_seq_error_block();
  }

  if (pid == 0) {
    dup2(out[1], STDOUT_FILENO);
    close(out[0]);
    close(out[1]);
    dup2(err[1], STDERR_FILENO);
    close(err[0]);
    close(err[1]);

    if (len(cmd) > 0) {
      char *command = get_terminated_string(cmd);

      // NOLINTNEXTLINE(*-vararg)
      ret = execl("/bin/sh", "/bin/sh", "-c", command, nullptr);
      ret == -1 ? exit(127) : exit(0);
    } else {
      ret = system(nullptr);
      exit(ret);
    }
  }

  close(out[1]);
  close(err[1]);

  fd_set read_fds;
  fd_set ready_fds;
  FD_ZERO(&read_fds);
  FD_SET(out[0], &read_fds);
  FD_SET(err[0], &read_fds);

  int done = 0;

  while (done < 2) {
    ready_fds = read_fds;
    if (select(FD_SETSIZE, &ready_fds, nullptr, nullptr, nullptr) == -1) {
      return get_k_seq_error_block();
    }
    if (FD_ISSET(out[0], &ready_fds)) {
      int nread = read(out[0], buf, IOBUFSIZE);
      if (nread == -1) {
        return get_k_seq_error_block();
      }

      if (nread == 0) {
        FD_CLR(out[0], &read_fds);
        done++;
      } else {
        hook_BUFFER_concat_raw(out_buffer, buf, nread);
      }
    }
    if (FD_ISSET(err[0], &ready_fds)) {
      int nread = read(err[0], buf, IOBUFSIZE);
      if (nread == -1) {
        return get_k_seq_error_block();
      }

      if (nread == 0) {
        FD_CLR(err[0], &read_fds);
        done++;
      } else {
        hook_BUFFER_concat_raw(err_buffer, buf, nread);
      }
    }
  }

  waitpid(pid, &ret, 0);
  ret = WEXITSTATUS(ret);

  auto *ret_block = static_cast<block *>(kore_alloc(
      sizeof(block) + sizeof(mpz_ptr) + sizeof(string *) + sizeof(string *)));

  mpz_t result;
  mpz_init_set_si(result, ret);
  mpz_ptr p = move_int(result);
  memcpy(ret_block->children, &p, sizeof(mpz_ptr));

  ret_block->h = get_block_header_for_symbol(
      (uint64_t)get_tag_for_symbol_name(GETTAG(systemResult)));
  string *out_str = nullptr;
  string *err_str = nullptr;
  out_str = hook_BUFFER_toString(out_buffer);
  err_str = hook_BUFFER_toString(err_buffer);
  memcpy(ret_block->children + 1, &out_str, sizeof(string *));
  memcpy(ret_block->children + 2, &err_str, sizeof(string *));

  return ret_block;
}

block *hook_IO_stat(string *path) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: IO.stat");
}

block *hook_IO_lstat(string *path) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: IO.lstat");
}

block *hook_IO_opendir(string *path) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: IO.opendir");
}

SortInt hook_IO_time() {
  mpz_t result;
  mpz_init_set_si(result, time(nullptr));
  return move_int(result);
}
}
