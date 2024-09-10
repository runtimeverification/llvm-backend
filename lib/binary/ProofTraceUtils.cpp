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

llvm_event *kllvm::build_post_function_event(
    sptr<kore_composite_pattern> current_config,
    sptr<llvm_function_event> &function_event, bool expand_terms) {
  sptr<kore_composite_pattern> new_config = nullptr;

  // The name of the function is actually the kore_symbol
  // corresponding to the function's constructor: function_name{...}
  // We need to extract only the function name to build the composite pattern
  auto function_name = function_event->get_name();
  auto const *delimiter = "{";
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
  // And it's a top level function.
  if (positions.size() == 1) {

    // Create a new configuration, set the ith argument to be replaced by the function or a pattern with it
    sptr<kore_composite_pattern> new_config
        = kore_composite_pattern::create(current_config->get_constructor());
    int index = positions[0];

    // Add the new pattern to the new configuration
    for (int i = 0; i < current_config->get_arguments().size(); i++) {
      auto argument
          = i == index ? function : current_config->get_arguments()[i];
      new_config->add_argument(argument);
    }

    // Get new configuration size
    std::stringstream ss;
    new_config->print(ss);
    auto new_config_size = ss.str().size();

    auto *new_config_event = new llvm_event();
    new_config_event->setkore_pattern(new_config, new_config_size);

    return new_config_event;
  }

  return nullptr;
}
