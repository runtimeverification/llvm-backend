#include <kllvm/printer/printer.h>

#include <cinttypes>
#include <cstdarg>
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
#include <kllvm/util/temporary_file.h>

#include "gmp.h"

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
  print_state() = default;

  // We never want to copy the state; it should only ever get passed around by
  // reference.
  print_state(print_state const &) = delete;
  print_state &operator=(print_state const &) = delete;

  print_state(print_state &&) = default;
  print_state &operator=(print_state &&) = default;

  ~print_state() = default;

  std::vector<block *> boundVariables;
  std::unordered_map<string *, std::string, StringHash, StringEq> varNames;
  std::set<std::string> usedVarNames;
  uint64_t varCounter{0};
};

void printInt(writer *file, mpz_t i, char const *sort, void *state) {
  auto str = intToString(i);
  sfprintf(file, R"(\dv{%s}("%s"))", sort, str.c_str());
}

void printFloat(writer *file, floating *f, char const *sort, void *state) {
  std::string str = floatToString(f);
  sfprintf(file, R"(\dv{%s}("%s"))", sort, str.c_str());
}

void printBool(writer *file, bool b, char const *sort, void *state) {
  char const *str = b ? "true" : "false";
  sfprintf(file, R"(\dv{%s}("%s"))", sort, str);
}

void printStringBuffer(
    writer *file, stringbuffer *b, char const *sort, void *state) {
  std::string str(b->contents->data, b->strlen);
  sfprintf(file, R"(\dv{%s}("%s"))", sort, str.c_str());
}

void printMInt(
    writer *file, size_t *i, size_t bits, char const *sort, void *state) {
  if (i == nullptr) {
    sfprintf(file, R"(\dv{%s}("0p%zd"))", sort, bits);
  } else {
    mpz_ptr z = hook_MINT_import(i, bits, false);
    auto str = intToString(z);
    sfprintf(file, R"(\dv{%s}("%sp%zd"))", sort, str.c_str(), bits);
  }
}

void printComma(writer *file, void *state) {
  sfprintf(file, ",");
}

// NOLINTNEXTLINE(*-cognitive-complexity)
void printConfigurationInternal(
    writer *file, block *subject, char const *sort, bool isVar,
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
    char const *symbol = getSymbolNameForTag(tag);
    sfprintf(file, "%s()", symbol);
    return;
  }
  uint16_t layout = get_layout(subject);
  if (!layout) {
    auto *str = (string *)subject;
    size_t subject_len = len(subject);
    sfprintf(file, "\\dv{%s}(\"", sort);
    for (size_t i = 0; i < subject_len; ++i) {
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
      std::string suffix;
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
  char const *symbol = getSymbolNameForTag(tag);
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
         printComma,
         printRangeMap};

  visitChildren(subject, file, &callbacks, state_ptr);

  if (isBinder) {
    state.boundVariables.pop_back();
  }
  sfprintf(file, ")");
}

void printStatistics(char const *filename, uint64_t steps) {
  FILE *file = fopen(filename, "w");
  fmt::print(file, "{}\n", steps - 1); // off by one adjustment
  fclose(file);
}

void printConfiguration(char const *filename, block *subject) {
  FILE *file = fopen(filename, "a");
  auto state = print_state();

  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, nullptr, false, &state);

  fclose(file);
}

// If the parameter `results` is passed by reference, the ordering induced by
// the hash table becomes unstable when running tests in CI and examining the
// expected output. We therefore just pass the results by value for now as this
// code is not on a hot path.
// NOLINTBEGIN(performance-unnecessary-value-param)
void printConfigurations(
    char const *filename, std::unordered_set<block *, HashBlock, KEq> results) {
  FILE *file = fopen(filename, "a");
  auto state = print_state();

  writer w = {file, nullptr};
  ssize_t size = results.size();
  if (size == 0) {
    sfprintf(&w, "\\bottom{SortGeneratedTopCell{}}()");
  } else {
    sfprintf(&w, "\\or{SortGeneratedTopCell{}}(");
    size_t j = 0;
    for (auto const &subject : results) {
      printConfigurationInternal(&w, subject, nullptr, false, &state);
      if (++j != results.size()) {
        sfprintf(&w, ",");
      }
    }
    sfprintf(&w, ")");
  }

  fclose(file);
}
// NOLINTEND(performance-unnecessary-value-param)

