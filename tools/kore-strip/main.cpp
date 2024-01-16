#include <kllvm/binary/deserializer.h>
#include <kllvm/util/temporary_file.h>

#include <llvm/ADT/DenseMapInfo.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace llvm;

cl::OptionCategory KoreStripCat("kore-strip options");

cl::opt<bool> StripArity(
    "a",
    cl::desc(
        "Strip a single sequence of arity bytes from the end of the input"),
    cl::cat(KoreStripCat));

cl::opt<bool> StripHeader(
    "k",
    cl::desc("Strip the leading bytes (header, version and size for version "
             "1.2.0 onwards) from the input file"),
    cl::cat(KoreStripCat));

cl::opt<std::string> InputFilename(
    "i", cl::desc("Specify input filename"), cl::value_desc("filename"),
    cl::Required, cl::cat(KoreStripCat));

cl::opt<std::string> OutputFilename(
    "o", cl::desc("Specify output filename"), cl::value_desc("filename"),
    cl::init("-"), cl::cat(KoreStripCat));

std::FILE *check_fopen(char const *name, char const *mode) {
  auto *f = std::fopen(name, mode);
  if (!f) {
    auto str = std::stringstream{};
    str << "Could not open file " << name;
    std::perror(str.str().c_str());
    std::exit(1);
  }

  return f;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KoreStripCat});
  cl::ParseCommandLineOptions(argc, argv);

  auto *input = check_fopen(InputFilename.c_str(), "rb");

  std::fseek(input, 0, SEEK_END);
  auto file_size = std::ftell(input);

  auto end_skip_length = 0;
  auto begin_skip_length = 0;

  if (StripArity) {
    std::fseek(input, file_size - 9, SEEK_SET);
    auto buffer = std::vector<uint8_t>(9);
    auto read
        = std::fread(buffer.data(), sizeof(uint8_t), buffer.size(), input);
    if (read != buffer.size()) {
      std::cerr << "Failed to read last 9 bytes into buffer\n";
      return 1;
    }

    // Find the first prefix of the last 9 bytes such that the high bits form
    // a valid continued arity representation. That is, they must satisfy the
    // regex (1*)0.
    auto valid_at = [&buffer](int i) -> bool {
      auto all = true;

      for (auto j = i; j < buffer.size() - 1; ++j) {
        all = all && (buffer[j] & 0x80);
      }

      return all && !(buffer.back() & 0x80);
    };

    for (auto i = 0; i < buffer.size(); ++i) {
      if (valid_at(i)) {
        end_skip_length = 9 - i;
        break;
      }
    }

    if (end_skip_length == 0) {
      std::cerr << "No arity found at end of file; not stripping\n";
    }
  }

  if (StripHeader) {
    std::fseek(input, 5, SEEK_SET);
    auto buffer = std::vector<uint8_t>(6);
    auto read
        = std::fread(buffer.data(), sizeof(uint8_t), buffer.size(), input);
    if (read != buffer.size()) {
      std::cerr << "Failed to read 6-byte version into buffer\n";
    }

    auto begin = buffer.begin();
    auto version = kllvm::detail::read_version(begin, buffer.end());

    if (version >= kllvm::binary_version(1, 2, 0)) {
      begin_skip_length = 19;
    } else {
      begin_skip_length = 11;
    }
  }

  auto result_size = file_size - (begin_skip_length + end_skip_length);
  auto buffer = std::vector<uint8_t>(result_size);

  std::fseek(input, begin_skip_length, SEEK_SET);
  auto read = std::fread(buffer.data(), sizeof(uint8_t), result_size, input);
  if (read != result_size) {
    std::cerr << "Failed to read from temporary file\n";
    return 1;
  }

  if (OutputFilename == "-") {
    std::fwrite(buffer.data(), sizeof(uint8_t), result_size, stdout);
    std::fclose(input);
  } else {
    auto tmp_file = temporary_file("tmp.strip.XXXXXXXXXX");
    auto *file_pointer = tmp_file.file_pointer("wb");

    std::fwrite(buffer.data(), sizeof(uint8_t), result_size, file_pointer);
    std::fflush(file_pointer);

    tmp_file.rename(OutputFilename);
  }
}
