#include <kllvm/ast/AST.h>

#include <string>
#include <unordered_set>

namespace kllvm {

std::unordered_set<std::string>
KOREDefinition::getSortsHookedTo(std::string const &hookName) const {
  auto ret = std::unordered_set<std::string>{};

  for (auto const &[name, decl] : getSortDeclarations()) {
    if (decl->isHooked()) {
      if (auto hook = decl->getStringAttribute("hook"); hook == hookName) {
        ret.insert(name);
      }
    }
  }

  return ret;
}

} // namespace kllvm
