#include <kllvm/printer/printer.h>

#include "gmp.h"
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <kllvm/parser/KOREParser.h>
#include <kllvm/util/FileRAII.h>

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

string *debug_print_term(block *subject, char const *sort) {
  auto state = print_state();
  auto *buf = hook_BUFFER_empty();
  writer w = {nullptr, buf};

  char const *print_sort = nullptr;

  if (sort) {
    auto inj_sym = "inj{" + std::string(sort) + ", SortKItem{}}";
    auto tag = getTagForSymbolName(inj_sym.c_str());
    auto args = std::vector<void *>{subject};

    subject = static_cast<block *>(constructCompositePattern(tag, args));
    print_sort = "SortKItem{}";
  }

  printConfigurationInternal(&w, subject, print_sort, false, &state);
  return hook_BUFFER_toString(buf);
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

void *termToKorePattern(block *subject) {
  auto *kore_str = printConfigurationToString(subject);
  auto kore = std::string(kore_str->data, len(kore_str));

  auto parser = kllvm::parser::KOREParser::from_string(kore);
  auto pattern = parser->pattern();

  return static_cast<void *>(pattern.release());
}

extern "C" void printMatchResult(
    std::ostream &os, MatchLog *matchLog, size_t logSize,
    const std::string &definitionPath) {
  auto subject_raii = FileRAII("subject_XXXXXX");
  auto subject = subject_raii.getFILE("w");
  auto pattern_raii = FileRAII("pattern_XXXXXX");
  auto pattern = pattern_raii.getFILE("w");

  for (int i = 0; i < logSize; i++) {
    if (matchLog[i].kind == MatchLog::SUCCESS) {
      os << "Match succeeds\n";
    } else if (matchLog[i].kind == MatchLog::FAIL) {
      os << "Subject:\n";
      if (i == 0) {
        printSortedConfigurationToFile(
            subject, (block *)matchLog[i].subject, matchLog[i].sort);
      } else {
        auto subjectSort
            = debug_print_term((block *)matchLog[i].subject, matchLog[i].sort);
        auto strSubjectSort = std::string(subjectSort->data, len(subjectSort));
        fprintf(subject, "%s\n", strSubjectSort.c_str());
      }
      fflush(subject);
      kllvm::printKORE(
          os, definitionPath, subject_raii.getFilename(), false, true);
      os << "does not match pattern: \n";
      fprintf(pattern, "%s\n", matchLog[i].pattern);
      fflush(pattern);
      kllvm::printKORE(
          os, definitionPath, pattern_raii.getFilename(), false, true);
    } else if (matchLog[i].kind == MatchLog::FUNCTION) {
      os << matchLog[i].debugName << "(";

      for (int j = 0; j < matchLog[i].args.size(); j += 2) {
        auto typeName = reinterpret_cast<char *>(matchLog[i].args[j + 1]);
        printValueOfType(os, definitionPath, matchLog[i].args[j], typeName);
        if (j + 2 != matchLog[i].args.size())
          os << ", ";
      }
      os << ") => " << *reinterpret_cast<bool *>(matchLog[i].result) << "\n";
    }
  }
}

void printValueOfType(
    std::ostream &os, std::string definitionPath, void *value,
    std::string type) {
  if (type.compare("%mpz*") == 0) {
    os << reinterpret_cast<mpz_ptr>(value);
  } else if (type.compare("%block*") == 0) {
    if ((((uintptr_t)value) & 3) == 1) {
      auto f = FileRAII("subject_XXXXXX");
      auto subject = f.getFILE("w");
      string *s = printConfigurationToString(reinterpret_cast<block *>(value));
      auto strSubjectSort = std::string(s->data, len(s));
      fprintf(subject, "%s", strSubjectSort.c_str());
      fflush(subject);
      kllvm::printKORE(os, definitionPath, f.getFilename(), false, true);
    } else if ((((uintptr_t)value) & 1) == 0) {
      auto s = reinterpret_cast<string *>(value);
      os << std::string(s->data, len(s));
    } else {
      os << "Error: " << type << " not implemented!";
    }
  } else if (type.compare("%floating*") == 0) {
    os << floatToString(reinterpret_cast<floating *>(value));
  } else if (type.compare("i1") == 0) {
    os << *reinterpret_cast<bool *>(value);
  } else {
    os << "Error: " << type << " not implemented!";
  }
}