string *debug_print_term(block *subject, char const *sort) {
  auto state = print_state();
  auto *buf = hook_BUFFER_empty();
  writer w = {nullptr, buf};

  char const *print_sort = nullptr;

  if (sort) {
    subject = constructKItemInj(subject, sort, false);
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

extern "C" void printMatchResult(
    std::ostream &os, MatchLog *matchLog, size_t logSize,
    std::string const &definitionPath) {
  auto subject_file = temporary_file("subject_XXXXXX");
  auto *subject = subject_file.file_pointer("w");
  auto pattern_file = temporary_file("pattern_XXXXXX");

  for (int i = 0; i < logSize; i++) {
    if (matchLog[i].kind == MatchLog::SUCCESS) {
      os << "Match succeeds\n";
    } else if (matchLog[i].kind == MatchLog::FAIL) {
      os << "Subject:\n";
      if (i == 0) {
        printSortedConfigurationToFile(
            subject, (block *)matchLog[i].subject, matchLog[i].sort);
      } else {
        auto *subjectSort
            = debug_print_term((block *)matchLog[i].subject, matchLog[i].sort);
        auto strSubjectSort = std::string(subjectSort->data, len(subjectSort));
        subject_file.ofstream() << strSubjectSort << std::endl;
      }
      kllvm::printKORE(
          os, definitionPath, subject_file.filename(), false, true);
      os << "does not match pattern: \n";
      pattern_file.ofstream() << matchLog[i].pattern << std::endl;
      kllvm::printKORE(
          os, definitionPath, pattern_file.filename(), false, true);
    } else if (matchLog[i].kind == MatchLog::FUNCTION) {
      os << matchLog[i].debugName << "(";

      for (int j = 0; j < matchLog[i].args.size(); j += 2) {
        auto *typeName = static_cast<char *>(matchLog[i].args[j + 1]);
        printValueOfType(os, definitionPath, matchLog[i].args[j], typeName);
        if (j + 2 != matchLog[i].args.size()) {
          os << ", ";
        }
      }
      os << ") => " << *static_cast<bool *>(matchLog[i].result) << "\n";
    }
  }
}

void printValueOfType(
    std::ostream &os, std::string const &definitionPath, void *value,
    std::string const &type) {
  if (type == "%mpz*") {
    os << static_cast<mpz_ptr>(value);
  } else if (type == "%block*") {
    if ((((uintptr_t)value) & 3) == 1) {
      auto f = temporary_file("subject_XXXXXX");
      string *s = printConfigurationToString(static_cast<block *>(value));
      f.ofstream() << std::string(s->data, len(s)) << std::endl;
      kllvm::printKORE(os, definitionPath, f.filename(), false, true);
    } else if ((((uintptr_t)value) & 1) == 0) {
      auto *s = static_cast<string *>(value);
      os << std::string(s->data, len(s));
    } else {
      os << "Error: " << type << " not implemented!";
    }
  } else if (type == "%floating*") {
    os << floatToString(static_cast<floating *>(value));
  } else if (type == "i1") {
    os << *static_cast<bool *>(value);
  } else {
    os << "Error: " << type << " not implemented!";
  }
}

void printVariableToFile(char const *filename, char const *varname) {
  FILE *file = fopen(filename, "a");

  fmt::print(file, "{}", varname);
  char n = 0;
  fwrite(&n, 1, 1, file);
  fflush(file);

  fclose(file);
}
