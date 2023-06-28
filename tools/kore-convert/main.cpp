#include <kllvm/ast/AST.h>
#include <kllvm/binary/deserializer.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>
#include <kllvm/parser/location.h>

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/type_traits.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;
using namespace kllvm;
using namespace kllvm::parser;

enum kore_file_format {
  detect,
  text,
  binary,
};

cl::OptionCategory KoreConvertCat("kore-convert options");

cl::opt<std::string> InputFilename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(KoreConvertCat));

cl::opt<kore_file_format> InputFormat(
    "from", cl::desc("Specify input file format"),
    cl::values(
        clEnumVal(detect, "Detect input format automatically"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect), cl::cat(KoreConvertCat));

cl::opt<std::string> OutputFilename(
    "o", cl::desc("Specify output filename"), cl::value_desc("filename"),
    cl::init("-"), cl::cat(KoreConvertCat));

cl::opt<kore_file_format> OutputFormat(
    "to", cl::desc("Specify output file format"),
    cl::values(
        clEnumVal(detect, "Convert binary <=> text"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect), cl::cat(KoreConvertCat));

cl::opt<bool> ForceBinary(
    "F", cl::desc("Force binary output on stdout"), cl::cat(KoreConvertCat));

cl::opt<bool> NoHeader(
    "k",
    cl::desc(
        "Don't add the KORE header and version at the start of binary output"),
    cl::cat(KoreConvertCat));

cl::opt<bool> NoArity(
    "a",
    cl::desc(
        "Don't add the topmost constructor arity at the end of binary output"),
    cl::cat(KoreConvertCat));

cl::opt<bool> UseSize(
    "s",
    cl::desc("Emit size bytes for this pattern (useful if the resulting file "
             "will be embedded in a larger format, but will prohibit "
             "concatenation of terms)"),
    cl::cat(KoreConvertCat));

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

serializer::flags get_flags() {
  auto ret = serializer::NONE;

  if (NoHeader) {
    ret = static_cast<serializer::flags>(ret | serializer::DROP_HEADER);
  }

  if (NoArity) {
    ret = static_cast<serializer::flags>(ret | serializer::DROP_ARITY);
  }

  return ret;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KoreConvertCat});
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

  if (OutputFormat == text && NoArity) {
    std::cerr << "-a only applies to binary output\n"
              << "use --to=binary for binary input\n";
    return 3;
  }

  if (OutputFormat == binary && OutputFilename == "-" && !ForceBinary) {
    std::cerr << "Not outputting binary KORE to stdout\n"
              << "use -o to specify output file, or -F to force stdout\n";
    return 4;
  }

  if (OutputFormat == text) {
    dump_text(input);
    return 0;
  }

  if (OutputFormat == binary) {
    auto s = serializer(get_flags());
    input->serialize_to(s);

    if (UseSize) {
      s.correct_emitted_size();
    }

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
