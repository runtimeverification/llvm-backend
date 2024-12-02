#include <cstdint>
#include <cstring>
#include <list>
#include <unordered_set>

#include "runtime/collect.h"
#include "runtime/header.h"

static std::vector<block *> step_results;

extern "C" {

void add_search_result(block *result) {
  step_results.push_back(result);
}

void take_search_step(block *);
}

static std::list<block *> states;
static block *state;
static std::unordered_set<block *, hash_block, k_eq> states_set;
static std::unordered_set<block *, hash_block, k_eq> results;

static std::pair<
    std::vector<block **>::iterator, std::vector<block **>::iterator>
block_enumerator() {
  // NOLINTBEGIN(*-const-cast)
  static std::vector<block **> blocks;

  blocks.clear();

  for (auto &key_val : states) {
    blocks.push_back(const_cast<block **>(&(key_val)));
  }
  blocks.push_back(&state);
  for (auto &key_val : step_results) {
    blocks.push_back(const_cast<block **>(&(key_val)));
  }

  for (auto const &key_val : states_set) {
    blocks.push_back(const_cast<block **>(&(key_val)));
  }

  for (auto const &key_val : results) {
    blocks.push_back(const_cast<block **>(&(key_val)));
  }

  return std::make_pair(blocks.begin(), blocks.end());
  // NOLINTEND(*-const-cast)
}

// NOLINTNEXTLINE(*-cognitive-complexity)
std::unordered_set<block *, hash_block, k_eq> take_search_steps(
    bool execute_to_branch, int64_t depth, int64_t bound, block *subject) {
  static int registered = -1;
  if (registered == -1) {
    register_gc_roots_enumerator(block_enumerator);
  }

  states.clear();
  states_set.clear();
  results.clear();

  if (bound == 0) {
    return results;
  }

  states_set.insert(subject);
  states.push_back(subject);

  while (!states.empty() && depth != 0) {
    state = states.front();
    states.pop_front();
    if (states.empty()) {
      states_set.clear();
    } else {
      states_set.erase(state);
    }

    if (depth > 0) {
      depth--;
    }

    step_results.clear();
    take_search_step(state);

    if (execute_to_branch && step_results.size() > 1) {
      results.insert(state);
      return results;
    }
    if (step_results.empty()) {
      results.insert(state);
      if (results.size() == bound) {
        return results;
      }
    } else {
      for (block *result : step_results) {
        auto dirty = states_set.insert(result);
        if (dirty.second) {
          states.push_back(result);
        }
      }
    }
  }

  if (depth == 0) {
    for (auto *state : states) {
      results.insert(state);
      if (results.size() == bound) {
        return results;
      }
    }
  }

  return results;
}
