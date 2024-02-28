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

cl::OptionCategory kore_convert_cat("kore-convert options");

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_convert_cat));

cl::opt<kore_file_format> input_format(
    "from", cl::desc("Specify input file format"),
    cl::values(
        clEnumVal(detect, "Detect input format automatically"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect), cl::cat(kore_convert_cat));

cl::opt<std::string> output_filename(
    "o", cl::desc("Specify output filename"), cl::value_desc("filename"),
    cl::init("-"), cl::cat(kore_convert_cat));

cl::opt<kore_file_format> output_format(
    "to", cl::desc("Specify output file format"),
    cl::values(
        clEnumVal(detect, "Convert binary <=> text"),
        clEnumVal(text, "Textual KORE"), clEnumVal(binary, "Binary KORE")),
    cl::init(detect), cl::cat(kore_convert_cat));

cl::opt<bool> force_binary(
    "F", cl::desc("Force binary output on stdout"), cl::cat(kore_convert_cat));

cl::opt<bool> no_header(
    "k",
    cl::desc(
        "Don't add the KORE header and version at the start of binary output"),
    cl::cat(kore_convert_cat));

cl::opt<bool> no_arity(
    "a",
    cl::desc(
        "Don't add the topmost constructor arity at the end of binary output"),
    cl::cat(kore_convert_cat));

cl::opt<bool> use_size(
    "s",
    cl::desc("Emit size bytes for this pattern (useful if the resulting file "
             "will be embedded in a larger format, but will prohibit "
             "concatenation of terms)"),
    cl::cat(kore_convert_cat));

sptr<kore_pattern> get_input_pattern() {
  auto get_text
      = [&]() { return kore_parser(input_filename.getValue()).pattern(); };
  auto get_binary = [&]() { return deserialize_pattern(input_filename); };

  switch (input_format) {
  case text: return get_text();
  case binary: return get_binary();

  case detect: {
    if (has_binary_kore_header(input_filename)) {
      input_format = binary;
      return get_binary();
    }
    input_format = text;
    return get_text();

    break;
  }
  }
}

void dump_text(sptr<kore_pattern> const &pat) {
  if (output_filename == "-") {
    pat->print(std::cout);
    std::cout << '\n';
  } else {
    auto os = std::ofstream(output_filename);
    pat->print(os);
    os << '\n';
  }
}

serializer::flags get_flags() {
  auto ret = serializer::NONE;

  if (no_header) {
    ret = static_cast<serializer::flags>(ret | serializer::DROP_HEADER);
  }

  if (no_arity) {
    ret = static_cast<serializer::flags>(ret | serializer::DROP_ARITY);
  }

  return ret;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_convert_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto input = get_input_pattern();
  if (!input) {
    std::cerr << "Invalid input pattern\n";
    return 1;
  }

  if (output_format == detect) {
    output_format = input_format == text ? binary : text;
  }

  if (output_format == text && no_header) {
    std::cerr << "-k only applies to binary output\n"
              << "use --to=binary for binary input\n";
    return 2;
  }

  if (output_format == text && no_arity) {
    std::cerr << "-a only applies to binary output\n"
              << "use --to=binary for binary input\n";
    return 3;
  }

  if (output_format == binary && output_filename == "-" && !force_binary) {
    std::cerr << "Not outputting binary KORE to stdout\n"
              << "use -o to specify output file, or -F to force stdout\n";
    return 4;
  }

  if (output_format == text) {
    dump_text(input);
    return 0;
  }

  if (output_format == binary) {
    auto s = serializer(get_flags());
    input->serialize_to(s);

    if (use_size) {
      s.correct_emitted_size();
    }

    auto output = [&](std::ostream &os) {
      for (auto byte : s.data()) {
        os << static_cast<uint8_t>(byte);
      }
    };

    if (output_filename == "-") {
      output(std::cout);
    } else {
      auto os = std::ofstream(output_filename, std::ios::binary);
      output(os);
    }

    return 0;
  }

  return 3;
}
