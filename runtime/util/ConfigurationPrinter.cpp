#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "runtime/alloc.h"
#include "runtime/header.h"

struct StringHash {
  size_t operator()(string *const &k) const {
    return std::hash<std::string>{}(std::string(k->data, len(k)));
  }
};

struct StringEq {
  bool operator()(string *const &lhs, string *const &rhs) const {
    return hook_STRING_eq(lhs, rhs);
  }
};

struct print_state {
  print_state()
      : boundVariables{}
      , varNames{}
      , usedVarNames{}
      , varCounter(0) { }

  // We never want to copy the state; it should only ever get passed around by
  // reference.
  print_state(print_state const &) = delete;

  std::vector<block *> boundVariables;
  std::unordered_map<string *, std::string, StringHash, StringEq> varNames;
  std::set<std::string> usedVarNames;
  uint64_t varCounter;
};

void printInt(writer *file, mpz_t i, const char *sort, void *state) {
  auto str = intToString(i);
  sfprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str());
}

void printFloat(writer *file, floating *f, const char *sort, void *state) {
  std::string str = floatToString(f);
  sfprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str());
}

void printBool(writer *file, bool b, const char *sort, void *state) {
  const char *str = b ? "true" : "false";
  sfprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printStringBuffer(
    writer *file, stringbuffer *b, const char *sort, void *state) {
  std::string str(b->contents->data, b->strlen);
  sfprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str());
}

void printMInt(
    writer *file, size_t *i, size_t bits, const char *sort, void *state) {
  if (i == nullptr) {
    sfprintf(file, "\\dv{%s}(\"0p%zd\")", sort, bits);
  } else {
    mpz_ptr z = hook_MINT_import(i, bits, false);
    auto str = intToString(z);
    sfprintf(file, "\\dv{%s}(\"%sp%zd\")", sort, str.c_str(), bits);
  }
}

void sfprintf(writer *file, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  if (file->file) {
    vfprintf(file->file, fmt, args);
  } else {
    char buf[8192];
    char *finalBuf = buf;
    int res = vsnprintf(
        buf + sizeof(blockheader), sizeof(buf) - sizeof(blockheader), fmt,
        args);
    if (res >= sizeof(buf) - sizeof(blockheader)) {
      size_t size = sizeof(buf) * 2;
      finalBuf = (char *)malloc(size);
      memcpy(finalBuf, buf, sizeof(buf));
      res = vsnprintf(
          finalBuf + sizeof(blockheader), size - sizeof(blockheader), fmt,
          args);
      if (res >= size - sizeof(blockheader)) {
        do {
          size *= 2;
          finalBuf = (char *)realloc(finalBuf, size);
          res = vsnprintf(
              finalBuf + sizeof(blockheader), size - sizeof(blockheader), fmt,
              args);
        } while (res >= size - sizeof(blockheader));
      }
    }
    string *str = (string *)finalBuf;
    set_len(str, res);
    hook_BUFFER_concat(file->buffer, str);
  }
}

void printComma(writer *file, void *state) {
  sfprintf(file, ",");
}

void printConfigurationInternal(
    writer *file, block *subject, const char *sort, bool isVar,
    void *state_ptr) {
  auto &state = *static_cast<print_state *>(state_ptr);

  uint8_t isConstant = ((uintptr_t)subject) & 3;
  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;
    if (isConstant == 3) {
      // bound variable
      printConfigurationInternal(
          file, state.boundVariables[state.boundVariables.size() - 1 - tag],
          sort, true, state_ptr);
      return;
    }
    const char *symbol = getSymbolNameForTag(tag);
    sfprintf(file, "%s()", symbol);
    return;
  }
  uint16_t layout = layout(subject);
  if (!layout) {
    string *str = (string *)subject;
    size_t len = len(subject);
    sfprintf(file, "\\dv{%s}(\"", sort);
    for (size_t i = 0; i < len; ++i) {
      char c = str->data[i];
      switch (c) {
      case '\\': sfprintf(file, "\\\\"); break;
      case '"': sfprintf(file, "\\\""); break;
      case '\n': sfprintf(file, "\\n"); break;
      case '\t': sfprintf(file, "\\t"); break;
      case '\r': sfprintf(file, "\\r"); break;
      case '\f': sfprintf(file, "\\f"); break;
      default:
        if ((unsigned char)c >= 32 && (unsigned char)c < 127) {
          sfprintf(file, "%c", c);
        } else {
          sfprintf(file, "\\x%02x", (unsigned char)c);
        }
        break;
      }
    }
    if (isVar && !state.varNames.count(str)) {
      std::string stdStr = std::string(str->data, len(str));
      std::string suffix = "";
      while (state.usedVarNames.count(stdStr + suffix)) {
        suffix = std::to_string(state.varCounter++);
      }
      stdStr = stdStr + suffix;
      sfprintf(file, "%s", suffix.c_str());
      state.usedVarNames.insert(stdStr);
      state.varNames[str] = suffix;
    } else if (isVar) {
      sfprintf(file, "%s", state.varNames[str].c_str());
    }
    sfprintf(file, "\")");
    return;
  }
  uint32_t tag = tag_hdr(subject->h.hdr);
  bool isBinder = isSymbolABinder(tag);
  if (isBinder) {
    state.boundVariables.push_back(
        *(block **)(((char *)subject) + sizeof(blockheader)));
  }
  const char *symbol = getSymbolNameForTag(tag);
  std::string symbolStr(symbol);
  if (symbolStr.rfind("inj{", 0) == 0) {
    std::string prefix = symbolStr.substr(0, symbolStr.find_first_of(','));
    sfprintf(file, "%s, %s}(", prefix.c_str(), sort);
  } else {
    sfprintf(file, "%s(", symbol);
  }

  visitor callbacks
      = {printConfigurationInternal,
         printMap,
         printList,
         printSet,
         printInt,
         printFloat,
         printBool,
         printStringBuffer,
         printMInt,
         printComma};

  visitChildren(subject, file, &callbacks, state_ptr);

  if (isBinder) {
    state.boundVariables.pop_back();
  }
  sfprintf(file, ")");
}

