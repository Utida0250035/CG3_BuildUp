#include "System/Editor/Entity/EntityEditorComponentSlotParamsNormalize.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"

namespace Atrum::EntityEditor {

void NormalizeKnownComponentSlotParams(ComponentSlot &slot) {
  if (!EntityEditorKnownComponentRegistry::Contains(slot.typeName)) {
    return;
  }
  const nlohmann::json defaults = DefaultParamsForType(slot.typeName);
  if (!defaults.is_object() || defaults.empty()) {
    return;
  }
  if (!slot.params.is_object()) {
    slot.params = defaults;
    return;
  }
  if (slot.params.empty()) {
    slot.params = defaults;
    return;
  }
  for (const auto &[key, value] : defaults.items()) {
    if (!slot.params.contains(key)) {
      slot.params[key] = value;
    }
  }
}

} // namespace Atrum::EntityEditor