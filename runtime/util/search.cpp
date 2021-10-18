#include <cstdint>
#include <cstring>
#include <list>
#include <unordered_set>

#include "runtime/collect.h"
#include "runtime/header.h"

static std::vector<block *> stepResults;

extern "C" {

void addSearchResult(block *result) {
  stepResults.push_back(result);
}

void take_search_step(block *);
}

static std::list<block *> states;
static block *state;

static std::pair<
    std::vector<block **>::iterator, std::vector<block **>::iterator>
blockEnumerator() {
  static std::vector<block **> blocks;

  blocks.clear();

  for (auto &keyVal : states) {
    blocks.push_back(const_cast<block **>(&(keyVal)));
  }
  blocks.push_back(&state);
  for (auto &keyVal : stepResults) {
    blocks.push_back(const_cast<block **>(&(keyVal)));
  }

  return std::make_pair(blocks.begin(), blocks.end());
}

std::unordered_set<block *, HashBlock, KEq>
take_search_steps(int64_t depth, block *subject) {
  static int registered = -1;
  if (registered == -1) {
    registerGCRootsEnumerator(blockEnumerator);
  }

  std::unordered_set<block *, HashBlock, KEq> results;
  std::unordered_set<block *, HashBlock, KEq> states_set;
  states.clear();
  states_set.insert(subject);
  states.push_back(subject);
  while (!states.empty() && depth != 0) {
    state = states.front();
    states.pop_front();
    states_set.erase(state);
    if (depth > 0)
      depth--;
    stepResults.clear();
    take_search_step(state);
    if (stepResults.size() == 0) {
      results.insert(state);
    } else {
      for (block *result : stepResults) {
        auto dirty = states_set.insert(result);
        if (dirty.second) {
          states.push_back(result);
        }
      }
    }
  }
  if (depth == 0) {
    for (auto state : states) {
      results.insert(state);
    }
  }
  return results;
}
