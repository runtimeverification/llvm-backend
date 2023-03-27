#include <fmt/compile.h>
#include <fmt/format.h>

#include <llvm/Support/CommandLine.h>

#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace llvm;

cl::OptionCategory KoreEmbedCat("kore-embed options");

cl::opt<std::string> TargetFile(
    cl::Positional, cl::desc("filename"), cl::Required, cl::cat(KoreEmbedCat));

cl::opt<std::string> VariableName(
    "name",
    cl::desc(
        "Name for variables in generated code, and for generated filenames"),
    cl::init("data"), cl::cat(KoreEmbedCat));

cl::opt<std::string> Extension(
    "ext", cl::desc("Extension to use for generated files"), cl::init("c"),
    cl::cat(KoreEmbedCat));

cl::opt<bool> NoHeader(
    "no-header", cl::desc("Don't generate a header file"), cl::init(false),
    cl::cat(KoreEmbedCat));

std::string data_variable() {
  return fmt::format("char {}[]", VariableName);
}

std::string size_variable() {
  return fmt::format("int {}_size", VariableName);
}

void print_header(std::FILE *f) {
  using namespace fmt::literals;

  fmt::print(
      f, R"(
#ifndef GENERATED_{name}_H
#define GENERATED_{name}_H

#ifdef __cplusplus
extern "C" {{
#endif

extern {data};
extern {size};

#ifdef __cplusplus
}}
#endif

#endif
)",
      "name"_a = VariableName, "data"_a = data_variable(),
      "size"_a = size_variable());
}

void print_implementation(std::FILE *in_f, std::FILE *out_f) {
  fmt::print(out_f, R"(
#ifdef __cplusplus
extern "C" {{
#endif
)");

  fmt::print(out_f, "\n{} = {{", data_variable());

  constexpr auto buffer_size = std::size_t{4096};
  auto buffer = std::array<char, buffer_size>{};

  auto len = 0;
  auto comma = "";

  while (true) {
    auto read = std::fread(buffer.data(), 1, buffer_size, in_f);

    for (auto i = 0u; i < read; ++i) {
      fmt::print(out_f, "{}'\\x{:02x}'", comma, buffer[i]);
      comma = ",";
    }

    len += read;

    if (read != buffer_size) {
      break;
    }
  }

  fmt::print(out_f, "}};\n");
  fmt::print(out_f, "{} = {};\n", size_variable(), len);

  fmt::print(out_f, R"(
#ifdef __cplusplus
}}
#endif
)");
}

FILE *check_fopen(std::string const &path, std::string const &mode) {
  FILE *out_f = std::fopen(path.c_str(), mode.c_str());
  if (!out_f) {
    std::perror(fmt::format("Failed to open {}", path).c_str());
    std::exit(1);
  }
  return out_f;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KoreEmbedCat});
  cl::ParseCommandLineOptions(argc, argv);

  auto header_path = std::string(fmt::format("{}.h", VariableName));
  auto impl_path = std::string(fmt::format("{}.{}", VariableName, Extension));

  FILE *header_f = check_fopen(header_path.c_str(), "w");
  FILE *in_f = check_fopen(TargetFile.c_str(), "rb");
  FILE *impl_f = check_fopen(impl_path.c_str(), "w");

  print_header(header_f);
  print_implementation(in_f, impl_f);

  for (auto f : {header_f, impl_f, in_f}) {
    std::fclose(f);
  }
}
