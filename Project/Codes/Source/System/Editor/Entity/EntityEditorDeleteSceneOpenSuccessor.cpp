#include "System/Editor/Entity/EntityEditorDeleteSceneOpenSuccessor.h"

namespace Atrum::EntityEditor::EntityEditorDeleteSceneOpenSuccessor {

std::optional<std::string>
ResolveAfterDelete(const std::vector<std::string> &sortedBeforeDelete,
                   const std::string &deletedPath) {
  if (deletedPath.empty()) {
    return std::nullopt;
  }

  size_t deletedIndex = sortedBeforeDelete.size();
  for (size_t i = 0; i < sortedBeforeDelete.size(); ++i) {
    if (sortedBeforeDelete[i] == deletedPath) {
      deletedIndex = i;
      break;
    }
  }
  if (deletedIndex >= sortedBeforeDelete.size()) {
    return std::nullopt;
  }

  std::vector<std::string> remaining;
  remaining.reserve(sortedBeforeDelete.size() - 1);
  for (const std::string &path : sortedBeforeDelete) {
    if (path != deletedPath) {
      remaining.push_back(path);
    }
  }
  if (remaining.empty()) {
    return std::nullopt;
  }

  if (deletedIndex < remaining.size()) {
    return remaining[deletedIndex];
  }
  return remaining[deletedIndex - 1];
}

} // namespace Atrum::EntityEditor::EntityEditorDeleteSceneOpenSuccessor