#include <gmp.h>
#include <cstring>
#include <stdexcept>

#include "fcntl.h"
#include "unistd.h"
#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char
#define ERRTAG(err) "Lbl'hash'" #err "{}"

  mpz_ptr move_int(mpz_t);

  static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);
  static blockheader kseqHeader = {getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("kseq{}"))};

  block * block_errno() {
    const char * errStr;
    switch (errno) {
    case EOF: errStr = ERRTAG(EOF); break;
    case E2BIG: errStr = ERRTAG(E2BIG); break;
    case EACCES: errStr = ERRTAG(EACCES); break;
    case EAGAIN: errStr = ERRTAG(EAGAIN); break;
    case EBADF: errStr = ERRTAG(EBADF); break;
    case EBUSY: errStr = ERRTAG(EBUSY); break;
    case ECHILD: errStr = ERRTAG(ECHILD); break;
    case EDEADLK: errStr = ERRTAG(EDEADLK); break;
    case EDOM: errStr = ERRTAG(EDOM); break;
    case EEXIST: errStr = ERRTAG(EEXIST); break;
    case EFAULT: errStr = ERRTAG(EFAULT); break;
    case EFBIG: errStr = ERRTAG(EFBIG); break;
    case EINTR: errStr = ERRTAG(EINTR); break;
    case EINVAL: errStr = ERRTAG(EINVAL); break;
    case EIO: errStr = ERRTAG(EIO); break;
    case EISDIR: errStr = ERRTAG(EISDIR); break;
    case EMFILE: errStr = ERRTAG(EMFILE); break;
    case EMLINK: errStr = ERRTAG(EMLINK); break;
    case ENAMETOOLONG: errStr = ERRTAG(ENAMETOOLONG); break;
    case ENFILE: errStr = ERRTAG(ENFILE); break;
    case ENODEV: errStr = ERRTAG(ENODEV); break;
    case ENOENT: errStr = ERRTAG(ENOENT); break;
    case ENOEXEC: errStr = ERRTAG(ENOEXEC); break;
    case ENOLCK: errStr = ERRTAG(ENOLCK); break;
    case ENOMEM: errStr = ERRTAG(ENOMEM); break;
    case ENOSPC: errStr = ERRTAG(ENOSPC); break;
    case ENOSYS: errStr = ERRTAG(ENOSYS); break;
    case ENOTDIR: errStr = ERRTAG(ENOTDIR); break;
    case ENOTEMPTY: errStr = ERRTAG(ENOTEMPTY); break;
    case ENOTTY: errStr = ERRTAG(ENOTTY); break;
    case ENXIO: errStr = ERRTAG(ENXIO); break;
    case EPERM: errStr = ERRTAG(EPERM); break;
    case EPIPE: errStr = ERRTAG(EPIPE); break;
    case ERANGE: errStr = ERRTAG(ERANGE); break;
    case EROFS: errStr = ERRTAG(EROFS); break;
    case ESPIPE: errStr = ERRTAG(ESPIPE); break;
    case ESRCH: errStr = ERRTAG(ESRCH); break;
    case EXDEV: errStr = ERRTAG(EXDEV); break;
#if EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK: errStr = ERRTAG(EWOULDBLOCK); break;
#endif 
    case EINPROGRESS: errStr = ERRTAG(EINPROGRESS); break;
    case EALREADY: errStr = ERRTAG(EALREADY); break;
    case ENOTSOCK: errStr = ERRTAG(ENOTSOCK); break;
    case EDESTADDRREQ: errStr = ERRTAG(EDESTADDRREQ); break;
    case EMSGSIZE: errStr = ERRTAG(EMSGSIZE); break;
    case EPROTOTYPE: errStr = ERRTAG(EPROTOTYPE); break;
    case ENOPROTOOPT: errStr = ERRTAG(ENOPROTOOPT); break;
    case EPROTONOSUPPORT: errStr = ERRTAG(EPROTONOSUPPORT); break;
    case ESOCKTNOSUPPORT: errStr = ERRTAG(ESOCKTNOSUPPORT); break;
    case EOPNOTSUPP: errStr = ERRTAG(EOPNOTSUPP); break;
    case EPFNOSUPPORT: errStr = ERRTAG(EPFNOSUPPORT); break;
    case EAFNOSUPPORT: errStr = ERRTAG(EAFNOSUPPORT); break;
    case EADDRINUSE: errStr = ERRTAG(EADDRINUSE); break;
    case EADDRNOTAVAIL: errStr = ERRTAG(EADDRNOTAVAIL); break;
    case ENETDOWN: errStr = ERRTAG(ENETDOWN); break;
    case ENETUNREACH: errStr = ERRTAG(ENETUNREACH); break;
    case ENETRESET: errStr = ERRTAG(ENETRESET); break;
    case ECONNABORTED: errStr = ERRTAG(ECONNABORTED); break;
    case ECONNRESET: errStr = ERRTAG(ECONNRESET); break;
    case ENOBUFS: errStr = ERRTAG(ENOBUFS); break;
    case EISCONN: errStr = ERRTAG(EISCONN); break;
    case ENOTCONN: errStr = ERRTAG(ENOTCONN); break;
    case ESHUTDOWN: errStr = ERRTAG(ESHUTDOWN); break;
    case ETOOMANYREFS: errStr = ERRTAG(ETOOMANYREFS); break;
    case ETIMEDOUT: errStr = ERRTAG(ETIMEDOUT); break;
    case ECONNREFUSED: errStr = ERRTAG(ECONNREFUSED); break;
    case EHOSTDOWN: errStr = ERRTAG(EHOSTDOWN); break;
    case EHOSTUNREACH: errStr = ERRTAG(EHOSTUNREACH); break;
    case ELOOP: errStr = ERRTAG(ELOOP); break;
    case EOVERFLOW: errStr = ERRTAG(EOVERFLOW); break;
    default:
      block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(mpz_ptr)));
      retBlock->h = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("Lbl'Hash'unknownIOError{}"));
      mpz_t err;
      mpz_init_set_si(err, errno);
      mpz_ptr p = move_int(err);
      memcpy(retBlock->children, &p, sizeof(mpz_ptr));
      return retBlock;
    }

    return (block *)((((uint64_t)getTagForSymbolName(errStr)) << 32) | 1);
  }

  blockheader header_int() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      header = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortInt{}, SortIOInt{}}"));
    }

    return header;
  }

  blockheader header_err() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      header = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortIOError{}, SortKItem{}}"));
    }

    return header;
  }

  blockheader header_string() {
    static blockheader header = {(uint64_t)-1};

    if (header.hdr == -1) {
      header = getBlockHeaderForSymbol((uint64_t)getTagForSymbolName("inj{SortString{}, SortIOString{}}"));
    }

    return header;
  }

  inline block * getKSeqErrorBlock() {
    block * err = block_errno();
    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + 2 * sizeof(block *)));
    block * inj = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(block *)));
    retBlock->h = kseqHeader;
    inj->h = header_err();
    memcpy(inj->children, &err, sizeof(block *));
    memcpy(retBlock->children, &inj, sizeof(block *));
    memcpy(retBlock->children + 1, &dotK, sizeof(block *));
    return retBlock;
  }

  inline block* getInjErrorBlock() {
    block * p = block_errno();
    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(block *)));
    retBlock->h = header_err();
    memcpy(retBlock->children, &p, sizeof(block *));
    return retBlock;
  }

  char * getTerminatedString(string * str) {
    int length = len(str);
    string * buf = static_cast<string *>(koreAllocToken(sizeof(string) + (length + 1)));
    memcpy(buf->data, str->data, length);
    set_len(buf, length + 1);
    buf->data[length] = '\0';
    return buf->data;
  }

