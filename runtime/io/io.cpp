#include <gmp.h>
#include <cstring>
#include <stdexcept>

#include "fcntl.h"
#include "unistd.h"
#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
#define ERRTAG(err) "Lbl'hash'" #err

  mpz_ptr move_int(mpz_t);

  static block dotK = {getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName("dotk")) << 32) | 1), {}};
  static blockheader kseqHeader = {getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("kseq{}"))};

  block * block_errno() {
    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block)));
    switch (errno) {
    case EOF:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EOF))) << 32) | 1);
    case E2BIG:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(E2BIG))) << 32) | 1);
    case EACCES:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EACCES))) << 32) | 1);
    case EAGAIN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EAGAIN))) << 32) | 1);
    case EBADF:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EBADF))) << 32) | 1);
    case EBUSY:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EBUSY))) << 32) | 1);
    case ECHILD:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ECHILD))) << 32) | 1);
    case EDEADLK:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EDEADLK))) << 32) | 1);
    case EDOM:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EDOM))) << 32) | 1);
    case EEXIST:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EEXIST))) << 32) | 1);
    case EFAULT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EFAULT))) << 32) | 1);
    case EFBIG:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EFBIG))) << 32) | 1);
    case EINTR:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EINTR))) << 32) | 1);
    case EINVAL:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EINVAL))) << 32) | 1);
    case EIO:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EIO))) << 32) | 1);
    case EISDIR:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EISDIR))) << 32) | 1);
    case EMFILE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EMFILE))) << 32) | 1);
    case EMLINK:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EMLINK))) << 32) | 1);
    case ENAMETOOLONG:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENAMETOOLONG))) << 32) | 1);
    case ENFILE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENFILE))) << 32) | 1);
    case ENODEV:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENODEV))) << 32) | 1);
    case ENOENT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOENT))) << 32) | 1);
    case ENOEXEC:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOEXEC))) << 32) | 1);
    case ENOLCK:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOLCK))) << 32) | 1);
    case ENOMEM:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOMEM))) << 32) | 1);
    case ENOSPC:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOSPC))) << 32) | 1);
    case ENOSYS:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOSYS))) << 32) | 1);
    case ENOTDIR:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOTDIR))) << 32) | 1);
    case ENOTEMPTY:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOTEMPTY))) << 32) | 1);
    case ENOTTY:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOTTY))) << 32) | 1);
    case ENXIO:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENXIO))) << 32) | 1);
    case EPERM:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EPERM))) << 32) | 1);
    case EPIPE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EPIPE))) << 32) | 1);
    case ERANGE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ERANGE))) << 32) | 1);
    case EROFS:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EROFS))) << 32) | 1);
    case ESPIPE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ESPIPE))) << 32) | 1);
    case ESRCH:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ESRCH))) << 32) | 1);
    case EXDEV:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EXDEV))) << 32) | 1);