void printStatistics(const char *filename, uint64_t steps) {
  FILE *file = fopen(filename, "w");
  fprintf(file, "%" PRIu64 "\n", steps - 1); // off by one adjustment
  fclose(file);
}

void printConfiguration(const char *filename, block *subject) {
  FILE *file = fopen(filename, "a");
  auto state = print_state();

  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, nullptr, false, &state);

  fclose(file);
}

void printConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results) {
  FILE *file = fopen(filename, "a");
  auto state = print_state();

  writer w = {file, nullptr};
  ssize_t size = results.size();
  if (size == 0) {
    sfprintf(&w, "\\bottom{SortGeneratedTopCell{}}()");
  } else {
    sfprintf(&w, "\\left-assoc{}(\\or{SortGeneratedTopCell{}}(");
    size_t j = 0;
    for (const auto &subject : results) {
      printConfigurationInternal(&w, subject, nullptr, false, &state);
      if (++j != results.size()) {
        sfprintf(&w, ",");
      }
    }
    sfprintf(&w, "))");
  }

  fclose(file);
}

string *printConfigurationToString(block *subject) {
  auto state = print_state();
  stringbuffer *buf = hook_BUFFER_empty();
  writer w = {nullptr, buf};
  printConfigurationInternal(&w, subject, nullptr, false, &state);
  return hook_BUFFER_toString(buf);
}

void printConfigurationToFile(FILE *file, block *subject) {
  auto state = print_state();
  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, nullptr, false, &state);
}

void printSortedConfigurationToFile(
    FILE *file, block *subject, char const *sort) {
  auto state = print_state();
  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, sort, false, &state);
}

extern "C" void *getStderr(void) {
  return stderr;
}

struct MatchLog {
  enum { SUCCESS = 0, FUNCTION, FAIL } kind;

  char *function;
  char *debugName;
  void *result;
  std::vector<void *> args;

  char *pattern;
  void *subject;
  char *sort;
};

static std::vector<MatchLog> matchLog;

void resetMatchReason(void) {
  matchLog.clear();
}

MatchLog *getMatchLog(void) {
  return &matchLog[0];
}

void **getMatchFnArgs(MatchLog *log) {
  return &log->args[0];
}

size_t getMatchLogSize(void) {
  return matchLog.size();
}

extern "C" {

void addMatchSuccess(void) {
  matchLog.push_back(
      {MatchLog::SUCCESS, NULL, NULL, NULL, {}, NULL, NULL, NULL});
}

void addMatchFailReason(void *subject, char *pattern, char *sort) {
  matchLog.push_back(
      {MatchLog::FAIL, NULL, NULL, NULL, {}, pattern, subject, sort});
}

void addMatchFunction(char *debugName, char *function, void *result, ...) {
  va_list ap;
  va_start(ap, result);

  std::vector<void *> args;
  while (true) {
    void *arg = va_arg(ap, void *);
    if (!arg)
      break;
    args.push_back(arg);
  }

  matchLog.push_back(
      {MatchLog::FUNCTION, function, debugName, result, args, NULL, NULL,
       NULL});

  va_end(ap);
}
}

#define DEFINE_GDB_PY_SCRIPT(script_path, script_name)                         \
  asm("\
.pushsection \".debug_gdb_scripts\", \"MS\",@progbits,1\n\
.byte 4 /* Python inlined */\n\
.ascii \"" script_name "\\n\"\n\
.incbin \"" script_path "/" script_name "\"\n\
.byte 0 \n\
.popsection \n\
");

#ifndef __APPLE__
DEFINE_GDB_PY_SCRIPT(GDB_SCRIPT_PATH, GDB_SCRIPT_NAME)
#endif
