#include "kllvm/binary/ProofTraceUtils.h"

#include <kllvm/ast/AST.h>
#include <kllvm/binary/ProofTraceUtils.h>

using namespace kllvm;

std::vector<int> kllvm::parse_relative_location(std::string location) {
  if (location.empty()) {
    return {};
  }
  std::vector<int> positions;
  std::string delimiter = ":";
  size_t pos = 0;
  std::string token;
  while ((pos = location.find(delimiter)) != std::string::npos) {
    token = location.substr(0, pos);
    positions.push_back(std::stoi(token));
    location.erase(0, pos + delimiter.length());
  }
  positions.push_back(std::stoi(location));
  return positions;
}

sptr<kore_composite_pattern> kllvm::replace_argument(
    sptr<kore_composite_pattern> current_config,
    sptr<kore_composite_pattern> function, std::vector<int> positions) {

  // The function is in the rhs of a rewrite rule
  if (positions.empty()) {
    return nullptr;
  }
  // Create a new configuration, set the ith argument to be replaced by the function or a pattern with it
  auto new_config = kllvm::kore_composite_pattern::create(
      current_config->get_constructor());
  int index = positions[0];
  auto pattern = function;

  // If there function is still nested, we need to call replace the argument in the nested configuration
  if (positions.size() > 1) {
    positions.erase(positions.begin());
    auto config = std::dynamic_pointer_cast<kore_composite_pattern>(
        current_config->get_arguments()[index]);

    pattern = kllvm::replace_argument(config, function, positions);
  }

  // Add the new pattern to the new configuration
  for (int i = 0; i < current_config->get_arguments().size(); i++) {
    auto argument = i == index ? pattern : current_config->get_arguments()[i];
    new_config->add_argument(argument);
  }

  return new_config;
}

llvm_event *kllvm::build_post_function_event(
    sptr<kore_composite_pattern> current_config,
    sptr<llvm_function_event> function_event, bool expand_terms) {
  sptr<kore_composite_pattern> new_config = nullptr;

  // The name of the function is actually the kore_symbol
  // corresponding to the function's constructor: function_name{...}
  // We need to extract only the function name to build the composite pattern
  auto function_name = function_event->get_name();
  auto delimiter = "{";
  auto pos = function_name.find(delimiter);
  if (pos != std::string::npos) {
    function_name = function_name.substr(0, pos);
  }

  // Construct the composite pattern for the function
  auto function = sptr<kore_composite_pattern>(
      kore_composite_pattern::create(function_name));
  for (auto const &arg : function_event->get_arguments()) {
    function->add_argument(arg.getkore_pattern());
  }

  // Construct location
  std::string location = function_event->get_relative_position();
  std::vector<int> positions = kllvm::parse_relative_location(location);

  // We can only replace the argument and build a new configuration if we have a location
  if (!positions.empty()) {
    auto new_config = replace_argument(current_config, function, positions);

    // Get new configuration size
    std::stringstream ss;
    new_config->print(ss);
    auto new_config_size = ss.str().size();

    llvm_event *new_config_event = new llvm_event();
    new_config_event->setkore_pattern(new_config, new_config_size);

    return new_config_event;
  }

  return nullptr;
}