#if EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EWOULDBLOCK))) << 32) | 1);
#endif 
    case EINPROGRESS:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EINPROGRESS))) << 32) | 1);
    case EALREADY:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EALREADY))) << 32) | 1);
    case ENOTSOCK:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOTSOCK))) << 32) | 1);
    case EDESTADDRREQ:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EDESTADDRREQ))) << 32) | 1);
    case EMSGSIZE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EMSGSIZE))) << 32) | 1);
    case EPROTOTYPE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EPROTOTYPE))) << 32) | 1);
    case ENOPROTOOPT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOPROTOOPT))) << 32) | 1);
    case EPROTONOSUPPORT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EPROTONOSUPPORT))) << 32) | 1);
    case ESOCKTNOSUPPORT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ESOCKTNOSUPPORT))) << 32) | 1);
    case EOPNOTSUPP:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EOPNOTSUPP))) << 32) | 1);
    case EPFNOSUPPORT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EPFNOSUPPORT))) << 32) | 1);
    case EAFNOSUPPORT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EAFNOSUPPORT))) << 32) | 1);
    case EADDRINUSE:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EADDRINUSE))) << 32) | 1);
    case EADDRNOTAVAIL:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EADDRNOTAVAIL))) << 32) | 1);
    case ENETDOWN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENETDOWN))) << 32) | 1);
    case ENETUNREACH:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENETUNREACH))) << 32) | 1);
    case ENETRESET:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENETRESET))) << 32) | 1);
    case ECONNABORTED:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ECONNABORTED))) << 32) | 1);
    case ECONNRESET:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ECONNRESET))) << 32) | 1);
    case ENOBUFS:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOBUFS))) << 32) | 1);
    case EISCONN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EISCONN))) << 32) | 1);
    case ENOTCONN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ENOTCONN))) << 32) | 1);
    case ESHUTDOWN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ESHUTDOWN))) << 32) | 1);
    case ETOOMANYREFS:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ETOOMANYREFS))) << 32) | 1);
    case ETIMEDOUT:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ETIMEDOUT))) << 32) | 1);
    case ECONNREFUSED:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ECONNREFUSED))) << 32) | 1);
    case EHOSTDOWN:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EHOSTDOWN))) << 32) | 1);
    case EHOSTUNREACH:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EHOSTUNREACH))) << 32) | 1);
    case ELOOP:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(ELOOP))) << 32) | 1);
    case EOVERFLOW:
      retBlock->h = getBlockHeaderForSymbol((((uint64_t)getTagForSymbolName(ERRTAG(EOVERFLOW))) << 32) | 1);
    default:
      retBlock = static_cast<block *>(koreResizeLastAlloc(retBlock, sizeof(block) + sizeof(uint64_t), sizeof(block)));
      retBlock->h = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("Lbl'Hash'unkownIOError"));
      mpz_t err;
      mpz_init_set_si(err, errno);
      mpz_ptr p = move_int(err);
      memcpy(&(retBlock->children[0]), &p, sizeof(uint64_t));
    }

    return retBlock;
  }

  blockheader header_int() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortInt, SortIOInt}"));
    }

    return header;
  }

  blockheader header_err() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortIOError, SortKItem}"));
    }

    return header;
  }

  blockheader header_string() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortString, SortIOString}"));
    }

    return header;
  }

  char * getTerminatedString(string * str) {
    int length = len(str);
    char * buf = (char *) malloc(sizeof(KCHAR) * (length + 1));
    strncpy(buf, str->data, length);
    buf[length] = '\0';
    return buf;
  }

  block * hook_IO_open(string * filename, string * control) {
    int flags = 0;
    int length = len(control);

    switch (control->data[0]) {
      case 'r':
        flags = O_RDONLY;
      case 'w':
        flags = O_WRONLY | O_TRUNC | O_CREAT;
      case 'a':
        flags = O_WRONLY | O_APPEND | O_CREAT;
      default:
        flags = O_RDONLY;
    }

    if ((length > 1 && control->data[1] == '+') || (length > 2 && control->data[2] == '+')) {
      flags &= (~O_WRONLY) & ~(O_RDONLY);
      flags |= O_RDWR;
    }

    mpz_t result;
    char * f = getTerminatedString(filename);
    int fd = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(uint64_t)));

    if (-1 == fd) {
      retBlock->h = header_err();
      block * p = block_errno();
      memcpy(&(retBlock->children[0]), &p, sizeof(uint64_t));

    } else {
      retBlock->h = header_int();
      mpz_init_set_si(result, fd);
      mpz_ptr p = move_int(result);
      memcpy(&(retBlock->children[0]), &p, sizeof(uint64_t));
    }

    free(f);

    return retBlock;
  }

  mpz_ptr hook_IO_tell(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);
    off_t loc = lseek(fd, 0, SEEK_CUR);

    mpz_t result;
    mpz_init_set_si(result, (long) loc);
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
    mpz_init_set_si(result, (int) c);
    return move_int(result);
  }

  string * hook_IO_read(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    size_t length = mpz_get_ui(len);

    auto result = static_cast<string *>(koreAllocToken(sizeof(string) + length));
    int bytes = read(fd, &(result->data), length);
    result = static_cast<string *>(koreResizeLastAlloc(result, bytes, length));
    set_len(result, bytes);

    return result;
  }

  block * hook_IO_close(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);
    int ret = close(fd);

    if (ret == -1) {
      block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + 2 * sizeof(uint64_t)));
      retBlock->h = header;
      block * inj = static_cast<block *>(koreAlloc(sizeof(block + sizeof(uint64_t))));
      inj->h = header_err();
      block * err = block_err();
      memcpy(&(inj->children[0]), &block_err, sizeof(uint64_t));
      memcpy(&(retBlock->children[0]), &inj, sizeof(uint64_t));
      memcpy(&(retBlock->children[1]), &dotK, sizeof(uint64_t));
      return retBlock;
    }

    return &dotK;
  }

  block * hook_IO_seek(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(loc);
    lseek(fd, l, SEEK_SET);
    return &dotK;
  }

  block * hook_IO_seekEnd(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(loc);
    lseek(fd, l, SEEK_END);
    return &dotK;
  }

  block * hook_IO_putc(mpz_t i, mpz_t c) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(c)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int ch = mpz_get_si(c);

    write(fd, &ch, 1);
    return &dotK;
  }

  block * hook_IO_write(mpz_t i, string * str) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int32_t");
    }

    int fd = mpz_get_si(i);
    write(fd, str->data, len(str));
    return &dotK;
  }

  block * hook_IO_lock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(len);

    lockf(fd, F_LOCK, l);
    return &dotK;
  }

  block * hook_IO_unlock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(len);

    lockf(fd, F_ULOCK, l);
    return &dotK;
  }
}
