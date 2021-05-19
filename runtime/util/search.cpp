#include <cstdint>
#include <cstring>
#include <list>
#include <unordered_set>

#include "runtime/header.h"
#include "runtime/collect.h"

extern "C" {

void addSearchResult(block *result, block ***bufPtr, uint64_t *count, uint64_t *capacity) {
  if (*count == *capacity) {
    size_t len = *count * sizeof(block *) * 2;
    block **newBuf = (block **)koreAllocAlwaysGC(len);
    memcpy(newBuf, *bufPtr, len);
    *bufPtr = newBuf;
    *capacity = *capacity * 2;
  }
  (*bufPtr)[*count] = result;
  (*count)++;
}

block** take_search_step(block *, uint64_t *);

}

static std::list<block *> states;
static block *state;

static std::pair<std::vector<block **>::iterator, std::vector<block **>::iterator> blockEnumerator() {
  static std::vector<block **> blocks;

  blocks.clear();

  for (auto &keyVal : states) {
    blocks.push_back(const_cast<block**>(&(keyVal)));
  }
  blocks.push_back(&state);

  return std::make_pair(blocks.begin(), blocks.end());
}

std::unordered_set<block *, HashBlock, KEq> take_search_steps(int64_t depth, block *subject) {
  static int registered = -1;
  if (registered == -1) {
    registerGCRootsEnumerator(blockEnumerator);
  }

  std::unordered_set<block *, HashBlock, KEq> results;
  std::unordered_set<block *, HashBlock, KEq> states_set;
  states.clear();
  states_set.insert(subject);
  states.push_back(subject);
  while(!states.empty() && depth != 0) {
    state = states.front();
    states.pop_front();
    states_set.erase(state);
    if (depth > 0) depth--;
    uint64_t count;
    block **stepResults = take_search_step(state, &count);
    if (count == 0) {
      results.insert(state);
    } else {
      for (uint64_t i = 0; i < count; i++) {
        auto dirty = states_set.insert(stepResults[i]);
        if (dirty.second) {
          states.push_back(stepResults[i]);
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
