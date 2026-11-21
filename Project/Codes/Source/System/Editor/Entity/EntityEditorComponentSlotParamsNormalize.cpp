#include "System/Editor/Entity/EntityEditorComponentSlotParamsNormalize.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"

namespace Atrum::EntityEditor {

nlohmann::json MergeParamsForRegisteredType(const std::string &typeName,
                                            const nlohmann::json &incoming) {
  if (!EntityEditorKnownComponentRegistry::Contains(typeName)) {
    if (incoming.is_object()) {
      return incoming;
    }
    return nlohmann::json::object();
  }
  nlohmann::json merged = DefaultParamsForType(typeName);
  if (!incoming.is_object() || incoming.empty()) {
    return merged;
  }
  for (const auto &[key, value] : incoming.items()) {
    merged[key] = value;
  }
  return merged;
}

void NormalizeKnownComponentSlotParams(ComponentSlot &slot) {
  slot.params = MergeParamsForRegisteredType(slot.typeName, slot.params);
}

} // namespace Atrum::EntityEditor