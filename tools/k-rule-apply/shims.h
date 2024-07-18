#ifndef KLLVM_SHIMS_H
#define KLLVM_SHIMS_H

#include <cstddef>
#include <string>

struct match_log;

namespace kllvm {

class kore_pattern;

void *
construct_initial_configuration(kore_pattern const *pattern, void *handle);

void *reset_match_reason(void *handle);

match_log *getmatch_log(void *handle);

size_t getmatch_log_size(void *handle);

void *print_match_result(
    std::ostream &os, match_log *log, size_t logSize, std::string const &dir,
    void *handle);

void *init_static_objects(void *handle);

} // namespace kllvm

#endif