#define MODE_R 1
#define MODE_W 2
#define MODE_A 4
#define MODE_E 8
#define MODE_X 16
#define MODE_B 32
#define MODE_P 64

  int getFileModes(string * modes) {
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
        case 'e':
          flags |= (flags & MODE_E) ? -1 : MODE_E;
          break;
        case 'x':
          flags |= (flags & MODE_X) ? -1 : MODE_X;
          break;
        case 'b':
          flags |= (flags & MODE_B) ? -1 : MODE_B;
          break;
        case '+':
          flags |= (flags & MODE_P) ? -1 : MODE_P;
          break;
        default: return -1;
      }
    }

    return (flags < 0) ? -1 : flags;
  }

  block * hook_IO_open(string * filename, string * control) {
    int flags = 0;
    int access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    int modes = getFileModes(control);
    int fd;
    mpz_t result;

    if (-1 != modes) {
      switch (modes & 7) {
        case 1:
          flags = O_RDONLY;
          break;
        case 2:
          flags = O_WRONLY | O_TRUNC | O_CREAT;
          break;
        case 4:
          flags = O_WRONLY | O_APPEND | O_CREAT;
          break;
      }

      if (modes & MODE_P) {
        flags &= (~O_WRONLY) & ~(O_RDONLY);
        flags |= O_RDWR;
      }

      if (modes & MODE_E) {
        flags |= O_CLOEXEC;
      }

      if (modes & MODE_X) {
        flags |= O_EXCL;
      }

      if (modes & MODE_B) {
      }

      char * f = getTerminatedString(filename);
      fd = open(f, flags, access);
    } else {
      errno = EINVAL;
      fd = -1;
    }

    if (-1 == fd) {
      return getInjErrorBlock();
    }

    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(uint64_t)));
    retBlock->h = header_int();
    mpz_init_set_si(result, fd);
    mpz_ptr p = move_int(result);
    memcpy(retBlock->children, &p, sizeof(mpz_ptr));
    return retBlock;
  }

  block * hook_IO_tell(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int");
    }

    int fd = mpz_get_si(i);
    off_t loc = lseek(fd, 0, SEEK_CUR);

    if (-1 == loc) {
      return getInjErrorBlock();
    }

    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(uint64_t)));
    retBlock->h = header_int();
    mpz_t result;
    mpz_init_set_si(result, (long) loc);
    mpz_ptr p =  move_int(result);
    memcpy(retBlock->children, &p, sizeof(mpz_ptr));
    return retBlock;
  }

  block * hook_IO_getc(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int");
    }

    int fd = mpz_get_si(i);
    char c;
    ssize_t ret = read(fd, &c, sizeof(char));


    if (-1 == ret) {
      return getInjErrorBlock();
    }

    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(uint64_t)));
    retBlock->h = header_int();
    mpz_t result;
    mpz_init_set_si(result, (int) c);
    mpz_ptr p = move_int(result);
    memcpy(retBlock->children, &p, sizeof(mpz_ptr));
    return retBlock;
  }

  block * hook_IO_read(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    size_t length = mpz_get_ui(len);

    auto result = static_cast<string *>(koreAllocToken(sizeof(string) + length));
    int bytes = read(fd, &(result->data), length);

    if (-1 == bytes) {
      return getInjErrorBlock();
    }

    result = static_cast<string *>(koreResizeLastAlloc(result, sizeof(string) + bytes, sizeof(string) + length));
    block * retBlock = static_cast<block *>(koreAlloc(sizeof(block) + sizeof(uint64_t)));
    retBlock->h = header_string();
    set_len(result, bytes);
    memcpy(retBlock->children, &result, sizeof(string *));
    return retBlock;
  }

  block * hook_IO_close(mpz_t i) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int");
    }

    int fd = mpz_get_si(i);
    int ret = close(fd);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_seek(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(loc);
    int ret = lseek(fd, l, SEEK_SET);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_seekEnd(mpz_t i, mpz_t loc) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(loc)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(loc);
    int ret = lseek(fd, l, SEEK_END);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_putc(mpz_t i, mpz_t c) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_sint_p(c)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    int ch = mpz_get_si(c);
    int ret = write(fd, &ch, 1);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_write(mpz_t i, string * str) {
    if (!mpz_fits_sint_p(i)) {
      throw std::invalid_argument("Arg too large for int");
    }

    int fd = mpz_get_si(i);
    int ret = write(fd, str->data, len(str));

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_lock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(len);

    int ret = lockf(fd, F_LOCK, l);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }

  block * hook_IO_unlock(mpz_t i, mpz_t len) {
    if (!mpz_fits_sint_p(i) || !mpz_fits_slong_p(len)) {
      throw std::invalid_argument("Arg too large");
    }

    int fd = mpz_get_si(i);
    off_t l = mpz_get_si(len);

    int ret = lockf(fd, F_ULOCK, l);

    if (ret == -1) {
      return getKSeqErrorBlock();
    }

    return dotK;
  }
}
