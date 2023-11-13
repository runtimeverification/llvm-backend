#include <kllvm/binary/ProofTraceValidator.h>

namespace kllvm {

ProofTraceValidator::ProofTraceValidator(bool _verbose) : depth(0), verbose(_verbose) { }

bool ProofTraceValidator::validate_proof_trace(std::string const &filename) {
  auto data = file_contents(filename);

  auto ptr = data.begin();
  bool result = validate_trace(ptr, data.end());

  if (ptr != data.end()) {
    return false;
  }
  return result;
}

} // namespace kllvm
