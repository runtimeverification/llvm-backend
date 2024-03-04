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

struct string_hash {
  size_t operator()(string *const &k) const {
    return std::hash<std::string>{}(std::string(k->data, len(k)));
  }
};

struct string_eq {
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

  std::vector<block *> bound_variables;
  std::unordered_map<string *, std::string, string_hash, string_eq> var_names;
  std::set<std::string> used_var_names;
  uint64_t var_counter{0};
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
    writer *file, block *subject, char const *sort, bool is_var,
    void *state_ptr) {
  auto &state = *static_cast<print_state *>(state_ptr);

  uint8_t isConstant = ((uintptr_t)subject) & 3;
  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;
    if (isConstant == 3) {
      // bound variable
      printConfigurationInternal(
          file, state.bound_variables[state.bound_variables.size() - 1 - tag],
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
    if (is_var && !state.var_names.contains(str)) {
      std::string stdStr = std::string(str->data, len(str));
      std::string suffix;
      while (state.used_var_names.contains(stdStr + suffix)) {
        suffix = std::to_string(state.var_counter++);
      }
      stdStr = stdStr + suffix;
      sfprintf(file, "%s", suffix.c_str());
      state.used_var_names.insert(stdStr);
      state.var_names[str] = suffix;
    } else if (is_var) {
      sfprintf(file, "%s", state.var_names[str].c_str());
    }
    sfprintf(file, "\")");
    return;
  }
  uint32_t tag = tag_hdr(subject->h.hdr);
  bool isBinder = isSymbolABinder(tag);
  if (isBinder) {
    state.bound_variables.push_back(
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
    state.bound_variables.pop_back();
  }
  sfprintf(file, ")");
}

void printStatistics(FILE *file, uint64_t steps) {
  fmt::print(file, "{}\n", steps - 1); // off by one adjustment
}

void printConfiguration(FILE *file, block *subject) {
  auto state = print_state();

  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, nullptr, false, &state);
}

// If the parameter `results` is passed by reference, the ordering induced by
// the hash table becomes unstable when running tests in CI and examining the
// expected output. We therefore just pass the results by value for now as this
// code is not on a hot path.
// NOLINTBEGIN(performance-unnecessary-value-param)
void printConfigurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results) {
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

void printSortedConfigurationToFile(
    FILE *file, block *subject, char const *sort) {
  auto state = print_state();
  writer w = {file, nullptr};
  printConfigurationInternal(&w, subject, sort, false, &state);
}

extern "C" void printMatchResult(
    std::ostream &os, match_log *match_log, size_t log_size,
    std::string const &definition_path) {
  auto subject_file = temporary_file("subject_XXXXXX");
  auto *subject = subject_file.file_pointer("w");
  auto pattern_file = temporary_file("pattern_XXXXXX");

  for (int i = 0; i < log_size; i++) {
    if (match_log[i].kind == match_log::SUCCESS) {
      os << "Match succeeds\n";
    } else if (match_log[i].kind == match_log::FAIL) {
      os << "Subject:\n";
      if (i == 0) {
        printSortedConfigurationToFile(
            subject, (block *)match_log[i].subject, match_log[i].sort);
      } else {
        auto *subjectSort
            = debug_print_term((block *)match_log[i].subject, match_log[i].sort);
        auto strSubjectSort = std::string(subjectSort->data, len(subjectSort));
        subject_file.ofstream() << strSubjectSort << std::endl;
      }
      kllvm::printKORE(
          os, definition_path, subject_file.filename(), false, true);
      os << "does not match pattern: \n";
      pattern_file.ofstream() << match_log[i].pattern << std::endl;
      kllvm::printKORE(
          os, definition_path, pattern_file.filename(), false, true);
    } else if (match_log[i].kind == match_log::FUNCTION) {
      os << match_log[i].debug_name << "(";

      for (int j = 0; j < match_log[i].args.size(); j += 2) {
        auto *typeName = static_cast<char *>(match_log[i].args[j + 1]);
        printValueOfType(os, definition_path, match_log[i].args[j], typeName);
        if (j + 2 != match_log[i].args.size()) {
          os << ", ";
        }
      }
      os << ") => " << *static_cast<bool *>(match_log[i].result) << "\n";
    }
  }
}

void printValueOfType(
    std::ostream &os, std::string const &definition_path, void *value,
    std::string const &type) {
  if (type == "%mpz*") {
    os << static_cast<mpz_ptr>(value);
  } else if (type == "%block*") {
    if ((((uintptr_t)value) & 3) == 1) {
      auto f = temporary_file("subject_XXXXXX");
      string *s = printConfigurationToString(static_cast<block *>(value));
      f.ofstream() << std::string(s->data, len(s)) << std::endl;
      kllvm::printKORE(os, definition_path, f.filename(), false, true);
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

void printVariableToFile(FILE *file, char const *varname) {
  fmt::print(file, "{}", varname);
  char n = 0;
  fwrite(&n, 1, 1, file);
}
