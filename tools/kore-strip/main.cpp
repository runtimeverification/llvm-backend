#include <llvm/Support/CommandLine.h>

#include <array>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

using namespace llvm;

cl::opt<bool> StripArity(
    "a",
    cl::desc(
        "Strip a single sequence of arity bytes from the end of the input"));

cl::opt<bool> StripHeader(
    "k",
    cl::desc(
        "Strip the leading 11 bytes (header and version) from the input file"));

cl::opt<std::string> InputFilename(
    "i", cl::desc("Specify input filename"), cl::value_desc("filename"),
    cl::Required);

cl::opt<std::string> OutputFilename(
    "o", cl::desc("Specify output filename"), cl::value_desc("filename"),
    cl::init("-"));

void check_file_open(std::FILE *f, char const *name) {
  if (!f) {
    auto str = std::stringstream{};
    str << "Could not open file " << name;
    std::perror(str.str().c_str());
    std::exit(1);
  }
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  auto input = std::fopen(InputFilename.c_str(), "rb");

  if (OutputFilename == "-") {
    OutputFilename = InputFilename.getValue();
  }

  std::fseek(input, 0, SEEK_END);
  auto file_size = std::ftell(input);

  auto end_skip_length = 0;
  auto begin_skip_length = 0;

  if (StripArity) {
    std::fseek(input, file_size - 9, SEEK_SET);
    auto buffer = std::vector<uint8_t>(9);
    std::fread(buffer.data(), sizeof(uint8_t), buffer.size(), input);

    for (auto it = buffer.rbegin(); it != buffer.rend(); ++it) {
      auto cont = *it & 0x80;

      if (!cont && it != buffer.rbegin()) {
        break;
      }

      ++end_skip_length;
    }
  }

  if (StripHeader) {
    begin_skip_length = 11;
  }

  auto temp_file_name = std::array<char, L_tmpnam>{};
  std::tmpnam(temp_file_name.data());
  auto output = std::fopen(temp_file_name.data(), "wb");
  if (!output) {
  }

  auto result_size = file_size - (begin_skip_length + end_skip_length);
  auto buffer = std::vector<uint8_t>(result_size);

  std::fseek(input, begin_skip_length, SEEK_SET);
  std::fread(buffer.data(), sizeof(uint8_t), result_size, input);
  std::fwrite(buffer.data(), sizeof(uint8_t), result_size, output);

  std::fclose(input);
  std::fclose(output);

  std::rename(temp_file_name.data(), OutputFilename.c_str());
}
