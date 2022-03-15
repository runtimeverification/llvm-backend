#include "kllvm/ast/AST.h"
#include "kllvm/binary/deserializer.h"
#include "kllvm/binary/serializer.h"
#include "kllvm/parser/KOREParser.h"

#include <llvm/Support/CommandLine.h>

#include <algorithm>
#include <fstream>
#include <iostream>

using namespace llvm;
using namespace kllvm;
using namespace kllvm::parser;

enum kore_file_format {
  detect,
  text,
  binary,
};

cl::opt<std::string>
    InputFilename(cl::Positional, cl::desc("<input file>"), cl::Required);

cl::opt<kore_file_format> InputFormat(
    "from", cl::desc("Specify input file format"),
    cl::values(
        clEnumVal(detect, "Detect input format automatically"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect));

cl::opt<std::string> OutputFilename(
    "o", cl::desc("Specify output filename"), cl::value_desc("filename"),
    cl::init("-"));

cl::opt<kore_file_format> OutputFormat(
    "to", cl::desc("Specify output file format"),
    cl::values(
        clEnumVal(detect, "Convert binary <=> text"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect));

cl::opt<bool> ForceBinary("F", cl::desc("Force binary output on stdout"));

cl::opt<bool> NoHeader(
    "k",
    cl::desc(
        "Don't add the KORE header and version at the start of binary output"));

sptr<KOREPattern> get_input_pattern() {
  auto get_text = [&]() { return KOREParser(InputFilename).pattern(); };
  auto get_binary = [&]() { return deserialize_pattern(InputFilename); };

  switch (InputFormat) {
  case text: return get_text();
  case binary: return get_binary();

  case detect: {
    if (has_binary_kore_header(InputFilename)) {
      InputFormat = binary;
      return get_binary();
    } else {
      InputFormat = text;
      return get_text();
    }

    break;
  }
  }
}

void dump_text(sptr<KOREPattern> pat) {
  if (OutputFilename == "-") {
    pat->print(std::cout);
    std::cout << '\n';
  } else {
    auto os = std::ofstream(OutputFilename);
    pat->print(os);
    os << '\n';
  }
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  auto input = get_input_pattern();
  if (!input) {
    std::cerr << "Invalid input pattern\n";
    return 1;
  }

  if (OutputFormat == detect) {
    OutputFormat = InputFormat == text ? binary : text;
  }

  if (OutputFormat == text && NoHeader) {
    std::cerr << "-k only applies to binary output\n"
              << "use --to=binary for binary input\n";
    return 2;
  }

  if (OutputFormat == binary && OutputFilename == "-" && !ForceBinary) {
    std::cerr << "Not outputting binary KORE to stdout\n"
              << "use -o to specify output file, or -F to force stdout\n";
    return 3;
  }

  if (OutputFormat == text) {
    dump_text(input);
    return 0;
  }

  if (OutputFormat == binary) {
    auto s = serializer(!NoHeader);
    input->serialize_to(s);

    auto output = [&](std::ostream &os) {
      for (auto byte : s.data()) {
        os << static_cast<uint8_t>(byte);
      }
    };

    if (OutputFilename == "-") {
      output(std::cout);
    } else {
      auto os = std::ofstream(OutputFilename, std::ios::binary);
      output(os);
    }

    return 0;
  }

  return 3;
}
